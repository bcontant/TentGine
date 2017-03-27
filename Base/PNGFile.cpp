#include "precompiled.h"

#include "PNGFile.h"
#include "BitmapData.h"
#include "png.h"

#ifdef _MSC_VER
#pragma warning(disable:4611)
#endif /* VC++ */

//--------------------------------------------------------------------------------
enum EPNGColorType
{
	ePNG_GrayScale	= 0,
	ePNG_Indexed	= 1 << 0,
	ePNG_TrueColor	= 1 << 1,
	ePNG_Alpha		= 1 << 2
};

//--------------------------------------------------------------------------------
bool SavePNG(const Path& in_file, BitmapData* in_pData)
{
	PROFILE_BLOCK;

	if (in_pData == nullptr)
		return false;

	FILE *fp = nullptr;
	FOPEN(&fp, in_file.GetData(), L("wb"));	
	if (!fp)
	{
		AssertMsg(false, L("File %s could not be opened for writing"), in_file.GetData());
		return false;
	}

	png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

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

	s32 bit_depth = 0;
	s32 color_type = 0;
	switch (in_pData->GetFormat())
	{
	case BufferFormat::A_U1:
		color_type = ePNG_GrayScale;  bit_depth = 1; break;
	case BufferFormat::A_U8:
		color_type = ePNG_GrayScale;  bit_depth = 8; break;
	case BufferFormat::BGR_U24:
		color_type = ePNG_TrueColor;  bit_depth = 8; break;
	case BufferFormat::ABGR_U32:
		color_type = ePNG_TrueColor | ePNG_Alpha;  bit_depth = 8; break;
	default:
		return false;
	}

	png_set_IHDR(png_ptr, info_ptr, in_pData->GetWidth(), in_pData->GetHeight(), bit_depth, color_type, PNG_INTERLACE_NONE,	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
		AssertMsg(false, L("Error during writing bytes"));

	u8* pBuffer = (u8*)in_pData->GetBuffer();
	u8** row_pointers = new u8*[in_pData->GetHeight()];
	for (u32 i = 0; i < in_pData->GetHeight(); i++)
	{
		row_pointers[i] = pBuffer;
		pBuffer += in_pData->GetBufferPitch();
	}

	png_write_image(png_ptr, row_pointers);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		delete[] row_pointers;
		AssertMsg(false, L("Error during end of write"));
		return false;
	}

	png_write_end(png_ptr, nullptr);

	delete[] row_pointers;
	
	fclose(fp);
	return true;
}

//--------------------------------------------------------------------------------
BitmapData* LoadPNG(const Path& in_file)
{
	PROFILE_BLOCK;

	u8 header[8];

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

	png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

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

	s32 width = png_get_image_width(png_ptr, info_ptr);
	s32 height = png_get_image_height(png_ptr, info_ptr);
	u8 color_type = png_get_color_type(png_ptr, info_ptr);
	u8 bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		AssertMsg(false, L("Error during read_image"));
		return nullptr;
	}

	u8* pBuffer = new u8[height * png_get_rowbytes(png_ptr, info_ptr)];
	u8** row_pointers = new u8*[height];
	for (s32 y = 0; y < height; y++)
		row_pointers[y] = pBuffer + (png_get_rowbytes(png_ptr, info_ptr) * y);
	
	png_read_image(png_ptr, row_pointers);

	delete[] row_pointers;

	fclose(fp);

	BufferFormat format;
	if (color_type == 0 && bit_depth == 1)
		format = BufferFormat::A_U1;
	else if (color_type == 0 && bit_depth == 8)
		format = BufferFormat::A_U8;
	else if (color_type == 2 && bit_depth == 8)
		format = BufferFormat::BGR_U24;
	//else if (color_type == 3)
	//	format = BufferFormat::BGR_U24;
	else if (color_type == 6 && bit_depth == 8)
		format = BufferFormat::ABGR_U32;
	else
	{
		Assert(false);
		delete[] pBuffer;
		return nullptr;
	}

	BitmapData* pData = new BitmapData(width, height, pBuffer, format, (u32)png_get_rowbytes(png_ptr, info_ptr));
	return pData;
}

#ifdef _MSC_VER
#pragma warning(default:4611)
#endif /* VC++ */
