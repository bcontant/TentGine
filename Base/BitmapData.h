#pragma once

enum class BufferFormat
{
	A_U1,

	A_U5,

	A_U8,
	R_U8,

	X1R5G5B5_U16,
	R5G6B5_U16,
	A1R5G5B5_U16,

	BGR_U24,
	RGB_U24,

	ARGB_U32,
	ABGR_U32,
	RGBA_U32,

	R_F32,
	INVALID_FORMAT
};

class BitmapData
{
public:
	BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, BufferFormat in_eFormat);
	BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, void* in_pBuffer, BufferFormat in_eFormat, unsigned int in_uiBufferPitch = 0);
	BitmapData(const BitmapData& in_BitmapData);

	virtual ~BitmapData();
	
	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }

	BufferFormat GetFormat() const { return m_eFormat; }
	unsigned int GetBufferSize() const { return m_uiBufferSize; }
	unsigned int GetBufferPitch() const { return m_uiBufferPitch; }
	unsigned int GetBitsPerPixel() const { return m_uiBitsPerPixel; }

	const unsigned char* GetBuffer() const { return m_pBuffer; }

	const void* Get(int in_x, int in_y) const;
	void Set(int in_x, int in_y, const void* in_pValue);

	void Blit(int in_x, int in_y, const BitmapData* in_pData);

	void FlipX();
	void FlipY();

	BitmapData* ConvertTo(BufferFormat in_eFormat);

private:
	void Initialize(); 

	void UpdateBufferSize();
	void UpdateBitsPerPixel();

	BufferFormat m_eFormat = BufferFormat::INVALID_FORMAT;
	unsigned int m_uiBitsPerPixel = 0;

	unsigned int m_uiWidth = 0;
	unsigned int m_uiHeight = 0;

	unsigned char* m_pBuffer = nullptr;
	unsigned int m_uiBufferSize = 0;
	unsigned int m_uiBufferPitch = 0;
};