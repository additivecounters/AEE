#include <stdlib.h>
#include <stdio.h>
#include "SpaceSavingHeapAEE.hpp"

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

SpaceSavingHeapAEE::SpaceSavingHeapAEE(float fPhi, int seed) : bob(seed)
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

	AEE_Counter::set_seed(seed);
}

SpaceSavingHeapAEE::~SpaceSavingHeapAEE()
{
	free(hashtable);
	free(counters);
}

int SpaceSavingHeapAEE::hash_item(LCLitem_t item)
{
	return (int)hash31(hasha, hashb, item) % hashsize;
}


void SpaceSavingHeapAEE::LCL_RebuildHash()
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
		pt->next = hashtable[hash_item(counters[i].item)];
		if (pt->next)
			pt->next->prev = pt;
		hashtable[hash_item(counters[i].item)] = pt;
	}
}

void SpaceSavingHeapAEE::Heapify(int ptr)
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
			((counters[ptr << 1].count.return_actual_val() < counters[(ptr << 1) + 1].count.return_actual_val()) ? 0 : 1);
		minchild = &counters[mc];
		// compute which child is the lesser of the two

		if (cpt->count.return_actual_val() < minchild->count.return_actual_val()) break;
		// if the parent is less than the smallest child, we can stop

		tmp = *cpt;
		*cpt = *minchild;
		*minchild = tmp;
		// else, swap the parent and child in the heap

		if (hash_item(cpt->item) == hash_item(minchild->item))
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
					hashtable[hash_item(cpt->item)] = cpt; // put in pointer from hashtable
			}
			else
				cpt->prev->next = cpt;
			if (cpt->next)
				cpt->next->prev = cpt; // place in linked list

			if (!minchild->prev) // also fix up the child
				hashtable[hash_item(minchild->item)] = minchild;
			else
				minchild->prev->next = minchild;
			if (minchild->next)
				minchild->next->prev = minchild;
		}
		ptr = mc;
		// continue on with the heapify from the child position
	}
}

SpaceSavingHeapAEE::LCLCounter * SpaceSavingHeapAEE::LCL_FindItem(LCLitem_t item)
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

void SpaceSavingHeapAEE::increment_and_cond_divide(LCLCounter* hashptr)
{
	hashptr->count.increase_val();
	if (AEE_Counter::Should_I_divide)
	{
		AEE_Counter::Should_I_divide = false;
		for (int i = 0; i <= size; i++)
		{
			if (hashptr != (counters + i))
			{
				counters[i].count.divide_counter();
			}
		}
	}
}

void SpaceSavingHeapAEE::Probabilistic_Increment(const FTitem_t ft_item, LCLweight_t value)
{
	if (AEE_Counter::sample_or_advance())
	{
		LCL_Update(ft_item, value);
	}
	return;
}

void SpaceSavingHeapAEE::LCL_Update(const FTitem_t ft_item, LCLweight_t value)
{

	assert(value == 1 && "Only increment by 1 supported!");

	LCLitem_t item = bob.run(ft_item, FT_SIZE);

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

			//hashptr->count += value; // increment the count of the item
			increment_and_cond_divide(hashptr);

			Heapify(hashptr - counters); // and fix up the heap
			return;
		}
		else hashptr = hashptr->next;
	}
	// if control reaches here, then we have failed to find the item
	// so, overwrite smallest heap item and reheapify if necessary
	// fix up linked list from hashtable
	if (!root->prev) // if it is first in its list
		hashtable[hash_item(root->item)] = root->next;
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
	//root->hash = hashval;
	//root->delta = root->count.query();
	// update the implicit lower bound on the items frequency
	//  value+=root->delta;
	// update the upper bound on the items frequency

	//root->count = value + root->delta;
	increment_and_cond_divide(root);


	Heapify(1); // restore heap property if needed
	// return value;
}

int SpaceSavingHeapAEE::LCL_Size()
{ // return the size of the data structure in bytes
	return sizeof(this) + (hashsize * sizeof(int)) +
		(size * sizeof(LCLCounter));
}

LCLweight_t SpaceSavingHeapAEE::LCL_PointEst(const FTitem_t ft_item) 
{ // estimate the count of a particular item
	LCLitem_t item = bob.run(ft_item, FT_SIZE);

	LCLCounter * i;
	i = LCL_FindItem(item);
	if (i)
		return(i->count.query());
	else
		return 0;
}

/*
LCLweight_t SpaceSavingHeapAEE::LCL_PointErr(const FTitem_t ft_item)
{ // estimate the worst case error in the estimate of a particular item
	LCLitem_t item = bob.run(ft_item, FT_SIZE);

	LCLCounter * i;
	i = LCL_FindItem(item);
	if (i)
		return(i->delta);
	else
		return root->delta;
}
*/

int SpaceSavingHeapAEE::LCL_cmp(const void * a, const void * b) {
	LCLCounter * x = (LCLCounter*)a;
	LCLCounter * y = (LCLCounter*)b;
	if (x->count.return_actual_val() < y->count.return_actual_val()) return -1;
	else if (x->count.return_actual_val() > y->count.return_actual_val()) return 1;
	else return 0;
}

void SpaceSavingHeapAEE::LCL_Output() { // prepare for output
	if (counters->item == 0) {
		qsort(&counters[1], size, sizeof(LCLCounter), this->LCL_cmp);
		LCL_RebuildHash();
		counters->item = 1;
	}
}

std::map<uint32_t, uint32_t> SpaceSavingHeapAEE::LCL_Output(int thresh)
{
	std::map<uint32_t, uint32_t> res;

	for (int i = 1; i <= size; ++i)
	{
		if (counters[i].count.query() >= thresh)
			res.insert(std::pair<uint32_t, uint32_t>(counters[i].item, (uint32_t) counters[i].count.query()));
	}

	return res;
}

void SpaceSavingHeapAEE::LCL_CheckHash(int item, int hash)
{ // debugging routine to validate the hash table
	int i;
	LCLCounter * hashptr, *prev;

	for (i = 0; i < hashsize; i++)
	{
		prev = NULL;
		hashptr = hashtable[i];
		while (hashptr) {
			if (hash_item(hashptr->item) != i)
			{
				printf("\n Hash violation! hash = %d, should be %d \n",
					hash_item(hashptr->item), i);
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

void SpaceSavingHeapAEE::LCL_ShowHash()
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
				hash_item(hashptr->item),
				(int)hashptr->prev);
			hashptr = hashptr->next;
		}
		printf(" *** \n");
	}
}


void SpaceSavingHeapAEE::LCL_ShowHeap()
{ // debugging routine to show the heap
	int i, j;

	j = 1;
	for (i = 1; i <= size; i++)
	{
		printf("%d ", (int)counters[i].count.query());
		if (i == j)
		{
			printf("\n");
			j = 2 * j + 1;
		}
	}
	printf("\n\n");
}

