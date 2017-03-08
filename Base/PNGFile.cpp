#include "precompiled.h"

#include "PNGFile.h"
#include "BitmapData.h"
#include "png.h"

#ifdef _MSC_VER
#pragma warning(disable:4611)
#endif /* VC++ */

bool SavePNG(const Path& in_file, BitmapData* in_pData)
{
	FILE *fp = nullptr;
	FOPEN(&fp, in_file.GetData(), L("wb"));	
	if (!fp)
	{
		AssertMsg(false, L("File %s could not be opened for writing"), in_file.GetData());
		return false;
	}

	png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		AssertMsg(false, L("png_create_write_struct failed"));
		return false;
	}

	png_info* info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		AssertMsg(false, L("png_create_info_struct failed"));
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		AssertMsg(false, L("Error during init_io"));
		return false;
	}

	png_init_io(png_ptr, fp);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		AssertMsg(false, L("Error during writing header"));
		return false;
	}

	int bit_depth = 0;
	int color_type = 0;
	switch (in_pData->GetFormat())
	{
	case BitmapData::eBF_A_U1:		color_type = 0;  bit_depth = 1; break;
	case BitmapData::eBF_A_U8:		color_type = 0;  bit_depth = 8; break;
	case BitmapData::eBF_BGR_U24:	color_type = 2;  bit_depth = 8; break;
	case BitmapData::eBF_ABGR_U32:	color_type = 6;  bit_depth = 8; break;
	default:
		return false;
	}

	png_set_IHDR(png_ptr, info_ptr, in_pData->GetWidth(), in_pData->GetHeight(), bit_depth, color_type, PNG_INTERLACE_NONE,	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		AssertMsg(false, L("Error during writing bytes"));

	unsigned char* pBuffer = (unsigned char*)in_pData->GetBuffer();
	unsigned char** row_pointers = new unsigned char*[in_pData->GetHeight()];
	for (unsigned int i = 0; i < in_pData->GetHeight(); i++)
	{
		row_pointers[i] = pBuffer;
		pBuffer += in_pData->GetBufferPitch();
	}

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		delete[] row_pointers;
		AssertMsg(false, L("Error during end of write"));
		return false;
	}

	png_write_end(png_ptr, NULL);

	delete[] row_pointers;
	
	fclose(fp);
	return true;
}

BitmapData* LoadPNG(const Path& in_file)
{
	unsigned char header[8];

	FILE *fp = nullptr;
	FOPEN(&fp, in_file.GetData(), L("rb"));
	if (!fp)
	{
		AssertMsg(false, L("File %s could not be opened for reading"), in_file.GetData());
		return nullptr;
	}

	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		AssertMsg(false, L("File %s is not recognized as a PNG file"), in_file.GetData());
		return nullptr;
	}

	png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		AssertMsg(false, L("png_create_read_struct failed"));
		return nullptr;
	}

	png_info* info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		AssertMsg(false, L("png_create_info_struct failed"));
		return nullptr;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		AssertMsg(false, L("Error during init_io"));
		return nullptr;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	int width = png_get_image_width(png_ptr, info_ptr);
	int height = png_get_image_height(png_ptr, info_ptr);
	unsigned char color_type = png_get_color_type(png_ptr, info_ptr);
	unsigned char bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		AssertMsg(false, L("Error during read_image"));
		return nullptr;
	}

	unsigned char* pBuffer = new unsigned char[height * png_get_rowbytes(png_ptr, info_ptr)];
	unsigned char** row_pointers = new unsigned char*[height];
	for (int y = 0; y < height; y++)
		row_pointers[y] = pBuffer + (png_get_rowbytes(png_ptr, info_ptr) * y);
	
	png_read_image(png_ptr, row_pointers);

	delete[] row_pointers;

	fclose(fp);

	BitmapData::EBufferFormat format;
	if (color_type == 0 && bit_depth == 1)
		format = BitmapData::eBF_A_U1;
	if (color_type == 0 && bit_depth == 8)
		format = BitmapData::eBF_A_U8;
	else if (color_type == 2 && bit_depth == 8)
		format = BitmapData::eBF_BGR_U24;
	else if (color_type == 3)
		format = BitmapData::eBF_BGR_U24;
	else if (color_type == 6 && bit_depth == 8)
		format = BitmapData::eBF_ABGR_U32;
	else
	{
		Assert(false);
		delete[] pBuffer;
		return nullptr;
	}

	BitmapData* pData = new BitmapData(width, height, pBuffer, format, (unsigned int)png_get_rowbytes(png_ptr, info_ptr));
	return pData;
}

#ifdef _MSC_VER
#pragma warning(default:4611)
#endif /* VC++ */
