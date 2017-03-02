#include "precompiled.h"

#include "BitmapData.h"

bool RequiresConversion(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat);

using ConvertPixelFunc = void(*)(const unsigned char*, unsigned char*);
ConvertPixelFunc GetConvertPixelFunction(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat);

BitmapData::BitmapData(int in_uiWidth, int in_uiHeight, EBufferFormat in_eFormat)
	: m_uiWidth(in_uiWidth)
	, m_uiHeight(in_uiHeight)
	, m_eFormat(in_eFormat)
{
	UpdatePixelSize();

	m_pBuffer = new unsigned char[GetBufferSize()];
	memset(m_pBuffer, 0, GetBufferSize());	
}

BitmapData::BitmapData(int in_uiWidth, int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat)
	: BitmapData(in_uiWidth, in_uiHeight, in_eFormat)
{
	memcpy(m_pBuffer, in_pBuffer, GetBufferSize());
}

BitmapData* BitmapData::ConvertTo(EBufferFormat in_eFormat) const
{
	if ( !RequiresConversion(in_eFormat, m_eFormat) )
		return new BitmapData(m_uiWidth, m_uiHeight, m_pBuffer, in_eFormat);

	ConvertPixelFunc f = GetConvertPixelFunction(m_eFormat, in_eFormat);
	if (f == nullptr)
		return nullptr;

	BitmapData* pNewBitmap = new BitmapData(m_uiWidth, m_uiHeight, in_eFormat);

	unsigned char* pSrc = (unsigned char*)m_pBuffer;
	unsigned char* pDst = (unsigned char*)pNewBitmap->m_pBuffer;

	for (int pixelIndex = 0; pixelIndex < m_uiHeight * m_uiWidth; pixelIndex++)
	{
		f(pSrc, pDst);

		pSrc += m_uiPixelSize;
		pDst += pNewBitmap->m_uiPixelSize;
	}

	return pNewBitmap;
}

const void* BitmapData::Get(int in_x, int in_y) const
{
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Get : Invalid x or y"));

	return &m_pBuffer[in_y * m_uiWidth * m_uiPixelSize + in_x * m_uiPixelSize];
}

void BitmapData::Set(int in_x, int in_y, const void* in_pValue)
{
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Set : Invalid x or y"));

	memcpy(&m_pBuffer[in_y * m_uiWidth * m_uiPixelSize + in_x * m_uiPixelSize], in_pValue, m_uiPixelSize);
}

void BitmapData::Blit(int in_x, int in_y, const BitmapData* in_pData)
{
	//TODO : Convert?
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Blit : Invalid x or y"));
	AssertMsg(in_pData->m_eFormat == m_eFormat, L("BitmapData::Blit : Invalid format"));
	AssertMsg(in_x + in_pData->m_uiWidth <= m_uiWidth && in_y + in_pData->m_uiHeight <= m_uiHeight, L("BitmapData::Blit : Invalid size"));

	int pixelSize = GetPixelSize();
	
	for (int y = 0; y < in_pData->m_uiHeight; y++)
	{
		void* pDest = m_pBuffer + (y + in_y) * m_uiWidth * GetPixelSize() + in_x * pixelSize;
		void* pSrc = in_pData->m_pBuffer + y * in_pData->m_uiWidth * pixelSize;

		memcpy(pDest, pSrc, in_pData->m_uiWidth * pixelSize);
	}
}

void BitmapData::UpdatePixelSize()
{
	switch (m_eFormat)
	{
	case eBF_A_U8:
	case eBF_R_U8:			m_uiPixelSize = 1; break;

	case eBF_R5G5B5_U16:
	case eBF_R5G6B5_U16:
	case eBF_R5G5B5A1_U16:	m_uiPixelSize = 2; break;

	case eBF_RGB_U24:		m_uiPixelSize = 3; break;

	case eBF_RGBA_U32:
	case eBF_ARGB_U32:
	case eBF_R_F32:			m_uiPixelSize = 4; break;

	default:				m_uiPixelSize = 0; break;
	}
}

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

