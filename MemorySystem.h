#pragma once
#include"HeapManager.h"
#include "FixedSizeAllocator.h"

extern FixedSizeAllocator* FSAs[3];
extern HeapManager* pHeapManager;

// InitializeMemorySystem - initialize your memory system including your HeapManager and some FixedSizeAllocators
bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors);

// Collect - coalesce free blocks in attempt to create larger blocks
void Collect();

// DestroyMemorySystem - destroy your memory systems
void DestroyMemorySystem();
