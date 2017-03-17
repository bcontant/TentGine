#include "precompiled.h"

#include "BitmapData.h"
#include "BitField.h"

#include "PNGFile.h"
#include "TGAFile.h"

//--------------------------------------------------------------------------------
BitmapData* LoadBitmapData(const Path& in_file)
{
	BitmapData* pImage = nullptr;
	if (in_file.GetExtension() == L(".png"))
		pImage = LoadPNG(in_file);
	else if(in_file.GetExtension() == L(".tga"))
		pImage = LoadTGA(in_file);
	else
		Assert(false);

	return pImage;	
}


//--------------------------------------------------------------------------------
bool RequiresConversion(BufferFormat in_eSrcFormat, BufferFormat in_eDstFormat);

//Function pointer type to convert one pixel format to another
using ConvertPixelFunc = void(*)(const u8*, u8*, const u32);
ConvertPixelFunc GetConvertPixelFunction(BufferFormat in_eSrcFormat, BufferFormat in_eDstFormat);

//--------------------------------------------------------------------------------
//Static values for 1 to 7 bit pixels which can't return their addresses
static u8 ucPixelValues[128] =
{ 
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
	16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};

//--------------------------------------------------------------------------------
BitmapData::BitmapData(u32 in_uiWidth, u32 in_uiHeight, BufferFormat in_eFormat)
	: m_uiWidth(in_uiWidth)
	, m_uiHeight(in_uiHeight)
	, m_eFormat(in_eFormat)
{
	Initialize();
}

//--------------------------------------------------------------------------------
BitmapData::BitmapData(u32 in_uiWidth, u32 in_uiHeight, void* in_pBuffer, BufferFormat in_eFormat, u32 in_uiBufferPitch)
	: BitmapData(in_uiWidth, in_uiHeight, in_eFormat)
{
	if (in_uiBufferPitch == 0 || in_uiBufferPitch == m_uiBufferPitch)
	{
		memcpy(m_pBuffer, in_pBuffer, GetBufferSize());
	}
	else
	{
		u8* pDst = m_pBuffer;
		const u8* pSrc = (const u8*)in_pBuffer;
		for (u32 i = 0; i < m_uiHeight; i++)
		{
			memcpy(pDst, pSrc, m_uiBufferPitch);
			pDst += m_uiBufferPitch;
			pSrc += in_uiBufferPitch;
		}
	}
}

//--------------------------------------------------------------------------------
BitmapData::BitmapData(const BitmapData& in_BitmapData)
	: BitmapData(in_BitmapData.m_uiWidth, in_BitmapData.m_uiHeight, in_BitmapData.m_pBuffer, in_BitmapData.m_eFormat, in_BitmapData.m_uiBufferPitch)
{
}

//--------------------------------------------------------------------------------
BitmapData::~BitmapData()
{
	delete[] m_pBuffer;
	m_pBuffer = nullptr;

	m_eFormat = BufferFormat::INVALID_FORMAT;
}

//--------------------------------------------------------------------------------
void BitmapData::Initialize()
{
	UpdateBitsPerPixel();
	UpdateBufferSize();

	m_pBuffer = new u8[m_uiBufferSize];
	memset(m_pBuffer, 0, m_uiBufferSize);
}

//--------------------------------------------------------------------------------
BitmapData* BitmapData::ConvertTo(BufferFormat in_eFormat)
{
	PROFILE_BLOCK;

	if (!RequiresConversion(in_eFormat, m_eFormat))
	{
		m_eFormat = in_eFormat;
		return this;
	}

	ConvertPixelFunc f = GetConvertPixelFunction(m_eFormat, in_eFormat);
	if (f == nullptr)
	{
		//Slower fallback : m_eFormat -> RGBA32 -> in_eFormat.
		//Slower, but only requires every format to support to and from RGBA32 to support conversion to anything.
		if (GetConvertPixelFunction(m_eFormat, BufferFormat::RGBA_U32) == nullptr || GetConvertPixelFunction(BufferFormat::RGBA_U32, in_eFormat) == nullptr)
		{
			AssertMsg(false, L("ConvertTo can't convert from %d to %d."), m_eFormat, in_eFormat);
			return this;
		}

		ConvertTo(BufferFormat::RGBA_U32);
		ConvertTo(in_eFormat);
		return this;
	}

	u8* pSrcOrigin = (u8*)m_pBuffer;
	u8* pSrc = pSrcOrigin;
	u32 uiSrcPitch = m_uiBufferPitch;

	m_eFormat = in_eFormat;
	Initialize();

	u8* pDst = (u8*)m_pBuffer;
	u32 uiDstPitch = m_uiBufferPitch;

	u32 pixelsPerIteration = m_uiWidth;

	for (u32 y = 0; y < m_uiHeight; y++)
	{
		f(pSrc, pDst, pixelsPerIteration);
		pSrc += uiSrcPitch;
		pDst += uiDstPitch;
	}

	delete[] pSrcOrigin;
	return this;
}

