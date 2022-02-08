#pragma once


	struct MemoryBlock
	{
		void* pBaseAddress;
		size_t BlockSize;
		struct MemoryBlock* pNextBlock;
		struct MemoryBlock* pPreviousBlock;
	};

	class HeapManager {

	public:
		HeapManager(HeapManager*, size_t);
		~HeapManager();
		//HeapManager* Create(char* , size_t, unsigned int);
		void* alloc(size_t);
		void* free(void*);
		MemoryBlock* GetFreeMemoryBlock();
		bool Contains(void*);
		bool IsAllocated(void*);
		bool IsFreeBlock(void*);
		void Collect();
		bool  IsPowerOfTwo(size_t);
		inline size_t AlignUp(size_t, size_t);
		MemoryBlock* RemoveOutstandingAllocation(void*);
	private:
		struct MemoryBlock* pFreeBlock;
		struct MemoryBlock* outstandingAllocation;
		void* startAddress;
		void* endAddress;

	};
