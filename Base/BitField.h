#pragma once

class BitField
{
public:
	//BitField(unsigned char* in_pBitField, unsigned int in_uiBufferSizeInBytes, unsigned int in_uiBitsPerElement, unsigned int in_uiBufferStartOffsetInBits);
	BitField(unsigned char* in_pBitField, unsigned int in_uiElementCount, unsigned int in_uiBitsPerElement, unsigned int in_uiBufferStartOffsetInBits);

	unsigned char GetElement(unsigned int in_uiIndex) const;
	void SetElement(unsigned int in_uiIndex, unsigned char in_uiValue);
	
private:
	unsigned char* m_pBitField = nullptr;
	unsigned int m_uiBufferSizeInBytes = 0;
	unsigned int m_uiBitsPerElement = 0;
	unsigned int m_uiElementCount = 0;
	unsigned int m_uiBufferStartOffsetInBits = 0;
};
