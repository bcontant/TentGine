#pragma once

class BitmapData
{
public:
	enum EBufferFormat
	{
		eBF_A_U8,
		eBF_R_U8,

		eBF_R5G5B5_U16,
		eBF_R5G6B5_U16,
		eBF_R5G5B5A1_U16,

		eBF_RGB_U24,

		eBF_ARGB_U32,
		eBF_RGBA_U32,

		eBF_R_F32,
		eBF_Invalid
	};

	BitmapData(int in_uiWidth, int in_uiHeight, EBufferFormat in_eFormat);
	BitmapData(int in_uiWidth, int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat);
	
	int GetWidth() const { return m_uiWidth; }
	int GetHeight() const { return m_uiHeight; }

	EBufferFormat GetFormat() const { return m_eFormat; }
	unsigned int GetBufferSize() const { return m_uiWidth * m_uiHeight * m_uiPixelSize; }
	unsigned int GetPixelSize() const { return m_uiPixelSize; }

	const void* GetBuffer() const { return m_pBuffer; }

	const void* Get(int in_x, int in_y) const;
	void Set(int in_x, int in_y, const void* in_pValue);

	void Blit(int in_x, int in_y, const BitmapData* in_pData);

	BitmapData* ConvertTo(EBufferFormat in_eFormat) const;

private:
	void UpdatePixelSize();

	unsigned char* m_pBuffer = nullptr;

	int m_uiWidth = 0;
	int m_uiHeight = 0;

	EBufferFormat m_eFormat = eBF_Invalid;
	unsigned int m_uiPixelSize = 0;
};