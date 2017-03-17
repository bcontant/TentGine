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

	//TODO : This is a useless format I think.. remove (and change 'universal conversion' to use another one)
	RGBA_U32,

	R_F32,
	INVALID_FORMAT
};

class BitmapData
{
public:
	BitmapData(u32 in_uiWidth, u32 in_uiHeight, BufferFormat in_eFormat);
	BitmapData(u32 in_uiWidth, u32 in_uiHeight, void* in_pBuffer, BufferFormat in_eFormat, u32 in_uiBufferPitch = 0);
	BitmapData(const BitmapData& in_BitmapData);

	virtual ~BitmapData();
	
	u32 GetWidth() const { return m_uiWidth; }
	u32 GetHeight() const { return m_uiHeight; }

	BufferFormat GetFormat() const { return m_eFormat; }
	u32 GetBufferSize() const { return m_uiBufferSize; }
	u32 GetBufferPitch() const { return m_uiBufferPitch; }
	u32 GetBitsPerPixel() const { return m_uiBitsPerPixel; }

	const u8* GetBuffer() const { return m_pBuffer; }

	const void* Get(s32 in_x, s32 in_y) const;
	void Set(s32 in_x, s32 in_y, const void* in_pValue);

	void Blit(s32 in_x, s32 in_y, const BitmapData* in_pData);

	void FlipX();
	void FlipY();

	BitmapData* ConvertTo(BufferFormat in_eFormat);

private:
	void Initialize(); 

	void UpdateBufferSize();
	void UpdateBitsPerPixel();

	BufferFormat m_eFormat = BufferFormat::INVALID_FORMAT;
	u32 m_uiBitsPerPixel = 0;

	u32 m_uiWidth = 0;
	u32 m_uiHeight = 0;

	u8* m_pBuffer = nullptr;
	u32 m_uiBufferSize = 0;
	u32 m_uiBufferPitch = 0;
};

BitmapData* LoadBitmapData(const Path& in_file);