//--------------------------------------------------------------------------------
const void* BitmapData::Get(s32 in_x, s32 in_y) const
{
	AssertMsg(in_x >= 0 && static_cast<u32>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<u32>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	if (m_uiBitsPerPixel < 8)
	{
		s32 offset = in_y * m_uiBufferPitch;

		BitField bf(&m_pBuffer[offset], m_uiWidth, m_uiBitsPerPixel, 0);
		u8 v = bf.GetElement(in_x);

		AssertMsg(v <= 0xFF >> (8 - m_uiBitsPerPixel), L("Invalid value (%d) for bpp (%d)"), v, m_uiBitsPerPixel);

		return &ucPixelValues[v];
	}
	else
	{
		return &m_pBuffer[in_y * m_uiBufferPitch + in_x * (m_uiBitsPerPixel >> 3)];
	}
}

//--------------------------------------------------------------------------------
void BitmapData::Set(s32 in_x, s32 in_y, const void* in_pValue)
{
	AssertMsg(in_x >= 0 && static_cast<u32>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<u32>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	if (m_uiBitsPerPixel < 8)
	{
		s32 offset = in_y * m_uiBufferPitch;

		BitField bf(&m_pBuffer[offset], m_uiWidth, m_uiBitsPerPixel, 0);
		bf.SetElement(in_x, *(u8*)in_pValue);
	}
	else
	{
		memcpy(&m_pBuffer[in_y * m_uiBufferPitch + in_x * (m_uiBitsPerPixel >> 3)], in_pValue, (m_uiBitsPerPixel >> 3));
	}
}

//--------------------------------------------------------------------------------
void BitmapData::Blit(s32 in_x, s32 in_y, const BitmapData* in_pData)
{
	PROFILE_BLOCK;

	AssertMsg(in_x >= 0 && static_cast<u32>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<u32>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	AssertMsg(in_pData->m_eFormat == m_eFormat, L("Invalid format"));
	AssertMsg(in_x + in_pData->m_uiWidth <= m_uiWidth && in_y + in_pData->m_uiHeight <= m_uiHeight, L("Invalid size"));

	if (GetBitsPerPixel() < 8)
	{
		for (u32 y = 0; y < in_pData->m_uiHeight; y++)
		{
			for (u32 x = 0; x < in_pData->m_uiWidth; x++)
			{
				Set(x + in_x, y + in_y, in_pData->Get(x, y));
			}
		}
	}
	else
	{
		s32 bytesPerPixel = GetBitsPerPixel() / 8;

		for (u32 y = 0; y < in_pData->m_uiHeight; y++)
		{
			void* pDest = m_pBuffer + (y + in_y) * m_uiBufferPitch + in_x * bytesPerPixel;
			void* pSrc = in_pData->m_pBuffer + y * in_pData->m_uiBufferPitch;

			memcpy(pDest, pSrc, in_pData->m_uiWidth * bytesPerPixel);
		}
	}
}

//--------------------------------------------------------------------------------
void BitmapData::FlipX()
{
	//TODO
	Assert(false);
}

//--------------------------------------------------------------------------------
void BitmapData::FlipY()
{
	u8* pTmp = new u8[m_uiBufferPitch];

	u8* pTop = m_pBuffer;
	u8* pBottom = m_pBuffer + ((m_uiHeight - 1) * m_uiBufferPitch);

	for (u32 i = 0; i < m_uiHeight / 2; i++)
	{
		memcpy(pTmp, pBottom, m_uiBufferPitch);
		memcpy(pBottom, pTop, m_uiBufferPitch);
		memcpy(pTop, pTmp, m_uiBufferPitch);

		pTop += m_uiBufferPitch;
		pBottom -= m_uiBufferPitch;
	}
}

//--------------------------------------------------------------------------------
void BitmapData::UpdateBufferSize()
{
	m_uiBufferPitch = div_up((m_uiWidth * m_uiBitsPerPixel), 8);
	m_uiBufferSize = m_uiHeight * m_uiBufferPitch;
}

//--------------------------------------------------------------------------------
void BitmapData::UpdateBitsPerPixel()
{
	switch (m_eFormat)
	{
	case BufferFormat::A_U1:			m_uiBitsPerPixel = 1; break;

	case BufferFormat::A_U5:			m_uiBitsPerPixel = 5; break;

	case BufferFormat::A_U8:
	case BufferFormat::R_U8:			m_uiBitsPerPixel = 8; break;

	case BufferFormat::X1R5G5B5_U16:
	case BufferFormat::R5G6B5_U16:
	case BufferFormat::A1R5G5B5_U16:	m_uiBitsPerPixel = 16; break;

	case BufferFormat::BGR_U24:
	case BufferFormat::RGB_U24:			m_uiBitsPerPixel = 24; break;

	case BufferFormat::RGBA_U32:
	case BufferFormat::ABGR_U32:
	case BufferFormat::ARGB_U32:
	case BufferFormat::R_F32:			m_uiBitsPerPixel = 32; break;

	case BufferFormat::INVALID_FORMAT:	m_uiBitsPerPixel = 0; break;
	}

	AssertMsg(m_uiBitsPerPixel != 0, L("Invalid Buffer Format (%d) led to invalid BPP (0)"), m_eFormat);
}

//--------------------------------------------------------------------------------
bool RequiresConversion(BufferFormat in_eSrcFormat, BufferFormat in_eDstFormat)
{
	if (in_eSrcFormat == in_eDstFormat)
		return false;

	if (in_eSrcFormat == BufferFormat::A_U8 && in_eDstFormat == BufferFormat::R_U8)
		return false;

	if (in_eSrcFormat == BufferFormat::R_U8 && in_eDstFormat == BufferFormat::A_U8)
		return false;

	return true;
}

// On Intel, we use little-endian.
// In little-endian, the least significant byte is at the lowest address.
// So the following integer : 0xRRGGBBAA would be represented like so if stored at memory address 0x00 :
// 0x00 : AA
// 0x01 : BB
// 0x02 : GG
// 0x03 : RR

//--------------------------------------------------------------------------------
template<s32 BPP>
void ConvertPixel_NoOp(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	AssertMsg(in_uiPixelCount > 0 && in_uiPixelCount <= 8, L("Invalid in_uiPixelCount (%d)"), in_uiPixelCount);

	memcpy(in_pDst, in_pSrc, (BPP * in_uiPixelCount) >> 3);
}

//--------------------------------------------------------------------------------
template<s32 SrcBPP, s32 DstBPP>
void ConvertPixel_8BPP(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	static const s32 SrcMax = (0xFF >> (8 - SrcBPP));
	static const s32 DstMax = (0xFF >> (8 - DstBPP));

	AssertMsg(SrcBPP > 0 && SrcBPP <= 8 && DstBPP > 0 && DstBPP <= 8, L("Invalid SrcBPP or DstBPP (%d, %d)"), SrcBPP, DstBPP);

	BitField src_bf((u8*)in_pSrc, in_uiPixelCount, SrcBPP, 0);
	BitField dst_bf((u8*)in_pDst, in_uiPixelCount, DstBPP, 0);

	for (u32 i = 0; i < in_uiPixelCount; i++)
	{
		u8 value = (DstMax * src_bf.GetElement(i)) / SrcMax;
		dst_bf.SetElement(i, value);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_A_U1
//
// Memory Layout (byte-order)
//   AAAAAAAA
//   0x00
//--------------------------------------------------------------------------------
template<>
void ConvertPixel_8BPP<1, 8>(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	BitField bf((u8*)in_pSrc, in_uiPixelCount, 1, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++)
	{
		in_pDst[i] = bf.GetElement(i) ? 255 : 0;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U1_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	BitField bf((u8*)in_pSrc, in_uiPixelCount, 1, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pDst += 4)
	{
		*((u32*)in_pDst) = bf.GetElement(i) ? 0x000000FF : 0;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_A_U5
//
// Memory Layout (byte-order)
//   RRRRRRRR
//   0x00 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------
template<>
void ConvertPixel_8BPP<5, 8>(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	BitField bf((u8*)in_pSrc, in_uiPixelCount, 5, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++)
	{
		u8 value = (255 * bf.GetElement(i)) / (0xFF >> 3);
		in_pDst[i] = value; 
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U5_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	BitField bf((u8*)in_pSrc, in_uiPixelCount, 5, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pDst += 4)
	{
		*((u32*)in_pDst) = bf.GetElement(i) * (0x000000FF / (0xFF >> 3));
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_A_U8
//
// Memory Layout (byte-order)
//   A
//   0x00
//--------------------------------------------------------------------------------
template<>
void ConvertPixel_8BPP<8, 5>(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	BitField bf((u8*)in_pDst, in_uiPixelCount, 5, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++)
	{
		u8 value = in_pSrc[i] >> 3;
		bf.SetElement(i, value);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G5B5_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		//No alpha channel so convert U8 to greyscale
		(*(u16*)in_pDst) = (1 << 15) + ((*in_pSrc >> 3) << 10) + ((*in_pSrc >> 3) << 5) + (*in_pSrc >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G6B5_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		//No alpha channel so convert U8 to greyscale
		(*(u16*)in_pDst) = ((*in_pSrc >> 3) << 11) + ((*in_pSrc >> 2) << 5) + (*in_pSrc >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G5B5A1_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		(*(u16*)in_pDst) = ((*in_pSrc >> 7) << 15);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_RGB_U24(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 3)
	{
		//No alpha channel so convert U8 to greyscale
		in_pDst[0] = *in_pSrc; //B
		in_pDst[1] = *in_pSrc; //G
		in_pDst[2] = *in_pSrc; //R
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((u32*)in_pDst) = *in_pSrc << 24;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((u32*)in_pDst) = *in_pSrc;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R_U8
//
// Memory Layout (byte-order)
//   R
//   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_R_U8_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((u32*)in_pDst) = *in_pSrc << 24;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_X1R5G5B5_U16
//
// Memory Layout (byte-order)
//   G3B5A  X1R5G2
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_X1R5G5B5_U16_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);

		(*(u32*)in_pDst) = (0xFF << 24) + (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_X1R5G5B5_U16_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);
		(*(u32*)in_pDst) = (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + 0xFF;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R5G6B5_U16
//
// Memory Layout (byte-order)
//   G3B5A  R5G3
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_R5G6B5_U16_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);
		(*(u32*)in_pDst) = (0xFF << 24) + (((((pSrc & 0xF800) >> 11) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x07E0) >> 5) * 0xFF) / 0x3F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_R5G6B5_U16_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);
		(*(u32*)in_pDst) = (((((pSrc & 0xF800) >> 11) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x07E0) >> 5) * 0xFF) / 0x3F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + 0xFF;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R5G5B5A1_U16
//
// Memory Layout (byte-order)
//  G2B5A1  R5G3
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_A1R5G5B5_U16_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);
		(*(u32*)in_pDst) = ((((pSrc & 0x8000) >> 15) * 0xFF) << 24) + (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A1R5G5B5_U16_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		u16 pSrc = (*(u16*)in_pSrc);
		(*(u32*)in_pDst) = (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + ((((pSrc & 0x8000) >> 15) * 0xFF) << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_BGR_U24
//
// Memory Layout (byte-order)
//   R    G    B
//  0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_RGB_U24(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 3)
	{
		in_pDst[0] = in_pSrc[2];
		in_pDst[1] = in_pSrc[1];
		in_pDst[2] = in_pSrc[0];
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (0xFF << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (0xFF << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_RGB_U24
//
// Memory Layout (byte-order)
//   B    G    R
//  0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_RGB_U24_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[2] << 24) + (in_pSrc[1] << 16) + (in_pSrc[0] << 8) + (0xFF << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_ARGB_U32
//
// Memory Layout (byte-order)
//  B    G    R    A
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_A_U8(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = *((u32*)in_pSrc) >> 24;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_ABGR_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[3] << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[2] << 24) + (in_pSrc[1] << 16) + (in_pSrc[0] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_ABGR_U32
//
// Memory Layout (byte-order)
//  R    G    B    A
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_ABGR_U32_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[3] << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ABGR_U32_To_RGBA_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_RGBA_U32
//
// Memory Layout (byte-order)
//  A    B    G    R
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------

void ConvertPixel_RGBA_U32_To_A_U1(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	BitField bf(in_pDst, in_uiPixelCount, 1, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4)
	{
		bf.SetElement(i, static_cast<u8>(*((u32*)in_pSrc)) ? 1 : 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A_U5(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	BitField bf(in_pDst, in_uiPixelCount, 5, 0);
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4)
	{
		bf.SetElement(i, static_cast<u8>(*((u32*)in_pSrc)) >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A_U8(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA

	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = static_cast<u8>(*((u32*)in_pSrc));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_R_U8(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA

	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = static_cast<u8>((*((u32*)in_pSrc)) >> 24);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_X1R5G5B5_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		u32 pSrc = (*(u32*)in_pSrc);
		(*(u16*)in_pDst) = static_cast<u16>((1 << 15) + (((pSrc & 0xFF000000) >> 27) << 10) + (((pSrc & 0x00FF0000) >> 19) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_R5G6B5_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		u32 pSrc = (*(u32*)in_pSrc);
		(*(u16*)in_pDst) = static_cast<u16>((((pSrc & 0xFF000000) >> 27) << 11) + (((pSrc & 0x00FF0000) >> 18) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A1R5G5B5_U16(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		u32 pSrc = (*(u32*)in_pSrc);
		(*(u16*)in_pDst) = static_cast<u16>((((pSrc & 0x000000FF) >> 7) << 15) + (((pSrc & 0xFF000000) >> 27) << 10) + (((pSrc & 0x00FF0000) >> 19) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_RGB_U24(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 3)
	{
		in_pDst[0] = in_pSrc[1]; //B
		in_pDst[1] = in_pSrc[2]; //G
		in_pDst[2] = in_pSrc[3]; //R
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_ARGB_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[3] << 16) + (in_pSrc[2] << 8) + (in_pSrc[1] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_ABGR_U32(const u8* in_pSrc, u8* in_pDst, const u32 in_uiPixelCount)
{
	for (u32 i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(u32*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
ConvertPixelFunc GetConvertPixelFunction(BufferFormat in_eSrcFormat, BufferFormat in_eDstFormat)
{
	//TODO : Full support
	switch (in_eSrcFormat)
	{
	case BufferFormat::A_U1:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return ConvertPixel_NoOp<1>;
		case BufferFormat::A_U5:			return ConvertPixel_8BPP<1, 5>; 
		case BufferFormat::A_U8:			return ConvertPixel_8BPP<1, 8>; 
		case BufferFormat::R_U8:			return ConvertPixel_8BPP<1, 8>; 
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return nullptr;
		case BufferFormat::RGBA_U32:		return ConvertPixel_A_U1_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::A_U5:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return ConvertPixel_8BPP<5, 1>;
		case BufferFormat::A_U5:			return ConvertPixel_NoOp<5>;
		case BufferFormat::A_U8:			return ConvertPixel_8BPP<5, 8>;
		case BufferFormat::R_U8:			return ConvertPixel_8BPP<5, 8>;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return nullptr;
		case BufferFormat::RGBA_U32:		return ConvertPixel_A_U5_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::A_U8:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return ConvertPixel_8BPP<8, 1>;
		case BufferFormat::A_U5:			return ConvertPixel_8BPP<8, 5>;
		case BufferFormat::A_U8:			return ConvertPixel_NoOp<8>;
		case BufferFormat::R_U8:			return ConvertPixel_NoOp<8>;
		case BufferFormat::X1R5G5B5_U16:	return ConvertPixel_A_U8_To_R5G5B5_U16;
		case BufferFormat::R5G6B5_U16:		return ConvertPixel_A_U8_To_R5G6B5_U16;
		case BufferFormat::A1R5G5B5_U16:	return ConvertPixel_A_U8_To_R5G5B5A1_U16;
		case BufferFormat::RGB_U24:			return ConvertPixel_A_U8_To_RGB_U24;
		case BufferFormat::ARGB_U32:		return ConvertPixel_A_U8_To_ARGB_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_A_U8_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::R_U8:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return ConvertPixel_8BPP<8, 1>;
		case BufferFormat::A_U5:			return ConvertPixel_8BPP<8, 5>;
		case BufferFormat::A_U8:			return ConvertPixel_NoOp<8>;
		case BufferFormat::R_U8:			return ConvertPixel_NoOp<8>;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return nullptr;
		case BufferFormat::RGBA_U32:		return ConvertPixel_R_U8_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::X1R5G5B5_U16:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return ConvertPixel_NoOp<16>;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return ConvertPixel_X1R5G5B5_U16_To_ARGB_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_X1R5G5B5_U16_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::R5G6B5_U16:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return ConvertPixel_NoOp<16>;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return ConvertPixel_R5G6B5_U16_To_ARGB_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_R5G6B5_U16_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::A1R5G5B5_U16:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return ConvertPixel_NoOp<16>;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return ConvertPixel_A1R5G5B5_U16_To_ARGB_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_A1R5G5B5_U16_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::BGR_U24:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::BGR_U24:			return ConvertPixel_NoOp<24>;
		case BufferFormat::RGB_U24:			return ConvertPixel_BGR_U24_To_RGB_U24;
		case BufferFormat::ARGB_U32:		return ConvertPixel_BGR_U24_To_ARGB_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_BGR_U24_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::RGB_U24:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::BGR_U24:			return nullptr;
		case BufferFormat::RGB_U24:			return ConvertPixel_NoOp<24>;
		case BufferFormat::ARGB_U32:		return nullptr;
		case BufferFormat::ABGR_U32:		return nullptr;
		case BufferFormat::RGBA_U32:		return ConvertPixel_RGB_U24_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::ARGB_U32:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return ConvertPixel_ARGB_U32_To_A_U8;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return ConvertPixel_NoOp<32>;
		case BufferFormat::ABGR_U32:		return ConvertPixel_ARGB_U32_To_ABGR_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_ARGB_U32_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::ABGR_U32:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return ConvertPixel_ABGR_U32_To_ARGB_U32;
		case BufferFormat::ABGR_U32:		return ConvertPixel_NoOp<32>;
		case BufferFormat::RGBA_U32:		return ConvertPixel_ABGR_U32_To_RGBA_U32;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}

	case BufferFormat::RGBA_U32:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return ConvertPixel_RGBA_U32_To_A_U1;
		case BufferFormat::A_U5:			return ConvertPixel_RGBA_U32_To_A_U5;
		case BufferFormat::A_U8:			return ConvertPixel_RGBA_U32_To_A_U8;
		case BufferFormat::R_U8:			return ConvertPixel_RGBA_U32_To_R_U8;
		case BufferFormat::X1R5G5B5_U16:	return ConvertPixel_RGBA_U32_To_X1R5G5B5_U16;
		case BufferFormat::R5G6B5_U16:		return ConvertPixel_RGBA_U32_To_R5G6B5_U16;
		case BufferFormat::A1R5G5B5_U16:	return ConvertPixel_RGBA_U32_To_A1R5G5B5_U16;
		case BufferFormat::RGB_U24:			return ConvertPixel_RGBA_U32_To_RGB_U24;
		case BufferFormat::ARGB_U32:		return ConvertPixel_RGBA_U32_To_ARGB_U32;
		case BufferFormat::ABGR_U32:		return ConvertPixel_RGBA_U32_To_ABGR_U32;
		case BufferFormat::RGBA_U32:		return ConvertPixel_NoOp<32>;
		case BufferFormat::R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BufferFormat::R_F32:
		switch (in_eDstFormat)
		{
		case BufferFormat::A_U1:			return nullptr;
		case BufferFormat::A_U5:			return nullptr;
		case BufferFormat::A_U8:			return nullptr;
		case BufferFormat::R_U8:			return nullptr;
		case BufferFormat::X1R5G5B5_U16:	return nullptr;
		case BufferFormat::R5G6B5_U16:		return nullptr;
		case BufferFormat::A1R5G5B5_U16:	return nullptr;
		case BufferFormat::RGB_U24:			return nullptr;
		case BufferFormat::ARGB_U32:		return nullptr;
		case BufferFormat::RGBA_U32:		return nullptr;
		case BufferFormat::R_F32:			return ConvertPixel_NoOp<32>;
		default:							return nullptr;
		}
	default:
		return nullptr;
	}
}

