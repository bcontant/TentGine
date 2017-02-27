#include "precompiled.h"

#include "BitmapData.h"

BitmapData::BitmapData(int in_uiWidth, int in_uiHeight, EBufferFormat in_eFormat)
	: m_uiWidth(in_uiWidth)
	, m_uiHeight(in_uiHeight)
	, m_eFormat(in_eFormat)
{
	m_pBuffer = new unsigned char[GetBufferSize()];
	memset(m_pBuffer, 0, GetBufferSize());
}

BitmapData::BitmapData(int in_uiWidth, int in_uiHeight, void* in_pBuffer, EBufferFormat in_eFormat)
	: BitmapData(in_uiWidth, in_uiHeight, in_eFormat)
{
	memcpy(m_pBuffer, in_pBuffer, GetBufferSize());
}

BitmapData* BitmapData::ConvertTo(EBufferFormat in_eFormat)
{
	if (in_eFormat == m_eFormat)
		return nullptr;

	BitmapData* pNewBitmap = new BitmapData(m_uiWidth, m_uiHeight, in_eFormat);

	PixelValue v;
	PixelValue newV;
	
	for (int x = 0; x < m_uiWidth; x++)
	{
		for (int y = 0; y < m_uiHeight; y++)
		{
			v = Get(x, y);
			newV = v.ConvertTo(v, m_eFormat, in_eFormat);
			pNewBitmap->Set(x, y, newV);
		}
	}
	return pNewBitmap;
}

BitmapData::PixelValue BitmapData::Get(int in_x, int in_y) const
{
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Get : Invalid x or y"));

	PixelValue p;
	memset(&p, 0, sizeof(PixelValue));
	memcpy(&p, &m_pBuffer[in_y * m_uiWidth * GetPixelSize() + in_x * GetPixelSize()], GetPixelSize());
	return p;
}

void BitmapData::Blit(int in_x, int in_y, const BitmapData* in_pData)
{
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Blit : Invalid x or y"));
	AssertMsg(in_pData->m_eFormat == m_eFormat, L("BitmapData::Blit : Invalid format"));
	AssertMsg(in_x + in_pData->m_uiWidth < m_uiWidth && in_y + in_pData->m_uiHeight < m_uiHeight, L("BitmapData::Blit : Invalid size"));

	int pixelSize = GetPixelSize();
	
	for (int y = 0; y < in_pData->m_uiHeight; y++)
	{
		void* pDest = m_pBuffer + (y + in_y) * m_uiWidth * GetPixelSize() + in_x * pixelSize;
		void* pSrc = in_pData->m_pBuffer + y * in_pData->m_uiWidth * pixelSize;

		memcpy(pDest, pSrc, in_pData->m_uiWidth * pixelSize);
	}
}

void BitmapData::Set(int in_x, int in_y, const BitmapData::PixelValue in_value)
{
	AssertMsg(in_x < m_uiWidth && in_y < m_uiHeight, L("BitmapData::Set : Invalid x or y"));

	memcpy(&m_pBuffer[in_y * m_uiWidth * GetPixelSize() + in_x * GetPixelSize()], &in_value, GetPixelSize());
}

unsigned int BitmapData::GetPixelSize() const
{
	unsigned int pixelSize = 1;
	switch (m_eFormat)
	{
	case eBF_U8:		pixelSize = 1; break;
	case eBF_RGB_U24:	pixelSize = 3; break;
	case eBF_RGBA_U32:
	case eBF_ARGB_U32:
	case eBF_R_F32:		pixelSize = 4; break;
	default:			pixelSize = 0; break;
	}

	return pixelSize;
}

unsigned int BitmapData::GetBufferSize() const
{
	return m_uiWidth * m_uiHeight * GetPixelSize();
}

BitmapData::PixelValue BitmapData::PixelValue::ConvertTo(const PixelValue& in_v, EBufferFormat in_eSrcFormat, EBufferFormat in_eDstFormat)
{
	PixelValue v;
	if (in_eSrcFormat == eBF_U8 && in_eDstFormat == eBF_RGB_U24)
	{
		v.uiValue = (in_v.ucValue << 16) + (in_v.ucValue << 8) + in_v.ucValue;
	}
	else if (in_eSrcFormat == eBF_U8 && in_eDstFormat == eBF_ARGB_U32)
	{
		v.uiValue = (255 << 24) + (in_v.ucValue << 16) + (in_v.ucValue << 8) + in_v.ucValue;
	}
	else if (in_eSrcFormat == eBF_U8 && in_eDstFormat == eBF_RGBA_U32)
	{
		v.uiValue = (in_v.ucValue << 24) + (in_v.ucValue << 16) + (in_v.ucValue << 8) + 255;
	}
	else
	{
		AssertMsg(false, "Invalid Conversion");
	}
	return v;
}