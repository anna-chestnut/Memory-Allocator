#pragma once
#include <stdint.h>

#if WIN32
typedef uint32_t t_BitData;
#else
typedef uint64_t t_BitData;
#endif // WIN32

class BitArray {

public:

	BitArray(size_t i_numsBits, bool i_bInitToZero);

	//static BitArray* Create(size_t i_numBits, bool i_startClear, HeapAllocator* i_pAllocator);
	~BitArray();
	
	void ClearAll(void);
	void SetAll(void);
	
	bool AreAllBitsClear(void) const;
	bool AreAllBitsSet(void) const;
	
	bool IsBitSet(size_t i_bitNumber) const;//inline
	bool IsBitClear(size_t i_bitNumber) const;//inline
	
	void SetBit(size_t i_bitNumber);
	void ClearBit(size_t i_bitNumber);
	
	bool GetFirstClearBit(size_t& o_bitNumber) const;
	bool GetFirstSetBit(size_t& o_bitNumber) const;
	
	bool operator[](size_t i_index) const;

	void printBits(t_BitData i_bits) const;

	size_t numberOfBitArray() const;

private:

	t_BitData* m_pBits;
	size_t m_numBytes;
	size_t m_numOfBitArray;
	const size_t bitsPerElement = sizeof(t_BitData) * 8;

};