void ConvertPixel_NoOp_8(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_NoOp_16(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_NoOp_16(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_NoOp_24(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_NoOp_32(const unsigned char* in_pSrc, unsigned char* in_pDst);

void ConvertPixel_A_U8_To_R5G5B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_A_U8_To_R5G6B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_A_U8_To_R5G5B5A1_U16(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_A_U8_To_RGB_U24(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_A_U8_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst);
void ConvertPixel_A_U8_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst);

ConvertPixelFunc GetConvertPixelFunction(BitmapData::EBufferFormat in_eSrcFormat, BitmapData::EBufferFormat in_eDstFormat)
{
	//TODO : Full support

	switch (in_eSrcFormat)
	{
	case BitmapData::eBF_A_U8:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return ConvertPixel_NoOp_8;
		case BitmapData::eBF_R_U8:			return ConvertPixel_NoOp_8;
		case BitmapData::eBF_R5G5B5_U16:	return ConvertPixel_A_U8_To_R5G5B5_U16;
		case BitmapData::eBF_R5G6B5_U16:	return ConvertPixel_A_U8_To_R5G6B5_U16;
		case BitmapData::eBF_R5G5B5A1_U16:	return ConvertPixel_A_U8_To_R5G5B5A1_U16;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_A_U8_To_RGB_U24;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_A_U8_To_ARGB_U32;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_A_U8_To_RGBA_U32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R_U8:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return ConvertPixel_NoOp_8;
		case BitmapData::eBF_R_U8:			return ConvertPixel_NoOp_8;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R5G5B5_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return ConvertPixel_NoOp_16;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R5G6B5_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return ConvertPixel_NoOp_16;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R5G5B5A1_U16:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return ConvertPixel_NoOp_16;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_RGB_U24:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return ConvertPixel_NoOp_24;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_ARGB_U32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return ConvertPixel_NoOp_32;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_RGBA_U32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return ConvertPixel_NoOp_32;
		case BitmapData::eBF_R_F32:			return nullptr;
		default:							return nullptr;
		}
	case BitmapData::eBF_R_F32:
		switch (in_eDstFormat)
		{
		case BitmapData::eBF_A_U8:			return nullptr;
		case BitmapData::eBF_R_U8:			return nullptr;
		case BitmapData::eBF_R5G5B5_U16:	return nullptr;
		case BitmapData::eBF_R5G6B5_U16:	return nullptr;
		case BitmapData::eBF_R5G5B5A1_U16:	return nullptr;
		case BitmapData::eBF_RGB_U24:		return nullptr;
		case BitmapData::eBF_ARGB_U32:		return nullptr;
		case BitmapData::eBF_RGBA_U32:		return nullptr;
		case BitmapData::eBF_R_F32:			return ConvertPixel_NoOp_32;
		default:							return nullptr;
		}
	default:
		return nullptr;
	}
}

void ConvertPixel_NoOp_8(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	in_pDst[0] = in_pSrc[0];
}

void ConvertPixel_NoOp_16(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	(*(unsigned short*)in_pDst) = (*(unsigned short*)in_pSrc);
}

void ConvertPixel_NoOp_24(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	memcpy(in_pDst, in_pSrc, 3);
}

void ConvertPixel_NoOp_32(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	(*(unsigned int*)in_pDst) = (*(unsigned int*)in_pSrc);
}

void ConvertPixel_A_U8_To_R5G5B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//No alpha channel to convert U8 to greyscale
	(*(unsigned short*)in_pDst) = (1 << 15) + ((*in_pSrc >> 3) << 10) + ((*in_pSrc >> 3) << 5) + (*in_pSrc >> 3);
}

void ConvertPixel_A_U8_To_R5G6B5_U16(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//No alpha channel to convert U8 to greyscale
	(*(unsigned short*)in_pDst) = ((*in_pSrc >> 3) << 11) + ((*in_pSrc >> 2) << 5) + (*in_pSrc >> 3);
}

void ConvertPixel_A_U8_To_R5G5B5A1_U16(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//Set Alpha to U8 value
	(*(unsigned short*)in_pDst) = ((*in_pSrc >> 7) << 15);
}

void ConvertPixel_A_U8_To_RGB_U24(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//No alpha channel to convert U8 to greyscale
	in_pDst[0] = *in_pSrc; //B
	in_pDst[1] = *in_pSrc; //G
	in_pDst[2] = *in_pSrc; //R
}

void ConvertPixel_A_U8_To_ARGB_U32(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//Set Alpha to U8 value
	(*(unsigned int*)in_pDst) = (*in_pSrc << 24);
}

void ConvertPixel_A_U8_To_RGBA_U32(const unsigned char* in_pSrc, unsigned char* in_pDst)
{
	//Set Alpha to U8 value
	(*(unsigned int*)in_pDst) = *in_pSrc;
}

