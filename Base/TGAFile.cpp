#include "precompiled.h"

#include "TGAFile.h"
#include "BitmapData.h"
#include "File.h"

#pragma pack(push)
#pragma pack(1)

struct TGA_Header
{
	char  IDLength = 0;
	char  ColorMapType = 0;        
	char  ImageType = 2;
	short int ColorMapOrigin = 0;
	short int ColorMapLength = 0;
	char  ColorMapBPP = 0;
	short int XOrigin = 0;
	short int YOrigin = 0;
	short Width = 0;
	short Height = 0;
	char  BPP = 24;
	char  ImageDescriptor = 0;
};

#pragma pack(pop)

enum ETGAImageType
{
	eTGA_NoImage = 0,
	eTGA_ColorMapped = 1,
	eTGA_TrueColor = 2,
	eTGA_Grayscale = 3,
	eTGA_RLE = 1 << 3
};

enum ETGAImageOrigin
{
	eTGA_BottomLeft = 0,
	eTGA_BottomRight = 1,
	eTGA_TopLeft = 2,
	eTGA_TopRight = 3
};

void SaveTGA(const Path& in_file, BitmapData* in_pData, bool in_bCompress)
{
	TGA_Header hdr;

	hdr.IDLength = 0;
	hdr.ColorMapType = 0;
	
	//Like most files, TGA has a BGRA format on disk (which becomes ARGB in memory on little-endian)
	switch (in_pData->GetFormat())
	{
	case BitmapData::eBF_A_U8:		
		hdr.ImageType = eTGA_Grayscale;	hdr.ImageDescriptor = 0;  break;
	case BitmapData::eBF_X1R5G5B5_U16:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 0; break;
	case BitmapData::eBF_A1R5G5B5_U16:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 1; break;
	case BitmapData::eBF_RGB_U24:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 0; break;
	case BitmapData::eBF_ARGB_U32:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 8; break;
	default:	
		AssertMsg(false, "Unsupported format (%d)", in_pData->GetFormat());
		return;
	}

	//TODO
	//if (in_bCompress)
	//	hdr.ImageType |= eTGA_RLE;

	hdr.ColorMapOrigin = 0;
	hdr.ColorMapLength = 0;
	hdr.ColorMapBPP = 0;

	hdr.XOrigin = 0;
	hdr.YOrigin = 0;
	
	hdr.Width = static_cast<short>(in_pData->GetWidth());
	hdr.Height = static_cast<short>(in_pData->GetHeight());
	hdr.BPP = static_cast<char>(in_pData->GetBitsPerPixel());

	hdr.ImageDescriptor |= eTGA_TopLeft << 4;
		
	File TGAFile;
	TGAFile.Open(in_file, File::fmWriteOnly);
	TGAFile.Write(&hdr, sizeof(TGA_Header));

	unsigned int uiLineSize = in_pData->GetBufferPitch();
	unsigned char* pCurrentLine = (unsigned char*)in_pData->GetBuffer();
	for (unsigned int i = 0; i < in_pData->GetHeight(); i++)
	{
		TGAFile.Write(pCurrentLine, uiLineSize);
		pCurrentLine += uiLineSize;
	}

	TGAFile.Close();
}

BitmapData* LoadTGA(const Path& in_file)
{
	File TGAFile;
	if (!TGAFile.Open(in_file, File::fmReadOnly))
		return nullptr;

	TGA_Header hdr;
	TGAFile.Read(&hdr, sizeof(TGA_Header));

	//Skip IDLength
	TGAFile.Advance(hdr.IDLength);

	if (hdr.ColorMapType != 0)
	{
		return nullptr;
	}

	Assert(hdr.ColorMapOrigin == 0);
	Assert(hdr.ColorMapLength == 0);
	Assert(hdr.ColorMapBPP == 0);

	unsigned int uiImageSize = hdr.Width * hdr.Height * (hdr.BPP / 8);
	unsigned char* pImageData = new unsigned char[uiImageSize];

	if (hdr.ImageType & eTGA_RLE)
	{
		unsigned int bytesRead = 0;
		unsigned char* pCursor = pImageData;
		const unsigned char pixelSize = (hdr.BPP / 8);
		static unsigned char value[4];
		unsigned char runCount = 0;

		//Loop for the whole image
		while (bytesRead < uiImageSize)
		{
			//MSB == 0 : Raw data   MSB == 1 : RLE data
			TGAFile.Read(&runCount, 1);
			if (runCount & 0x80)
			{
				runCount -= 127;
				TGAFile.Read(value, pixelSize);
				for (unsigned int i = 0; i < runCount; i++)
				{
					memcpy(pCursor, value, pixelSize);
					pCursor += pixelSize;
				}
			}
			else
			{
				++runCount;
				TGAFile.Read(pCursor, runCount * pixelSize);
				pCursor += runCount * pixelSize;
			}
			bytesRead += runCount * pixelSize;
		}
		Assert(bytesRead == uiImageSize);
	}
	else
	{
		unsigned int bytesRead = TGAFile.Read(pImageData, uiImageSize);
		Assert(bytesRead == uiImageSize);
	}

	BitmapData::EBufferFormat eFormat;
	switch (hdr.ImageType & 0x3)
	{
	case eTGA_NoImage: 
	case eTGA_ColorMapped:
		delete[] pImageData;
		return nullptr;
	case eTGA_TrueColor:
		switch (hdr.BPP)
		{
		case 15: eFormat = BitmapData::eBF_X1R5G5B5_U16; break;
		case 16: eFormat = BitmapData::eBF_A1R5G5B5_U16; break;
		case 24: eFormat = BitmapData::eBF_RGB_U24; break;
		case 32: eFormat = BitmapData::eBF_ARGB_U32; break;
		default: 
			Assert(false); 
			delete[] pImageData;
			return nullptr;
		}
		break;
	case eTGA_Grayscale:
		Assert(hdr.BPP == 8);
		eFormat = BitmapData::eBF_A_U8;
		break;
	default:
		Assert(false);
		delete[] pImageData;
		return nullptr;
	}

	BitmapData* pBitmapData = new BitmapData(hdr.Width, hdr.Height, pImageData, eFormat);
	
	ETGAImageOrigin origin = (ETGAImageOrigin)((hdr.ImageDescriptor >> 4) & 0x3);
	switch (origin)
	{
	case eTGA_TopLeft:
		break;
	case eTGA_BottomLeft:
		pBitmapData->FlipY();
		break;
	case eTGA_BottomRight:
	case eTGA_TopRight:
	default:
		Assert(false); break;
	}

	return pBitmapData;
}