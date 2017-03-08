#pragma once

class BitmapData
{
public:
	enum EBufferFormat
	{
		eBF_A_U1,

		eBF_A_U5,

		eBF_A_U8,
		eBF_R_U8,

		eBF_X1R5G5B5_U16,
		eBF_R5G6B5_U16,
		eBF_A1R5G5B5_U16,

		eBF_BGR_U24,
		eBF_RGB_U24,

		eBF_ARGB_U32,
		eBF_ABGR_U32,
		eBF_RGBA_U32,

		eBF_R_F32,
		eBF_Invalid
	};

	BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, EBufferFormat in_eFormat);
	BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat, unsigned int in_uiBufferPitch = 0);
	BitmapData(const BitmapData& in_BitmapData);

	virtual ~BitmapData();
	
	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }

	EBufferFormat GetFormat() const { return m_eFormat; }
	unsigned int GetBufferSize() const { return m_uiBufferSize; }
	unsigned int GetBufferPitch() const { return m_uiBufferPitch; }
	unsigned int GetBitsPerPixel() const { return m_uiBitsPerPixel; }

	const void* GetBuffer() const { return m_pBuffer; }

	const void* Get(int in_x, int in_y) const;
	void Set(int in_x, int in_y, const void* in_pValue);

	void Blit(int in_x, int in_y, const BitmapData* in_pData);

	void FlipX();
	void FlipY();

	BitmapData* ConvertTo(EBufferFormat in_eFormat);

private:
	void Initialize(); 

	void UpdateBufferSize();
	void UpdateBitsPerPixel();

	EBufferFormat m_eFormat = eBF_Invalid;
	unsigned int m_uiBitsPerPixel = 0;

	unsigned int m_uiWidth = 0;
	unsigned int m_uiHeight = 0;

	unsigned char* m_pBuffer = nullptr;
	unsigned int m_uiBufferSize = 0;
	unsigned int m_uiBufferPitch = 0;
};