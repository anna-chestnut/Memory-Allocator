#include "HeapManager.h"
#include <Windows.h>

#include <assert.h>
#include <algorithm>
#include <vector>


	HeapManager::HeapManager(HeapManager* i_pManager, size_t i_Bytes) {

		MemoryBlock* p = reinterpret_cast<MemoryBlock*>(i_pManager);

		pFreeBlock = new (p)MemoryBlock();
		pFreeBlock->BlockSize = i_Bytes - sizeof(MemoryBlock);
		//printf("initial block size %i\n", pFreeBlock->BlockSize);

		char* c = reinterpret_cast<char*>(pFreeBlock);
		pFreeBlock->pBaseAddress = c + sizeof(MemoryBlock);
		pFreeBlock->pNextBlock = nullptr;

		outstandingAllocation = nullptr;

		startAddress = pFreeBlock;
		char* ch = reinterpret_cast<char*>(startAddress);
		endAddress = i_pManager + i_Bytes;
	}

	HeapManager::~HeapManager() {

	}

	MemoryBlock* HeapManager::GetFreeMemoryBlock() {
		return nullptr;
	}

	bool  HeapManager::IsPowerOfTwo(size_t n) {
		return n > 0 ? !(n & n - 1) : false;
	}

	inline size_t HeapManager::AlignUp(size_t i_value, size_t i_align)
	{
		assert(i_align);
		assert(IsPowerOfTwo(i_align));
		return (i_value + (i_align - 1)) & ~(i_align - 1);
	}

	void* HeapManager::alloc(size_t i_sizes) {

		MemoryBlock* pBlock = pFreeBlock;

		size_t sizeAfterAlign;
		//find free block
		while (pBlock) {

			sizeAfterAlign = AlignUp(i_sizes + sizeof(MemoryBlock), 4);

			if (pBlock->BlockSize >= sizeAfterAlign) {
				break;
			}
			pBlock = pBlock->pNextBlock;
		}
		//didn't find a block big enough
		if (!pBlock) return nullptr;
		assert(pBlock);

		char* p = reinterpret_cast<char*>(pBlock);
		char* add = p + sizeAfterAlign;
		MemoryBlock* m = reinterpret_cast<MemoryBlock*>(add);

		MemoryBlock* pNewFreeBlock = new (m)MemoryBlock();

		pNewFreeBlock->pBaseAddress = add + sizeof(MemoryBlock);
		pNewFreeBlock->pNextBlock = pBlock->pNextBlock;
		pNewFreeBlock->BlockSize = pBlock->BlockSize - (sizeAfterAlign);

		if (pBlock->pNextBlock) {
			pBlock->pNextBlock->pPreviousBlock = pNewFreeBlock;
		}

		if (pBlock->pPreviousBlock) {
			pNewFreeBlock->pPreviousBlock = pBlock->pPreviousBlock;
			pNewFreeBlock->pPreviousBlock->pNextBlock = pNewFreeBlock;
		}
		else
		{
			pFreeBlock = pNewFreeBlock;
		}


		pBlock->BlockSize = sizeAfterAlign - sizeof(MemoryBlock);
		pBlock->pNextBlock = nullptr;
		pBlock->pPreviousBlock = nullptr;

		//printf("[Alloc Block] base address: 0x%x, size: 0x%x\n", pBlock->pBaseAddress, pBlock->BlockSize);
		//printf("     new free base address: 0x%x, size: 0x%x\n", pNewFreeBlock->pBaseAddress, pNewFreeBlock->BlockSize);

		if (outstandingAllocation == nullptr) {
			outstandingAllocation = pBlock;
		}
		else {

			pBlock->pNextBlock = outstandingAllocation;
			outstandingAllocation->pPreviousBlock = pBlock;
			outstandingAllocation = pBlock;
		}

		return pBlock->pBaseAddress;

	}

	bool HeapManager::Contains(void* i_ptr) {

		char* i_ch = reinterpret_cast<char*>(i_ptr);
		char* start = reinterpret_cast<char*>(startAddress);
		char* end = reinterpret_cast<char*>(endAddress);

		if (i_ch >= start && i_ch <= end) {
			return true;
		}
		else
			return false;
	}

	bool HeapManager::IsAllocated(void* i_ptr) {

		char* c = reinterpret_cast<char*>(i_ptr);

		//printf("check allocate address: 0x%x\n", c);
		MemoryBlock* allocatedblock = outstandingAllocation;

		while (allocatedblock) {

			char* base = reinterpret_cast<char*>(allocatedblock->pBaseAddress);
			if (base - sizeof(MemoryBlock) <= c && c <= base + allocatedblock->BlockSize) {
				return true;
			}

			allocatedblock = allocatedblock->pNextBlock;
		}

		return false;
	}

	void* HeapManager::free(void* i_ptr) {

		printf("Heap Manager free address: 0x%x", i_ptr);

		if (!HeapManager::Contains(i_ptr)) {
			printf("Free failed because it is not in HeapManager range\n");
			return nullptr;
		}

		MemoryBlock* pBlock = RemoveOutstandingAllocation(i_ptr);
		assert(pBlock);

		printf(" - size: %i\n", pBlock->BlockSize);

		if (pFreeBlock == nullptr) {
			pFreeBlock = pBlock;
		}
		else if (pBlock < pFreeBlock) {
			pBlock->pNextBlock = pFreeBlock;
			pFreeBlock->pPreviousBlock = pBlock;
			pFreeBlock = pBlock;
		}
		else {

			MemoryBlock* pf = pFreeBlock;

			while (pf->pNextBlock != nullptr && pf->pNextBlock < pBlock) {
				pf = pf->pNextBlock;
			}

			pBlock->pNextBlock = pf->pNextBlock;
			pf->pNextBlock = pBlock;
			if (pBlock->pNextBlock != nullptr)
				pBlock->pNextBlock->pPreviousBlock = pBlock;
			pBlock->pPreviousBlock = pf;

		}

		return pBlock;

	}

	MemoryBlock* HeapManager::RemoveOutstandingAllocation(void* i_ptr) {

		int a = 3;

		MemoryBlock* pBlock = reinterpret_cast<MemoryBlock*>(i_ptr);
		pBlock--;
		if (pBlock == outstandingAllocation) {
			outstandingAllocation = pBlock->pNextBlock;
		}
		else {
			MemoryBlock* head = outstandingAllocation;
			while (head->pBaseAddress != i_ptr) {
				head = head->pNextBlock;
			}
			head->pPreviousBlock->pNextBlock = head->pNextBlock;
			if (head->pNextBlock != nullptr) {
				head->pNextBlock->pPreviousBlock = head->pPreviousBlock;
			}
			printf("0000000000000000000000000\n");
			/*
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			int a = 3;


			pBlock->pPreviousBlock->pNextBlock = pBlock->pNextBlock;
			if (pBlock->pNextBlock != nullptr) {
				pBlock->pNextBlock->pPreviousBlock = pBlock->pPreviousBlock;
			}*/
		}
		pBlock->pPreviousBlock = nullptr;
		pBlock->pNextBlock = nullptr;

		return pBlock;
	}

	bool HeapManager::IsFreeBlock(void* i_ptr) {

		MemoryBlock* c = reinterpret_cast<MemoryBlock*>(i_ptr);

		MemoryBlock* freeblock = pFreeBlock;

		while (freeblock) {

			if (freeblock == c) {
				return true;
			}

			freeblock = freeblock->pNextBlock;

		}

		return false;
	}

	void HeapManager::Collect() {

		printf("Collect!\n");
		MemoryBlock* freeBlock = pFreeBlock;

		while (freeBlock) {

			MemoryBlock* pBlock1 = reinterpret_cast<MemoryBlock*>(freeBlock);
			MemoryBlock* pBlock2 = pBlock1->pNextBlock;

			//printf("-------------------\n");
			//printf("\nb1 address: 0x%x\nb2 address: 0x%x\n", pBlock1, pBlock2);

			MemoryBlock* add = pBlock1 + 1;
			char* tmp = reinterpret_cast<char*>(add);
			tmp += pBlock1->BlockSize;
			MemoryBlock* c = reinterpret_cast<MemoryBlock*>(tmp);

			//printf("pBlock1->BlockSize: 0x%x\nadd: 0x%x\n", pBlock1->BlockSize, tmp);

			if (c == pBlock2) {

				printf("equal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
				pBlock1->BlockSize += sizeof(MemoryBlock) + pBlock2->BlockSize;
				pBlock1->pNextBlock = pBlock2->pNextBlock;
				if (pBlock2->pNextBlock)
					pBlock2->pNextBlock->pPreviousBlock = pBlock1;

			}
			else
			{
				freeBlock = freeBlock->pNextBlock;

			}


		}


	}
