#include "precompiled.h"

#include "TGAFile.h"
#include "BitmapData.h"
#include "File.h"

#pragma pack(push)
#pragma pack(1)

struct TGA_Header
{
	char  idlength = 0;
	char  colourmaptype = 0;
	char  datatypecode = 2;
	short int colourmaporigin = 0;
	short int colourmaplength = 0;
	char  colourmapdepth = 0;
	short int x_origin = 0;
	short int y_origin = 0;
	short width = 0;
	short height = 0;
	char  bitsperpixel = 24;
	char  imagedescriptor = 0;
};

#pragma pack(pop)

void SaveTGA(const Path& in_file, BitmapData* in_pData)
{
	BitmapData* pTmp = in_pData->ConvertTo(BitmapData::eBF_ARGB_U32);

	TGA_Header hdr;
	hdr.width = in_pData->GetWidth();
	hdr.height = in_pData->GetHeight();
	hdr.bitsperpixel = pTmp->GetPixelSize() * 8;
	
	File TGAFile;
	TGAFile.Open(in_file, File::fmWriteOnly);
	TGAFile.Write(&hdr, sizeof(TGA_Header));

	unsigned int uiLineSize = pTmp->GetPixelSize() * pTmp->GetWidth();
	char* pCurrentLine = (char*)pTmp->GetBuffer() + (uiLineSize * (pTmp->GetHeight() - 1));
	for (int i = 0; i < pTmp->GetHeight(); i++)
	{
		TGAFile.Write(pCurrentLine, uiLineSize);
		pCurrentLine -= uiLineSize;
	}

	TGAFile.Write(pTmp->GetBuffer(), pTmp->GetBufferSize());
	TGAFile.Close();
}

BitmapData* LoadTGA(const Path& in_file)
{
	return nullptr;
}