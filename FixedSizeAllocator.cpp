#include "FixedSizeAllocator.h"
#include <stdio.h>
#include <cassert>
#include <new>

FixedSizeAllocator::FixedSizeAllocator(HeapManager* i_baseAdress, size_t i_sizeBlock, size_t i_numBlocks)
{
    m_pAvailableBlocks = Create(i_sizeBlock * i_numBlocks, true, i_baseAdress);

    m_pAllocatorMemory = m_pAvailableBlocks + m_pAvailableBlocks->numberOfBitArray() + 1;

    size_t* base = reinterpret_cast<size_t*>(m_pAllocatorMemory);

    m_pAllocatorEndAddress = base + i_sizeBlock * i_numBlocks;

    m_BlockSize = i_sizeBlock;
    printf("Fixed Size Allocator initial: BitArray 0x%x Allocate 0x%x\n", m_pAvailableBlocks, m_pAllocatorMemory);
}

FixedSizeAllocator::~FixedSizeAllocator()
{
    //m_pAvailableBlocks->ClearAll();
    size_t firstClear;
    if (m_pAvailableBlocks->AreAllBitsClear())
    {
        printf("Oh no! We have an outstanding allocation\n");
        
    }
}

BitArray* FixedSizeAllocator::Create(size_t i_numBits, bool i_startClear, HeapManager* i_pAllocator) {//HeapAllocator* i_pAllocator

    assert((i_pAllocator != nullptr) && (i_numBits > 0));

    BitArray* bitArray = reinterpret_cast<BitArray*>(i_pAllocator->alloc(i_numBits));
    //return bitArray;
    return new (bitArray) BitArray(i_numBits, i_startClear);
}

void* FixedSizeAllocator::alloc() {

    printf("---FixedSizeAllocator alloc---\n");
    size_t i_firstAvailable;

    if (m_pAvailableBlocks->GetFirstClearBit(i_firstAvailable))
    {
        // mark it in use because we're going to allocate it to user
        m_pAvailableBlocks->SetBit(i_firstAvailable);

        printf("FSA ALLOC set bit at %i \n", i_firstAvailable);

        size_t* p = reinterpret_cast<size_t*>(m_pAllocatorMemory);
        // calculate it’s address and return it to user
        return p + (i_firstAvailable * m_BlockSize);
    }
    else
        return nullptr;
}

bool FixedSizeAllocator::free(void* i_ptr) {
    printf("---FixedSizeAllocator free---\n");
    size_t* p = reinterpret_cast<size_t*>(i_ptr);
    size_t* base = reinterpret_cast<size_t*>(m_pAllocatorMemory);
    size_t bits = (p - base)/m_BlockSize;

    if (p<base || p>m_pAllocatorEndAddress) {
        printf("FIXED SIZE FREE out of range !\n");
        return false;
    }

    printf("Check free bit: %i\n", bits);

    if (m_pAvailableBlocks->IsBitSet(bits)) {
        m_pAvailableBlocks->ClearBit(bits);
        return true;
    }
    else {
        printf("Fixed Size Allocate failed because Bit is not set\n");
        return false;
    }
}