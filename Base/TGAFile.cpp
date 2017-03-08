#include "precompiled.h"

#include "TGAFile.h"
#include "BitmapData.h"
#include "File.h"
#include "MemoryBuffer.h"
#include "CompressionUtils.h"

#pragma pack(push)
#pragma pack(1)

//--------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------
enum ETGAImageType
{
	eTGA_NoImage = 0,
	eTGA_ColorMapped = 1,
	eTGA_TrueColor = 2,
	eTGA_Grayscale = 3,
	eTGA_RLE = 1 << 3
};

//--------------------------------------------------------------------------------
enum ETGAImageOrigin
{
	eTGA_BottomLeft = 0,
	eTGA_BottomRight = 1,
	eTGA_TopLeft = 2,
	eTGA_TopRight = 3
};

//--------------------------------------------------------------------------------
bool SaveTGA(const Path& in_file, BitmapData* in_pData, bool in_bCompress)
{
	if (in_pData == nullptr)
		return false;

	TGA_Header hdr;

	hdr.IDLength = 0;
	hdr.ColorMapType = 0;

	//Like most files, TGA has a BGRA (byte-order) format on disk (which becomes ARGB (word-order) in memory on little-endian)
	BitmapData::EBufferFormat convertTo = BitmapData::eBF_Invalid;
	switch (in_pData->GetFormat())
	{
	case BitmapData::eBF_A_U1:
	case BitmapData::eBF_A_U5:
		convertTo = BitmapData::eBF_A_U8;
	case BitmapData::eBF_A_U8:		
		hdr.ImageType = eTGA_Grayscale;	hdr.ImageDescriptor = 0;  break;

	case BitmapData::eBF_R5G6B5_U16:
		convertTo = BitmapData::eBF_X1R5G5B5_U16;
	case BitmapData::eBF_X1R5G5B5_U16:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 0; break;

	case BitmapData::eBF_A1R5G5B5_U16:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 1; break;

	case BitmapData::eBF_R_U8:
	case BitmapData::eBF_BGR_U24:
	case BitmapData::eBF_R_F32:
		convertTo = BitmapData::eBF_RGB_U24;
	case BitmapData::eBF_RGB_U24:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 0; break;

	case BitmapData::eBF_RGBA_U32:
	case BitmapData::eBF_ABGR_U32:
		convertTo = BitmapData::eBF_ARGB_U32;
	case BitmapData::eBF_ARGB_U32:
		hdr.ImageType = eTGA_TrueColor; hdr.ImageDescriptor = 8; break;

	default:	
		AssertMsg(false, L("Unsupported format (%d)"), in_pData->GetFormat());
		return false;
	}

	File TGAFile;
	if (!TGAFile.Open(in_file, File::fmWriteOnly))
		return false;

	if (convertTo != BitmapData::eBF_Invalid)
	{
		in_pData = new BitmapData(*in_pData);
		in_pData->ConvertTo(convertTo);
	}

	if (in_bCompress)
		hdr.ImageType |= eTGA_RLE;

	hdr.ColorMapOrigin = 0;
	hdr.ColorMapLength = 0;
	hdr.ColorMapBPP = 0;

	hdr.XOrigin = 0;
	hdr.YOrigin = 0;
	
	hdr.Width = static_cast<short>(in_pData->GetWidth());
	hdr.Height = static_cast<short>(in_pData->GetHeight());
	hdr.BPP = static_cast<char>(in_pData->GetBitsPerPixel());

	hdr.ImageDescriptor |= eTGA_TopLeft << 4;
	
	TGAFile.Write(&hdr, sizeof(TGA_Header));

	if (!in_bCompress)
	{
		TGAFile.Write(in_pData->GetBuffer(), in_pData->GetBufferSize());
	}
	else
	{
		MemoryBuffer SrcBuffer((unsigned char*)in_pData->GetBuffer(), in_pData->GetBufferSize());
		RLE_Encode(&SrcBuffer, &TGAFile, hdr.BPP / 8);
	}

	TGAFile.Close();

	if (convertTo != BitmapData::eBF_Invalid)
	{
		delete in_pData;
	}

	return true;
}

//--------------------------------------------------------------------------------
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
		return nullptr;

	Assert(hdr.ColorMapOrigin == 0);
	Assert(hdr.ColorMapLength == 0);
	Assert(hdr.ColorMapBPP == 0);

	BitmapData::EBufferFormat eFormat;
	switch (hdr.ImageType & 0x3)
	{
	case eTGA_TrueColor:
		switch (hdr.BPP)
		{
		case 15: eFormat = BitmapData::eBF_X1R5G5B5_U16; break;
		case 16: eFormat = BitmapData::eBF_A1R5G5B5_U16; break;
		case 24: eFormat = BitmapData::eBF_RGB_U24; break;
		case 32: eFormat = BitmapData::eBF_ARGB_U32; break;
		default:
			Assert(false);
			return nullptr;
		}
		break;

	case eTGA_Grayscale:
		switch (hdr.BPP)
		{
		case 8: eFormat = BitmapData::eBF_A_U8; break;
		default:
			Assert(false);
			return nullptr;
		}
		break;

	case eTGA_NoImage:
	case eTGA_ColorMapped:
	default:
		Assert(false);
		return nullptr;
	}

	unsigned int uiImageSize = hdr.Width * hdr.Height * (hdr.BPP / 8);
	unsigned char* pImageData = new unsigned char[uiImageSize];

	if (hdr.ImageType & eTGA_RLE)
	{
		MemoryBuffer DstBuffer(pImageData, uiImageSize);
		RLE_Decode(&TGAFile, &DstBuffer, uiImageSize, hdr.BPP / 8);
	}
	else
	{
		unsigned int bytesRead = TGAFile.Read(pImageData, uiImageSize);
		Assert(bytesRead == uiImageSize);
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