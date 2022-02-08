#include "BitArray.h"

#include <iostream>
#include <intrin.h>
using namespace std;


BitArray::BitArray(size_t i_numBits, bool i_bInitToZero) {

	m_numBytes = i_numBits;
	m_numOfBitArray = i_numBits / bitsPerElement;
	if (i_numBits % bitsPerElement != 0) {
		m_numOfBitArray++;
	}
	m_pBits = new t_BitData[m_numOfBitArray];
	memset(m_pBits, i_bInitToZero ? 0 : 1, m_numOfBitArray);
	printf("INITIAL: bits per element %i, m_numOfBitArray %i, m_numBytes %i\n", bitsPerElement, m_numOfBitArray, m_numBytes);
}

/*
BitArray* BitArray::Create(size_t i_numBits, bool i_startClear = true, HeapAllocator* i_pAllocator) {

}*/

BitArray::~BitArray() {
	delete[] m_pBits;
}

void BitArray::ClearAll(void) {

	printf("Clear all: \n");
	for (int i = 0; i < m_numOfBitArray; i++) {
		m_pBits[i] = m_pBits[i] & 0;
		//BitArray::printBits(m_pBits[i]);
	}
}

void BitArray::SetAll(void) {

	printf("Set all: \n");
	for (int i = 0; i < m_numOfBitArray; i++) {
		m_pBits[i] = ~(m_pBits[i] & 0);
		//BitArray::printBits(m_pBits[i]);
	}
}

bool BitArray::AreAllBitsClear(void) const {

	unsigned long bitIndex;
	unsigned char isNonzero;

	for (int i = 0; i < m_numOfBitArray; i++) {

		isNonzero = _BitScanForward(&bitIndex, m_pBits[i]);

		if (isNonzero)
		{
			return false;
		}
		else
		{
			continue;
		}
	}

	return true;
}
bool BitArray::AreAllBitsSet(void) const {

	unsigned long bitIndex;
	unsigned char isNonzero;

	for (int i = 0; i < m_numOfBitArray; i++) {

		isNonzero = _BitScanForward(&bitIndex, m_pBits[i]);

		if (isNonzero)
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;

}

bool BitArray::IsBitSet(size_t i_bitNumber) const {

	size_t index = i_bitNumber / bitsPerElement;
	size_t bitIndex = i_bitNumber % bitsPerElement;

	/*
	if (bitIndex == 0) {
		index--;
		bitIndex = bitsPerElement;
	}*/

	if (m_pBits[index] & (1 << (bitIndex)))//std::bitset?
		return true;
	else
		return false;
}
bool BitArray::IsBitClear(size_t i_bitNumber) const {

	size_t index = i_bitNumber / bitsPerElement;
	size_t bitIndex = i_bitNumber % bitsPerElement;

	/*
	if (bitIndex == 0) {
		index--;
		bitIndex = bitsPerElement;
	}*/

	if (m_pBits[index] & (1 << (bitIndex)))//std::bitset?
		return false;
	else
		return true;
}

void BitArray::SetBit(size_t i_bitNumber) {

	size_t index = i_bitNumber / bitsPerElement;
	size_t bitIndex = i_bitNumber % bitsPerElement;
	/*
	if (bitIndex == 0) {
		index--;
		bitIndex = bitsPerElement;
	}*/

	printf("Set!!! index: %i bitIndex: %i\n", index, bitIndex);
	m_pBits[index] = m_pBits[index] | (1 << (bitIndex));
	BitArray::printBits(m_pBits[index]);

}
void BitArray::ClearBit(size_t i_bitNumber) {

	size_t index = i_bitNumber / bitsPerElement;
	size_t bitIndex = i_bitNumber % bitsPerElement;
	/*
	if (bitIndex == 0) {
		index--;
		bitIndex = bitsPerElement;
	}*/
	printf("Clear!!! index: %i bitIndex: %i\n", index, bitIndex);
	m_pBits[index] = m_pBits[index] & (~(1 << (bitIndex)));
	//BitArray::printBits(m_pBits[index]);

}

bool BitArray::GetFirstClearBit(size_t& o_bitNumber) const {


	size_t index = 0;
	printf("GET FIRST CLEAR BIT\n");

	printf("m_pBits[index]: %i\n", m_pBits[index]);
	BitArray::printBits(m_pBits[index]);

	while ((~m_pBits[index] == t_BitData(0)) && (index < m_numOfBitArray))
	{
		BitArray::printBits(m_pBits[index]);
		printf("\n");
		index++;
	}

	if (index == m_numOfBitArray) {
		printf("Out of BitArray range");
		return false;
	}

	unsigned long bitIndex;
	unsigned char isNonzero;

	t_BitData notBits = ~m_pBits[index];

	printf("find first set bit in (to find first clear) ");
	BitArray::printBits(notBits);

	isNonzero = _BitScanForward(&bitIndex, notBits);

	if (isNonzero)
	{
		printf("Find Clear Index: %i bitIndex: %lu\n", index, bitIndex);
		o_bitNumber = (index * bitsPerElement) + bitIndex;

		return true;
	}
	else
	{
		//cout << "No clear bits found." << endl;
		return false;
	}
}


bool BitArray::GetFirstSetBit(size_t& o_bitNumber) const {

	
	size_t index = 0;

	while ((m_pBits[index] == t_BitData(0)) && (index < m_numOfBitArray))
	{
		index++;
	}
	
	unsigned long bitIndex;
	unsigned char isNonzero;

	if (index == m_numOfBitArray) {
		printf("Out of BitArray range\n");
		return false;
	}
	printf("find first set bit in");
	BitArray::printBits(m_pBits[index]);
	
	isNonzero = _BitScanForward(&bitIndex, m_pBits[index]);
 
	if (isNonzero)
	{
		
		//cout << "Mask: " << m_pBits[index] << " Index: " << index << endl;
		printf("Mask: 0x%x Index: %lu\n", m_pBits[index], bitIndex);
		o_bitNumber = (index * bitsPerElement) + bitIndex;
		
		return true;
	}
	else
	{
		//cout << "No set bits found." << endl;
		printf("No set bits found.");
		return false;
	}

}

bool BitArray::operator[](size_t i_index) const {
	return true;
}

void BitArray::printBits(t_BitData i_bits) const {

	
	char s[40];
	itoa(i_bits, s, 2);
	printf(" binary %s\n", s);
}


size_t BitArray::numberOfBitArray() const {

	return m_numOfBitArray;
}