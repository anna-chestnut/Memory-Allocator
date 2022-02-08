#pragma once
#include "BitArray.h"
#include "HeapManager.h"

class  FixedSizeAllocator
{
public:
	 FixedSizeAllocator(HeapManager* i_baseAdress, size_t i_sizeBlock, size_t i_numBlocks);
	~ FixedSizeAllocator();
	BitArray* Create(size_t i_numBits, bool i_startClear, HeapManager* i_pAllocator);
	void* alloc();
	bool free(void* i_ptr);

private:

	BitArray* m_pAvailableBlocks;
	size_t m_BlockSize;
	void* m_pAllocatorMemory;
	void* m_pAllocatorEndAddress;
};

