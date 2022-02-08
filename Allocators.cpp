#include <inttypes.h>
#include <malloc.h>

#include <stdio.h>

#include "MemorySystem.h"
//#include "FixedSizeAllocator.h"


void* findAllocator(size_t i_size) {

	printf("malloc size: %i\n", i_size);
	if (i_size <= 16) {

		void* p = FSAs[0]->alloc();
		if (p != nullptr)
			return p;
	}

	if (i_size <= 32) {

		void* p = FSAs[1]->alloc();
		if (p != nullptr)
			return p;
	}

	if (i_size <= 96)
	{
		void* p = FSAs[2]->alloc();
		if (p != nullptr)
			return p;
	}

	return pHeapManager->alloc(i_size);
}

void * __cdecl malloc(size_t i_size)
{
	//HeapManager or FixedSizeAllocators
	
	return findAllocator(i_size);

	printf("malloc %zu\n", i_size);
	return _aligned_malloc(i_size, 4);
}

void __cdecl free(void * i_ptr)
{
	// HeapManager or FixedSizeAllocators
	for (int i = 0; i < 3; i++) {
		if (FSAs[i]->free(i_ptr)) return;
	}

	pHeapManager->free(i_ptr);
	printf("free 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));

	return;

	return _aligned_free(i_ptr);
}

void * operator new(size_t i_size)
{
	//HeapManager or FixedSizeAllocators

	printf("new %zu\n", i_size);
	return findAllocator(i_size);
	return _aligned_malloc(i_size, 4);
}

void operator delete(void * i_ptr)
{
	//HeapManager or FixedSizeAllocators
	printf("delete 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));
	for (int i = 0; i < 3; i++) {
		if (FSAs[i]->free(i_ptr)) return;
	}

	pHeapManager->free(i_ptr);
	printf("free 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));

	return;
	return _aligned_free(i_ptr);
}

void * operator new[](size_t i_size)
{
	//calls to HeapManager or FixedSizeAllocators
	printf("new [] %zu\n", i_size);
	return findAllocator(i_size);
	return _aligned_malloc(i_size, 4);
}

void operator delete [](void * i_ptr)
{
	// calls to HeapManager or FixedSizeAllocators
	printf("delete [] 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));
	for (int i = 0; i < 3; i++) {
		if (FSAs[i]->free(i_ptr)) return;
	}

	pHeapManager->free(i_ptr);
	printf("free 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));

	return;
	return _aligned_free(i_ptr);
}
