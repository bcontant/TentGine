#include "precompiled.h"

#include "BitField.h"

BitField::BitField(unsigned char* in_pBitField, unsigned int in_uiElementCount, unsigned int in_uiBitsPerElement, unsigned int in_uiBufferStartOffsetInBits)
	:m_pBitField(in_pBitField)
	,m_uiBitsPerElement(in_uiBitsPerElement)
	,m_uiElementCount(in_uiElementCount)
	,m_uiBufferStartOffsetInBits(in_uiBufferStartOffsetInBits)
{
	AssertMsg(m_uiBitsPerElement <= 8, L("m_uiBitsPerElement needs to be smaller than 8."));

	m_uiBufferSizeInBytes = div_up(m_uiElementCount * m_uiBitsPerElement, 8);
}

unsigned char BitField::GetElement(unsigned int in_uiIndex) const
{
	unsigned int bytesIndex = (in_uiIndex * m_uiBitsPerElement) / 8;
	int bitIndex = m_uiBufferStartOffsetInBits + ((in_uiIndex * m_uiBitsPerElement) % 8);

	//We need a 16 bit value in case a >1bpp value spans accross 2 bytes.
	unsigned short v = *(unsigned short*)(&m_pBitField[bytesIndex]);
	REVERSE_ENDIANNESS(v);

	//What we have so far:
	// v is a 16 bit value.     Ex:                                    01010101 01010101
	// bitIndex points to the start of value we're looking for         ^:0    ^:7

	//Position on the start of our value
	v = v >> ((16 - bitIndex) - m_uiBitsPerElement);

	//Keep only the bits we're interested in
	v = v & (0xFF >> (8 - m_uiBitsPerElement));

	AssertMsg(v < (1 << m_uiBitsPerElement), L("Something wrong happened."));
	
	return static_cast<unsigned char>(v);
}

void BitField::SetElement(unsigned int in_uiIndex, unsigned char in_ucValue)
{
	unsigned int bytesIndex = (in_uiIndex * m_uiBitsPerElement) / 8;
	int bitIndex = m_uiBufferStartOffsetInBits + ((in_uiIndex * m_uiBitsPerElement) % 8);

	//We need a 16 bit value in case a >1bpp value spans accross 2 bytes.
	unsigned short* v = (unsigned short*)(&m_pBitField[bytesIndex]);

	unsigned short value = in_ucValue;
	unsigned short mask = 0xFF >> (8 - m_uiBitsPerElement);

	AssertMsg(in_ucValue <= mask, L("in_ucValue is too large for this bitfield (%d, max = %d)"), in_ucValue, mask);

	//Move the new value and the mask to the proper position within the 16 bit value
	value = value << ((16 - bitIndex) - m_uiBitsPerElement);
	mask = mask << ((16 - bitIndex) - m_uiBitsPerElement);

	REVERSE_ENDIANNESS(value);
	REVERSE_ENDIANNESS(mask);

	//Set the region of our pixel value to 0
	*v = *v & ~mask;

	//Set the region of our pixel value to the new pixel value
	*v = *v | value;
}