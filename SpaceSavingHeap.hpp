// lossycount.h -- header file for Lossy Counting
// see Manku & Motwani, VLDB 2002 for details
// implementation by Graham Cormode, 2002,2003

#ifndef LOSSYCOUNTING_h
#define LOSSYCOUNTING_h

#include "prng.h"

/////////////////////////////////////////////////////////
#define LCLweight_t int
//#define LCL_SIZE 101 // size of k, for the summary
// if not defined, then it is dynamically allocated based on user parameter
////////////////////////////////////////////////////////

#define LCLitem_t uint32_t

class SpaceSavingHeap {

	typedef struct lclcounter_t
	{
		LCLitem_t item; // item identifier
		int hash; // its hash value
		LCLweight_t count; // (upper bound on) count for the item
		LCLweight_t delta; // max possible error in count for the value
		lclcounter_t *prev, *next; // pointers in doubly linked list for hashtable
	}LCLCounter; // 32 bytes

	#define LCL_HASHMULT 3  // how big to make the hashtable of elements:
	  // multiply 1/eps by this amount
	  // about 3 seems to work well

	#ifdef LCL_SIZE
	#define LCL_SPACE (LCL_HASHMULT*LCL_SIZE)
	#endif


	LCLweight_t n;
	int hasha, hashb, hashsize;
	int size;
	LCLCounter *root;
#ifdef LCL_SIZE
		LCLCounter counters[LCL_SIZE + 1]; // index from 1
		LCLCounter *hashtable[LCL_SPACE]; // array of pointers to items in 'counters'
		// 24 + LCL_SIZE*(32 + LCL_HASHMULT*4) + 8
				  // = 24 + 102*(32+12)*4 = 4504
				  // call it 4520 for luck...
#else
	LCLCounter *counters;
	LCLCounter ** hashtable; // array of pointers to items in 'counters'
#endif

	void LCL_RebuildHash();
	void Heapify(int ptr);
	LCLCounter * LCL_FindItem(LCLitem_t item);
	static int LCL_cmp(const void * a, const void * b);
	void LCL_Output();
	void LCL_CheckHash(int item, int hash);
	void LCL_ShowHash();
	void LCL_ShowHeap();

public:

	 SpaceSavingHeap(float fPhi);
	 ~SpaceSavingHeap();
	 void LCL_Update(LCLitem_t, LCLweight_t);
	 int LCL_Size();
	 int LCL_PointEst(LCLitem_t);
	 int LCL_PointErr(LCLitem_t);
	 std::map<uint32_t, uint32_t> LCL_Output(int);

};

#endif
