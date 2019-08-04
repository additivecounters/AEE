#include <stdlib.h>
#include <stdio.h>
#include "SpaceSavingHeap.hpp"

/********************************************************************
Implementation of Lazy Lossy Counting algorithm to Find Frequent Items
Based on the paper of Manku and Motwani, 2002
And Metwally, Agrwawal and El Abbadi, 2005
Implementation by G. Cormode 2002, 2003, 2005
This implements the space saving algorithm, which
guarantees 1/epsilon space.
This implementation uses a heap to track which is the current smallest count

Original Code: 2002-11
This version: 2002,2003,2005,2008

This work is licensed under the Creative Commons
Attribution-NonCommercial License. To view a copy of this license,
visit http://creativecommons.org/licenses/by-nc/1.0/ or send a letter
to Creative Commons, 559 Nathan Abbott Way, Stanford, California
94305, USA.
*********************************************************************/

#define LCL_NULLITEM 0x7FFFFFFF
// 2^31 -1 as a special character

SpaceSavingHeap::SpaceSavingHeap(float fPhi)
{
	int i;
	int k = 1 + (int) 1.0 / fPhi;

	// needs to be odd so that the heap always has either both children or 
	// no children present in the data structure

	size = (1 + k) | 1; // ensure that size is odd
	hashsize = LCL_HASHMULT * size;
	hashtable = (LCLCounter **)calloc(hashsize, sizeof(LCLCounter*));
	counters = (LCLCounter*)calloc(1 + size, sizeof(LCLCounter));
	// indexed from 1, so add 1

	hasha = 151261303;
	hashb = 6722461; // hard coded constants for the hash table,
	//should really generate these randomly
	n = (LCLweight_t)0;

	for (i = 1; i <= size; i++)
	{
		counters[i].next = NULL;
		counters[i].prev = NULL;
		counters[i].item = LCL_NULLITEM;
		// initialize items and counters to zero
	}
	root = &counters[1]; // put in a pointer to the top of the heap
}

SpaceSavingHeap::~SpaceSavingHeap()
{
	free(hashtable);
	free(counters);
}

void SpaceSavingHeap::LCL_RebuildHash()
{
	// rebuild the hash table and linked list pointers based on current
	// contents of the counters array
	int i;
	LCLCounter * pt;

	for (i = 0; i < hashsize; i++)
		hashtable[i] = 0;
	// first, reset the hash table
	for (i = 1; i <= size; i++) {
		counters[i].next = NULL;
		counters[i].prev = NULL;
	}
	// empty out the linked list
	for (i = 1; i <= size; i++) { // for each item in the data structure
		pt = &counters[i];
		pt->next = hashtable[counters[i].hash];
		if (pt->next)
			pt->next->prev = pt;
		hashtable[counters[i].hash] = pt;
	}
}

void SpaceSavingHeap::Heapify(int ptr)
{ // restore the heap condition in case it has been violated
	LCLCounter tmp;
	LCLCounter * cpt, *minchild;
	int mc;

	while (1)
	{
		if ((ptr << 1) + 1 > size) break;
		// if the current node has no children

		cpt = &counters[ptr]; // create a current pointer
		mc = (ptr << 1) +
			((counters[ptr << 1].count < counters[(ptr << 1) + 1].count) ? 0 : 1);
		minchild = &counters[mc];
		// compute which child is the lesser of the two

		if (cpt->count < minchild->count) break;
		// if the parent is less than the smallest child, we can stop

		tmp = *cpt;
		*cpt = *minchild;
		*minchild = tmp;
		// else, swap the parent and child in the heap

		if (cpt->hash == minchild->hash)
			// test if the hash value of a parent is the same as the 
			// hash value of its child
		{
			// swap the prev and next pointers back. 
			// if the two items are in the same linked list
			// this avoids some nasty buggy behaviour
			minchild->prev = cpt->prev;
			cpt->prev = tmp.prev;
			minchild->next = cpt->next;
			cpt->next = tmp.next;
		}
		else { // ensure that the pointers in the linked list are correct
		 // check: hashtable has correct pointer (if prev ==0)
			if (!cpt->prev) { // if there is no previous pointer
				if (cpt->item != LCL_NULLITEM)
					hashtable[cpt->hash] = cpt; // put in pointer from hashtable
			}
			else
				cpt->prev->next = cpt;
			if (cpt->next)
				cpt->next->prev = cpt; // place in linked list

			if (!minchild->prev) // also fix up the child
				hashtable[minchild->hash] = minchild;
			else
				minchild->prev->next = minchild;
			if (minchild->next)
				minchild->next->prev = minchild;
		}
		ptr = mc;
		// continue on with the heapify from the child position
	}
}

SpaceSavingHeap::LCLCounter * SpaceSavingHeap::LCL_FindItem(LCLitem_t item)
{ // find a particular item in the date structure and return a pointer to it
	LCLCounter * hashptr;
	int hashval;

	hashval = (int)hash31(hasha, hashb, item) % hashsize;
	hashptr = hashtable[hashval];
	// compute the hash value of the item, and begin to look for it in 
	// the hash table

	while (hashptr) {
		if (hashptr->item == item)
			break;
		else hashptr = hashptr->next;
	}
	return hashptr;
	// returns NULL if we do not find the item
}

