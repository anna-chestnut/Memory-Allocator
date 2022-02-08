#include "MemorySystem.h"
#include <new>
#include <assert.h>
#include <stdio.h>

struct FSAInitData
{
	size_t sizeBlock;
	size_t numBlocks;
};

FSAInitData FSASizes[3] = { {16, 100}, {32, 200}, {96, 100} };
HeapManager* pHeapManager;
FixedSizeAllocator* FSAs[3];


void InitializeFSA() {
	
	FSASizes[0].sizeBlock = 16;
	FSASizes[0].numBlocks = 100;
	FSASizes[1].sizeBlock = 32;
	FSASizes[1].numBlocks = 200;
	FSASizes[2].sizeBlock = 96;
	FSASizes[2].numBlocks = 400;
}

HeapManager* CreateHeapManager(void* i_pMemory, size_t i_Bytes, unsigned int numDescriptors) {

	printf("Create Heap Manager\n");
	assert((i_pMemory != nullptr) && (i_Bytes > 0));

	HeapManager* pManager = reinterpret_cast<HeapManager*>(i_pMemory);
	return new (pManager) HeapManager(++pManager, i_Bytes - sizeof(HeapManager));
}

FixedSizeAllocator* CreateFixedSizeAllocator(size_t i_sizeBlock, size_t i_numBlocks, HeapManager* pDefaultHeap) {


	printf("Create Fixed Size Allocator\n");
	size_t numsOfBitArray = (i_sizeBlock * i_numBlocks) / (sizeof(t_BitData) * 8);
	if ((i_sizeBlock * i_numBlocks) % (sizeof(t_BitData) * 8) != 0) {
		numsOfBitArray++;
	}

	void* pPtr = pDefaultHeap->alloc(i_sizeBlock * i_numBlocks);
	return new (pPtr) FixedSizeAllocator(pDefaultHeap, i_sizeBlock, i_numBlocks);
}

bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors)
{
	// create HeapManager and FixedSizeAllocators
	pHeapManager = CreateHeapManager(i_pHeapMemory, i_sizeHeapMemory, i_OptionalNumDescriptors);
	assert(pHeapManager);

	for (unsigned int i = 0; i < sizeof(FSASizes) / sizeof(FSASizes[0]); i++)
		FSAs[i] = CreateFixedSizeAllocator(FSASizes[i].sizeBlock, FSASizes[i].numBlocks, pHeapManager);

	return true;
}

void Collect()
{
	// coalesce free blocks
	// you may or may not need to do this depending on how you've implemented your HeapManager
	pHeapManager->Collect();

}

void DestroyMemorySystem()
{
	// Destroy HeapManager and FixedSizeAllocators
	pHeapManager->~HeapManager();
}

