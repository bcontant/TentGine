#include "precompiled.h"

#include "BitmapData.h"
#include "BitField.h"


//--------------------------------------------------------------------------------
bool RequiresConversion(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat);

//Function pointer type to convert one pixel format to another
using ConvertPixelFunc = void(*)(const unsigned char*, unsigned char*, const unsigned int);
ConvertPixelFunc GetConvertPixelFunction(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat);

//--------------------------------------------------------------------------------
//Static values for 1 to 7 bit pixels which can't return their addresses
static unsigned char ucPixelValues[128] = 
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
BitmapData::BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, EBufferFormat in_eFormat)
	: m_uiWidth(in_uiWidth)
	, m_uiHeight(in_uiHeight)
	, m_eFormat(in_eFormat)
{
	Initialize();
}

//--------------------------------------------------------------------------------
BitmapData::BitmapData(unsigned int in_uiWidth, unsigned int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat, unsigned int in_uiBufferPitch)
	: BitmapData(in_uiWidth, in_uiHeight, in_eFormat)
{
	if (in_uiBufferPitch == 0 || in_uiBufferPitch == m_uiBufferPitch)
	{
		memcpy(m_pBuffer, in_pBuffer, GetBufferSize());
	}
	else
	{
		unsigned char* pDst = m_pBuffer;
		const unsigned char* pSrc = (const unsigned char*)in_pBuffer;
		for (unsigned int i = 0; i < m_uiHeight; i++)
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

	m_eFormat = eBF_Invalid;
}

//--------------------------------------------------------------------------------
void BitmapData::Initialize()
{
	UpdateBitsPerPixel();
	UpdateBufferSize();

	m_pBuffer = new unsigned char[m_uiBufferSize];
	memset(m_pBuffer, 0, m_uiBufferSize);
}

//--------------------------------------------------------------------------------
BitmapData* BitmapData::ConvertTo(EBufferFormat in_eFormat)
{
	PROFILE_BLOCK;

	if (!RequiresConversion(in_eFormat, m_eFormat))
		return this;

	ConvertPixelFunc f = GetConvertPixelFunction(m_eFormat, in_eFormat);
	if (f == nullptr)
	{
		//Slower fallback : m_eFormat -> RGBA32 -> in_eFormat.
		//Slower, but only requires every format to support to and from RGBA32 to support conversion to anything.
		if (GetConvertPixelFunction(m_eFormat, eBF_RGBA_U32) == nullptr || GetConvertPixelFunction(eBF_RGBA_U32, in_eFormat) == nullptr)
		{
			AssertMsg(false, L("ConvertTo can't convert from %d to %d."), m_eFormat, in_eFormat);
			return this;
		}

		ConvertTo(eBF_RGBA_U32);
		ConvertTo(in_eFormat);
		return this;
	}

	unsigned char* pSrcOrigin = (unsigned char*)m_pBuffer;
	unsigned char* pSrc = pSrcOrigin;
	unsigned int uiSrcPitch = m_uiBufferPitch;

	m_eFormat = in_eFormat;
	Initialize();

	unsigned char* pDst = (unsigned char*)m_pBuffer;
	unsigned int uiDstPitch = m_uiBufferPitch;

	unsigned int pixelsPerIteration = m_uiWidth;

	for (unsigned int y = 0; y < m_uiHeight; y++)
	{
		f(pSrc, pDst, pixelsPerIteration);
		pSrc += uiSrcPitch;
		pDst += uiDstPitch;
	}

	delete[] pSrcOrigin;
	return this;
}

//--------------------------------------------------------------------------------
const void* BitmapData::Get(int in_x, int in_y) const
{
	AssertMsg(in_x >= 0 && static_cast<unsigned int>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<unsigned int>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	if (m_uiBitsPerPixel < 8)
	{
		int offset = in_y * m_uiBufferPitch;

		BitField bf(&m_pBuffer[offset], m_uiWidth, m_uiBitsPerPixel, 0);
		unsigned char v = bf.GetElement(in_x);

		AssertMsg(v <= 0xFF >> (8 - m_uiBitsPerPixel), L("Invalid value (%d) for bpp (%d)"), v, m_uiBitsPerPixel);

		return &ucPixelValues[v];
	}
	else
	{
		return &m_pBuffer[in_y * m_uiBufferPitch + in_x * (m_uiBitsPerPixel >> 3)];
	}
}

//--------------------------------------------------------------------------------
void BitmapData::Set(int in_x, int in_y, const void* in_pValue)
{
	AssertMsg(in_x >= 0 && static_cast<unsigned int>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<unsigned int>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	if (m_uiBitsPerPixel < 8)
	{
		int offset = in_y * m_uiBufferPitch;

		BitField bf(&m_pBuffer[offset], m_uiWidth, m_uiBitsPerPixel, 0);
		bf.SetElement(in_x, *(unsigned char*)in_pValue);
	}
	else
	{
		memcpy(&m_pBuffer[in_y * m_uiBufferPitch + in_x * (m_uiBitsPerPixel >> 3)], in_pValue, (m_uiBitsPerPixel >> 3));
	}
}

//--------------------------------------------------------------------------------
void BitmapData::Blit(int in_x, int in_y, const BitmapData* in_pData)
{
	AssertMsg(in_x >= 0 && static_cast<unsigned int>(in_x) < m_uiWidth, L("Invalid x (%d)"), in_x);
	AssertMsg(in_y >= 0 && static_cast<unsigned int>(in_y) < m_uiHeight, L("Invalid y (%d)"), in_y);

	AssertMsg(in_pData->m_eFormat == m_eFormat, L("Invalid format"));
	AssertMsg(in_x + in_pData->m_uiWidth <= m_uiWidth && in_y + in_pData->m_uiHeight <= m_uiHeight, L("Invalid size"));

	if (GetBitsPerPixel() < 8)
	{
		for (unsigned int y = 0; y < in_pData->m_uiHeight; y++)
		{
			for (unsigned int x = 0; x < in_pData->m_uiWidth; x++)
			{
				Set(x + in_x, y + in_y, in_pData->Get(x, y));
			}
		}
	}
	else
	{
		int bytesPerPixel = GetBitsPerPixel() / 8;

		for (unsigned int y = 0; y < in_pData->m_uiHeight; y++)
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
	unsigned char* pTmp = new unsigned char[m_uiBufferPitch];

	unsigned char* pTop = m_pBuffer;
	unsigned char* pBottom = m_pBuffer + ((m_uiHeight - 1) * m_uiBufferPitch);

	for (unsigned int i = 0; i < m_uiHeight / 2; i++)
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
	m_uiBufferPitch = (m_uiWidth * m_uiBitsPerPixel) / 8;
	if((m_uiWidth * m_uiBitsPerPixel) % 8)
		m_uiBufferPitch++;

	m_uiBufferSize = m_uiHeight * m_uiBufferPitch;
}

//--------------------------------------------------------------------------------
void BitmapData::UpdateBitsPerPixel()
{
	switch (m_eFormat)
	{
	case eBF_A_U1:			m_uiBitsPerPixel = 1; break;

	case eBF_A_U5:			m_uiBitsPerPixel = 5; break;

	case eBF_A_U8:
	case eBF_R_U8:			m_uiBitsPerPixel = 8; break;

	case eBF_X1R5G5B5_U16:
	case eBF_R5G6B5_U16:
	case eBF_A1R5G5B5_U16:	m_uiBitsPerPixel = 16; break;

	case eBF_BGR_U24:
	case eBF_RGB_U24:		m_uiBitsPerPixel = 24; break;

	case eBF_RGBA_U32:
	case eBF_ABGR_U32:
	case eBF_ARGB_U32:
	case eBF_R_F32:			m_uiBitsPerPixel = 32; break;

	default:				m_uiBitsPerPixel = 0; break;
	}
}

//--------------------------------------------------------------------------------
bool RequiresConversion(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat)
{
	if (in_eSrcFormat == in_eDstFormat)
		return false;

	if (in_eSrcFormat == BitmapData::eBF_A_U8 && in_eDstFormat == BitmapData::eBF_R_U8)
		return false;

	if (in_eSrcFormat == BitmapData::eBF_R_U8 && in_eDstFormat == BitmapData::eBF_A_U8)
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
template<int BPP>
void ConvertPixel_NoOp(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	AssertMsg(in_uiPixelCount > 0 && in_uiPixelCount <= 8, L("Invalid in_uiPixelCount (%d)"), in_uiPixelCount);

	memcpy(in_pDst, in_pSrc, (BPP * in_uiPixelCount) >> 3);
}

//--------------------------------------------------------------------------------
template<int SrcBPP, int DstBPP>
void ConvertPixel_8BPP(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	static const int SrcMax = (0xFF >> (8 - SrcBPP));
	static const int DstMax = (0xFF >> (8 - DstBPP));

	AssertMsg(SrcBPP > 0 && SrcBPP <= 8 && DstBPP > 0 && DstBPP <= 8, L("Invalid SrcBPP or DstBPP (%d, %d)"), SrcBPP, DstBPP);

	BitField src_bf((unsigned char*)in_pSrc, in_uiPixelCount, SrcBPP, 0);
	BitField dst_bf((unsigned char*)in_pDst, in_uiPixelCount, DstBPP, 0);

	for (unsigned int i = 0; i < in_uiPixelCount; i++)
	{
		unsigned char value = (DstMax * src_bf.GetElement(i)) / SrcMax;
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
void ConvertPixel_8BPP<1, 8>(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	BitField bf((unsigned char*)in_pSrc, in_uiPixelCount, 1, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++)
	{
		in_pDst[i] = bf.GetElement(i) ? 255 : 0;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U1_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	BitField bf((unsigned char*)in_pSrc, in_uiPixelCount, 1, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pDst += 4)
	{
		*((unsigned int*)in_pDst) = bf.GetElement(i) ? 0x000000FF : 0;
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
void ConvertPixel_8BPP<5, 8>(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	BitField bf((unsigned char*)in_pSrc, in_uiPixelCount, 5, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++)
	{
		unsigned char value = (255 * bf.GetElement(i)) / (0xFF >> 3);
		in_pDst[i] = value; 
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U5_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	BitField bf((unsigned char*)in_pSrc, in_uiPixelCount, 5, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pDst += 4)
	{
		*((unsigned int*)in_pDst) = bf.GetElement(i) * (0x000000FF / (0xFF >> 3));
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
void ConvertPixel_8BPP<8, 5>(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	BitField bf((unsigned char*)in_pDst, in_uiPixelCount, 5, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++)
	{
		unsigned char value = in_pSrc[i] >> 3;
		bf.SetElement(i, value);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G5B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		//No alpha channel so convert U8 to greyscale
		(*(unsigned short*)in_pDst) = (1 << 15) + ((*in_pSrc >> 3) << 10) + ((*in_pSrc >> 3) << 5) + (*in_pSrc >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G6B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		//No alpha channel so convert U8 to greyscale
		(*(unsigned short*)in_pDst) = ((*in_pSrc >> 3) << 11) + ((*in_pSrc >> 2) << 5) + (*in_pSrc >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_R5G5B5A1_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 2)
	{
		(*(unsigned short*)in_pDst) = ((*in_pSrc >> 7) << 15);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_RGB_U24(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 3)
	{
		//No alpha channel so convert U8 to greyscale
		in_pDst[0] = *in_pSrc; //B
		in_pDst[1] = *in_pSrc; //G
		in_pDst[2] = *in_pSrc; //R
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((unsigned int*)in_pDst) = *in_pSrc << 24;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A_U8_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((unsigned int*)in_pDst) = *in_pSrc;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R_U8
//
// Memory Layout (byte-order)
//   R
//   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_R_U8_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc++, in_pDst += 4)
	{
		*((unsigned int*)in_pDst) = *in_pSrc << 24;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_X1R5G5B5_U16
//
// Memory Layout (byte-order)
//   G3B5A  X1R5G2
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_X1R5G5B5_U16_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);

		/*
		int r = ((pSrc & 0x7C00) >> 10);
		int g = ((pSrc & 0x03E0) >> 5);
		int b = ((pSrc & 0x001F) >> 0);

		r = ((r * 0xFF) / 0x1F);
		g = ((g * 0xFF) / 0x1F);
		b = ((b * 0xFF) / 0x1F);

		(*(unsigned int*)in_pDst) = (0xFF << 24) + (r << 16) + (g << 8) + (b << 0);
		*/

		(*(unsigned int*)in_pDst) = (0xFF << 24) + (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_X1R5G5B5_U16_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);
		(*(unsigned int*)in_pDst) = (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + 0xFF;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R5G6B5_U16
//
// Memory Layout (byte-order)
//   G3B5A  R5G3
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_R5G6B5_U16_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	/*
	unsigned short pSrc = (*(unsigned short*)in_pSrc);

	int r = ((pSrc & 0xF800) >> 11);
	int g = ((pSrc & 0x07E0) >> 5);
	int b = ((pSrc & 0x001F) >> 0);

	r = ((r * 0xFF) / 0x1F);
	g = ((g * 0xFF) / 0x3F);
	b = ((b * 0xFF) / 0x1F);

	(*(unsigned int*)in_pDst) = (0xFF << 24) + (r << 16) + (g << 8) + (b << 0);
	*/

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);
		(*(unsigned int*)in_pDst) = (0xFF << 24) + (((((pSrc & 0xF800) >> 11) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x07E0) >> 5) * 0xFF) / 0x3F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_R5G6B5_U16_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);
		(*(unsigned int*)in_pDst) = (((((pSrc & 0xF800) >> 11) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x07E0) >> 5) * 0xFF) / 0x3F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + 0xFF;
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_R5G5B5A1_U16
//
// Memory Layout (byte-order)
//  G2B5A1  R5G3
//   0x00   0x00
//--------------------------------------------------------------------------------
void ConvertPixel_A1R5G5B5_U16_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	/*
	int a = ((pSrc & 0x8000) >> 15);
	int r = ((pSrc & 0x7C00) >> 10);
	int g = ((pSrc & 0x03E0) >> 5);
	int b = ((pSrc & 0x001F) >> 0);

	a = a * 0xFF);
	r = ((r * 0xFF) / 0x1F);
	g = ((g * 0xFF) / 0x1F);
	b = ((b * 0xFF) / 0x1F);

	(*(unsigned int*)in_pDst) = (0xFF << 24) + (r << 16) + (g << 8) + (b << 0);
	*/

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);
		(*(unsigned int*)in_pDst) = ((((pSrc & 0x8000) >> 15) * 0xFF) << 24) + (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 8) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_A1R5G5B5_U16_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 2, in_pDst += 4)
	{
		unsigned short pSrc = (*(unsigned short*)in_pSrc);
		(*(unsigned int*)in_pDst) = (((((pSrc & 0x7C00) >> 10) * 0xFF) / 0x1F) << 24) + (((((pSrc & 0x03E0) >> 5) * 0xFF) / 0x1F) << 16) + (((((pSrc & 0x001F) >> 0) * 0xFF) / 0x1F) << 8) + ((((pSrc & 0x8000) >> 15) * 0xFF) << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_BGR_U24
//
// Memory Layout (byte-order)
//   R    G    B
//  0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_RGB_U24(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 3)
	{
		in_pDst[0] = in_pSrc[2];
		in_pDst[1] = in_pSrc[1];
		in_pDst[2] = in_pSrc[0];
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (0xFF << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_BGR_U24_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (0xFF << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_RGB_U24
//
// Memory Layout (byte-order)
//   B    G    R
//  0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_RGB_U24_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 3, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[2] << 24) + (in_pSrc[1] << 16) + (in_pSrc[0] << 8) + (0xFF << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_ARGB_U32
//
// Memory Layout (byte-order)
//  B    G    R    A
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_A_U8(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = *((unsigned int*)in_pSrc) >> 24;
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_ABGR_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[3] << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ARGB_U32_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[2] << 24) + (in_pSrc[1] << 16) + (in_pSrc[0] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_ABGR_U32
//
// Memory Layout (byte-order)
//  R    G    B    A
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------
void ConvertPixel_ABGR_U32_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[3] << 24) + (in_pSrc[0] << 16) + (in_pSrc[1] << 8) + (in_pSrc[2] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_ABGR_U32_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
//BitmapData::eBF_RGBA_U32
//
// Memory Layout (byte-order)
//  A    B    G    R
// 0x00 0x00 0x00 0x00
//--------------------------------------------------------------------------------

void ConvertPixel_RGBA_U32_To_A_U1(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	BitField bf((unsigned char*)in_pDst, in_uiPixelCount, 1, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4)
	{
		bf.SetElement(i, static_cast<unsigned char>(*((unsigned int*)in_pSrc)) ? 1 : 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A_U5(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA
	BitField bf((unsigned char*)in_pDst, in_uiPixelCount, 5, 0);
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4)
	{
		bf.SetElement(i, static_cast<unsigned char>(*((unsigned int*)in_pSrc)) >> 3);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A_U8(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = static_cast<unsigned char>(*((unsigned int*)in_pSrc));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_R_U8(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF RGB DATA

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst++)
	{
		*in_pDst = static_cast<unsigned char>((*((unsigned int*)in_pSrc)) >> 24);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_X1R5G5B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		unsigned int pSrc = (*(unsigned int*)in_pSrc);
		(*(unsigned short*)in_pDst) = static_cast<unsigned short>((1 << 15) + (((pSrc & 0xFF000000) >> 27) << 10) + (((pSrc & 0x00FF0000) >> 19) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_R5G6B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		unsigned int pSrc = (*(unsigned int*)in_pSrc);
		(*(unsigned short*)in_pDst) = static_cast<unsigned short>((((pSrc & 0xFF000000) >> 27) << 11) + (((pSrc & 0x00FF0000) >> 18) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_A1R5G5B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 2)
	{
		unsigned int pSrc = (*(unsigned int*)in_pSrc);
		(*(unsigned short*)in_pDst) = static_cast<unsigned short>((((pSrc & 0x000000FF) >> 7) << 15) + (((pSrc & 0xFF000000) >> 27) << 10) + (((pSrc & 0x00FF0000) >> 19) << 5) + (((pSrc & 0x0000FF00) >> 11) << 0));
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_RGB_U24(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	//WARNING : LOSS OF ALPHA CHANNEL

	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 3)
	{
		in_pDst[0] = in_pSrc[1]; //B
		in_pDst[1] = in_pSrc[2]; //G
		in_pDst[2] = in_pSrc[3]; //R
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[3] << 16) + (in_pSrc[2] << 8) + (in_pSrc[1] << 0);
	}
}

//--------------------------------------------------------------------------------
void ConvertPixel_RGBA_U32_To_ABGR_U32(const unsigned char* in_pSrc, unsigned char* in_pDst, const unsigned int in_uiPixelCount)
{
	for (unsigned int i = 0; i < in_uiPixelCount; i++, in_pSrc += 4, in_pDst += 4)
	{
		(*(unsigned int*)in_pDst) = (in_pSrc[0] << 24) + (in_pSrc[1] << 16) + (in_pSrc[2] << 8) + (in_pSrc[3] << 0);
	}
}

//--------------------------------------------------------------------------------
ConvertPixelFunc GetConvertPixelFunction(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat)
{
	//TODO : Full support
	switch (in_eSrcFormat)
	{
	case BitmapData::eBF_A_U1:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return ConvertPixel_NoOp<1>;
		case BitmapData::eBF_A_U5:			return ConvertPixel_8BPP<1, 5>; 
		case BitmapData::eBF_A_U8:			return ConvertPixel_8BPP<1, 8>; 
		case BitmapData::eBF_R_U8:			return ConvertPixel_8BPP<1, 8>; 
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_A_U1_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_A_U5:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return ConvertPixel_8BPP<5, 1>;
		case BitmapData::eBF_A_U5:			return ConvertPixel_NoOp<5>;
		case BitmapData::eBF_A_U8:			return ConvertPixel_8BPP<5, 8>;
		case BitmapData::eBF_R_U8:			return ConvertPixel_8BPP<5, 8>;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_A_U5_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_A_U8:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return ConvertPixel_8BPP<8, 1>;
		case BitmapData::eBF_A_U5:			return ConvertPixel_8BPP<8, 5>;
		case BitmapData::eBF_A_U8:			return ConvertPixel_NoOp<8>;
		case BitmapData::eBF_R_U8:			return ConvertPixel_NoOp<8>;
		case BitmapData::eBF_X1R5G5B5_U16:	return ConvertPixel_A_U8_To_R5G5B5_U16;
		case BitmapData::eBF_R5G6B5_U16:	return ConvertPixel_A_U8_To_R5G6B5_U16;
		case BitmapData::eBF_A1R5G5B5_U16:	return ConvertPixel_A_U8_To_R5G5B5A1_U16;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_A_U8_To_RGB_U24;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_A_U8_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_A_U8_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R_U8:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return ConvertPixel_8BPP<8, 1>;
		case BitmapData::eBF_A_U5:			return ConvertPixel_8BPP<8, 5>;
		case BitmapData::eBF_A_U8:			return ConvertPixel_NoOp<8>;
		case BitmapData::eBF_R_U8:			return ConvertPixel_NoOp<8>;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_R_U8_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_X1R5G5B5_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return ConvertPixel_NoOp<16>;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_X1R5G5B5_U16_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_X1R5G5B5_U16_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R5G6B5_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return ConvertPixel_NoOp<16>;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_R5G6B5_U16_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_R5G6B5_U16_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_A1R5G5B5_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return ConvertPixel_NoOp<16>;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_A1R5G5B5_U16_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_A1R5G5B5_U16_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_BGR_U24:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_BGR_U24:		return ConvertPixel_NoOp<24>;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_BGR_U24_To_RGB_U24;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_BGR_U24_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_BGR_U24_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_RGB_U24:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_BGR_U24:		return nullptr;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_NoOp<24>;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_ABGR_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_RGB_U24_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_ARGB_U32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return ConvertPixel_ARGB_U32_To_A_U8;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_NoOp<32>;
		case BitmapData::eBF_ABGR_U32:		return ConvertPixel_ARGB_U32_To_ABGR_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_ARGB_U32_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_ABGR_U32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_ABGR_U32_To_ARGB_U32;
		case BitmapData::eBF_ABGR_U32:		return ConvertPixel_NoOp<32>;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_ABGR_U32_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}

	case BitmapData::eBF_RGBA_U32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return ConvertPixel_RGBA_U32_To_A_U1;
		case BitmapData::eBF_A_U5:			return ConvertPixel_RGBA_U32_To_A_U5;
		case BitmapData::eBF_A_U8:			return ConvertPixel_RGBA_U32_To_A_U8;
		case BitmapData::eBF_R_U8:			return ConvertPixel_RGBA_U32_To_R_U8;
		case BitmapData::eBF_X1R5G5B5_U16:	return ConvertPixel_RGBA_U32_To_X1R5G5B5_U16;
		case BitmapData::eBF_R5G6B5_U16:	return ConvertPixel_RGBA_U32_To_R5G6B5_U16;
		case BitmapData::eBF_A1R5G5B5_U16:	return ConvertPixel_RGBA_U32_To_A1R5G5B5_U16;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_RGBA_U32_To_RGB_U24;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_RGBA_U32_To_ARGB_U32;
		case BitmapData::eBF_ABGR_U32:		return ConvertPixel_RGBA_U32_To_ABGR_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_NoOp<32>;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R_F32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U1:			return nullptr;
		case BitmapData::eBF_A_U5:			return nullptr;
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_X1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_A1R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return ConvertPixel_NoOp<32>;
		default:							return nullptr;
		}
	default:
		return nullptr;
	}
}