void SpaceSavingHeap::LCL_Update(LCLitem_t item, LCLweight_t value)
{
	int hashval;
	LCLCounter * hashptr;
	// find whether new item is already stored, if so store it and add one
	// update heap property if necessary

	n += value;
	counters->item = 0; // mark data structure as 'dirty'

	hashval = (int)hash31(hasha, hashb, item) % hashsize;
	hashptr = hashtable[hashval];
	// compute the hash value of the item, and begin to look for it in 
	// the hash table

	while (hashptr) {
		if (hashptr->item == item) {
			hashptr->count += value; // increment the count of the item
			Heapify(hashptr - counters); // and fix up the heap
			return;
		}
		else hashptr = hashptr->next;
	}
	// if control reaches here, then we have failed to find the item
	// so, overwrite smallest heap item and reheapify if necessary
	// fix up linked list from hashtable
	if (!root->prev) // if it is first in its list
		hashtable[root->hash] = root->next;
	else
		root->prev->next = root->next;
	if (root->next) // if it is not last in the list
		root->next->prev = root->prev;
	// update the hash table appropriately to remove the old item

	// slot new item into hashtable
	hashptr = hashtable[hashval];
	root->next = hashptr;
	if (hashptr)
		hashptr->prev = root;
	hashtable[hashval] = root;
	// we overwrite the smallest item stored, so we look in the root
	root->prev = NULL;
	root->item = item;
	root->hash = hashval;
	root->delta = root->count;
	// update the implicit lower bound on the items frequency
	//  value+=root->delta;
	// update the upper bound on the items frequency
	root->count = value + root->delta;
	Heapify(1); // restore heap property if needed
	// return value;
}

int SpaceSavingHeap::LCL_Size()
{ // return the size of the data structure in bytes
	return sizeof(this) + (hashsize * sizeof(int)) +
		(size * sizeof(LCLCounter));
}

LCLweight_t SpaceSavingHeap::LCL_PointEst(LCLitem_t item)
{ // estimate the count of a particular item
	LCLCounter * i;
	i = LCL_FindItem(item);
	if (i)
		return(i->count);
	else
		return 0;
}

LCLweight_t SpaceSavingHeap::LCL_PointErr(LCLitem_t item)
{ // estimate the worst case error in the estimate of a particular item
	LCLCounter * i;
	i = LCL_FindItem(item);
	if (i)
		return(i->delta);
	else
		return root->delta;
}

int SpaceSavingHeap::LCL_cmp(const void * a, const void * b) {
	LCLCounter * x = (LCLCounter*)a;
	LCLCounter * y = (LCLCounter*)b;
	if (x->count < y->count) return -1;
	else if (x->count > y->count) return 1;
	else return 0;
}

void SpaceSavingHeap::LCL_Output() { // prepare for output
	if (counters->item == 0) {
		qsort(&counters[1], size, sizeof(LCLCounter), this->LCL_cmp);
		LCL_RebuildHash();
		counters->item = 1;
	}
}

std::map<uint32_t, uint32_t> SpaceSavingHeap::LCL_Output(int thresh)
{
	std::map<uint32_t, uint32_t> res;

	for (int i = 1; i <= size; ++i)
	{
		if (counters[i].count >= thresh)
			res.insert(std::pair<uint32_t, uint32_t>(counters[i].item, counters[i].count));
	}

	return res;
}

void SpaceSavingHeap::LCL_CheckHash(int item, int hash)
{ // debugging routine to validate the hash table
	int i;
	LCLCounter * hashptr, *prev;

	for (i = 0; i < hashsize; i++)
	{
		prev = NULL;
		hashptr = hashtable[i];
		while (hashptr) {
			if (hashptr->hash != i)
			{
				printf("\n Hash violation! hash = %d, should be %d \n",
					hashptr->hash, i);
				printf("after inserting item %d with hash %d\n", item, hash);
			}
			if (hashptr->prev != prev)
			{
				printf("\n Previous violation! prev = %d, should be %d\n",
					(int)hashptr->prev, (int)prev);
				printf("after inserting item %d with hash %d\n", item, hash);
				exit(EXIT_FAILURE);
			}
			prev = hashptr;
			hashptr = hashptr->next;
		}
	}
}

void SpaceSavingHeap::LCL_ShowHash()
{ // debugging routine to show the hashtable
	int i;
	LCLCounter * hashptr;

	for (i = 0; i < hashsize; i++)
	{
		printf("%d:", i);
		hashptr = hashtable[i];
		while (hashptr) {
			printf(" %d [h(%u) = %d, prev = %d] ---> ", (int)hashptr,
				(unsigned int)hashptr->item,
				hashptr->hash,
				(int)hashptr->prev);
			hashptr = hashptr->next;
		}
		printf(" *** \n");
	}
}


void SpaceSavingHeap::LCL_ShowHeap()
{ // debugging routine to show the heap
	int i, j;

	j = 1;
	for (i = 1; i <= size; i++)
	{
		printf("%d ", (int)counters[i].count);
		if (i == j)
		{
			printf("\n");
			j = 2 * j + 1;
		}
	}
	printf("\n\n");
}

