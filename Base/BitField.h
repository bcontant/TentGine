#pragma once

//--------------------------------------------------------------------------------
class BitField
{
public:
	BitField(u8* in_pBitField, u32 in_uiElementCount, u32 in_uiBitsPerElement, u32 in_uiBufferStartOffsetInBits);

	u8 GetElement(u32 in_uiIndex) const;
	void SetElement(u32 in_uiIndex, u8 in_uiValue);
	
private:
	u8* m_pBitField = nullptr;
	u32 m_uiBufferSizeInBytes = 0;
	u32 m_uiBitsPerElement = 0;
	u32 m_uiElementCount = 0;
	u32 m_uiBufferStartOffsetInBits = 0;
};
