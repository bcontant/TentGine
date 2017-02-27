#pragma once

class BitmapData
{
public:
	enum EBufferFormat
	{
		eBF_U8,
		eBF_RGB_U24,
		eBF_ARGB_U32,
		eBF_RGBA_U32,
		eBF_R_F32,
		eBF_Invalid
	};

	union PixelValue
	{
		unsigned char ucValue;
		unsigned int  uiValue = 0;
		float         fValue;

		PixelValue ConvertTo(const PixelValue& in_v, EBufferFormat T, EBufferFormat V);
	};
	
	BitmapData(int in_uiWidth, int in_uiHeight, EBufferFormat in_eFormat);
	BitmapData(int in_uiWidth, int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat);

	BitmapData* ConvertTo(EBufferFormat in_eFormat);

	int GetWidth() const { return m_uiWidth; }
	int GetHeight() const { return m_uiHeight; }

	PixelValue Get(int in_x, int in_y) const;
	void Set(int in_x, int in_y, const PixelValue in_value);

	void Blit(int in_x, int in_y, const BitmapData* in_pData);

	unsigned int GetBufferSize() const;
	unsigned int GetPixelSize() const;

	const void* GetBuffer() const { return m_pBuffer; }

private:
	unsigned char* m_pBuffer = nullptr;

	int m_uiWidth = 0;
	int m_uiHeight = 0;

	EBufferFormat m_eFormat = eBF_Invalid;
};