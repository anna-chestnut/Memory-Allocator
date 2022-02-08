#include <Windows.h>

#include "MemorySystem.h"
//#include "BitArray.h"
#include "FixedSizeAllocator.h"

#include <assert.h>
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

bool MemorySystem_UnitTest();
void BitArrayTest();
void FSATest(void* pManager);

void Test() {

	size_t alloc[] = { 100, 200, 300, 400, 500 };
	std::vector<void*> AllocatedAddresses2;
	void* pPtr;

	for (size_t i = 0; i < 5; i++) {

		pPtr = malloc(alloc[i]);
		AllocatedAddresses2.push_back(pPtr);
	}

	free(AllocatedAddresses2[2]);
	free(AllocatedAddresses2[0]);
	free(AllocatedAddresses2[1]);
	free(AllocatedAddresses2[3]);
	free(AllocatedAddresses2[4]);

	return;
}

int main(int i_arg, char **)
{
	//BitArrayTest();
	

	const size_t 		sizeHeap = 1024 * 1024;

	// you may not need this if you don't use a descriptor pool
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void * pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	//FSATest(pHeapMemory);

	// Create your HeapManager and FixedSizeAllocators.
	InitializeMemorySystem(pHeapMemory, sizeHeap, numDescriptors);

	//Test();

	//int i = 0;
	bool success = MemorySystem_UnitTest();
	assert(success);

	// Clean up your Memory System (HeapManager and FixedSizeAllocators)
	DestroyMemorySystem();

	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// in a Debug build make sure we didn't leak any memory.
#if defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif // _DEBUG

	return 0;
}

void BitArrayTest() {

	BitArray bitArray(128, true);

	bitArray.ClearAll();
	bool success = bitArray.AreAllBitsClear();
	assert(success);

	size_t setBit = 0;
	bitArray.SetBit(32);
	if (bitArray.GetFirstSetBit(setBit)) {
		printf("Find First Set at: %i\n", setBit);
	}
	else
	{
		printf("did not find set bit");
	}
	success = bitArray.IsBitSet(32);
	assert(success);

	bitArray.SetAll();
	success = bitArray.AreAllBitsSet();
	assert(success);

	bitArray.ClearBit(90);
	if (bitArray.GetFirstClearBit(setBit)) {
		printf("Find First Clear at: %i\n", setBit);
	}
	else
	{
		printf("did not find clear bit");
	}
	success = bitArray.IsBitClear(90);
	assert(success);

}

/*
void FSATest(void* pMemory) {

	FixedSizeAllocator fsa(pMemory, 16, 100);
	void* allocMemory = fsa.alloc();
	fsa.free(allocMemory);
	printf("End FSA TEST\n");
}*/
bool MemorySystem_UnitTest()
{
	const size_t maxAllocations = 1000; // 10240
	std::vector<void *> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	size_t totalAllocated = 0;

	// reserve space in AllocatedAddresses for the maximum number of allocation attempts
	// prevents new returning null when std::vector expands the underlying array
	AllocatedAddresses.reserve(10 * 1024);

	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do
	{
		const size_t		maxTestAllocationSize = 1024;//1024

		size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

		void * pPtr = malloc(sizeAlloc);

		// if allocation failed see if garbage collecting will create a large enough block
		if (pPtr == nullptr)
		{
			Collect();

			pPtr = malloc(sizeAlloc);

			// if not we're done. go on to cleanup phase of test
			if (pPtr == nullptr)
				break;
		}

		AllocatedAddresses.push_back(pPtr);
		numAllocs++;

		totalAllocated += sizeAlloc;

		// randomly free and/or garbage collect during allocation phase
		const unsigned int freeAboutEvery = 0x07;
		const unsigned int garbageCollectAboutEvery = 0x07;

		if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0))
		{
			void * pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			free(pPtrToFree);
			numFrees++;
		}
		else if ((rand() % garbageCollectAboutEvery) == 0)
		{
			Collect();

			numCollects++;
		}

	} while (numAllocs < maxAllocations);

	// now free those blocks in a random order
	if (!AllocatedAddresses.empty())
	{
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty())
		{
			void * pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			delete[] pPtrToFree;
		}

		// do garbage collection
		Collect();
		// our heap should be one single block, all the memory it started with

		// do a large test allocation to see if garbage collection worked
		void * pPtr = malloc(totalAllocated / 2);

		if (pPtr)
		{
			free(pPtr);
		}
		else
		{
			// something failed
			return false;
		}
	}
	else
	{
		return false;
	}

	// this new [] / delete [] pair should run through your allocator
	char * pNewTest = new char[1024];
	
	delete[] pNewTest;

	// we succeeded
	return true;
}
