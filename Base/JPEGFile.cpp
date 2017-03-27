#include "precompiled.h"

#include "JPEGFile.h"
#include "BitmapData.h"

#include "jpeglib.h"

#ifdef _MSC_VER
#pragma warning(disable:4611)
#pragma warning(disable:4324)
#endif /* VC++ */

//--------------------------------------------------------------------------------
struct my_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};


//--------------------------------------------------------------------------------
void my_error_exit(j_common_ptr cinfo)
{
	my_error_mgr* myerr = (my_error_mgr*)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

//--------------------------------------------------------------------------------
bool SaveJPEG(const Path& in_file, BitmapData* in_pData, s32 in_Quality)
{
	struct jpeg_compress_struct cinfo;
	//struct jpeg_error_mgr jerr;
	struct my_error_mgr jerr;

	FILE *outfile;                
	JSAMPROW row_pointer[1];      

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	jpeg_create_compress(&cinfo);

	FOPEN(&outfile, in_file.GetData(), L("wb"));

	if (outfile == nullptr)
	{
		Assert(false);
		return false;
	}

	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_compress(&cinfo);
		fclose(outfile);
		return nullptr;
	}


	jpeg_stdio_dest(&cinfo, outfile);

	switch (in_pData->GetFormat())
	{
	case BufferFormat::RGBA_U32:	cinfo.in_color_space = JCS_EXT_ABGR;	cinfo.input_components = 4;	break;
	case BufferFormat::ARGB_U32:	cinfo.in_color_space = JCS_EXT_BGRA;	cinfo.input_components = 4;	break;
	case BufferFormat::ABGR_U32:	cinfo.in_color_space = JCS_EXT_RGBA;	cinfo.input_components = 4;	break;
	case BufferFormat::BGR_U24:		cinfo.in_color_space = JCS_RGB;			cinfo.input_components = 3;	break;
	case BufferFormat::RGB_U24:		cinfo.in_color_space = JCS_EXT_BGR;		cinfo.input_components = 3;	break;
	//case BufferFormat::R5G6B5_U16:	cinfo.in_color_space = JCS_RGB565;		cinfo.input_components = 3;	break;
	case BufferFormat::R_U8:
	case BufferFormat::A_U8:		cinfo.in_color_space = JCS_GRAYSCALE;	cinfo.input_components = 1;	break;
	default:
		Assert(false);
		return false;
	}

	cinfo.image_width = in_pData->GetWidth(); 
	cinfo.image_height = in_pData->GetHeight();

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, in_Quality, true);
	jpeg_start_compress(&cinfo, true);

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		row_pointer[0] = (u8*) &in_pData->GetBuffer()[cinfo.next_scanline * in_pData->GetBufferPitch()];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);

	return true;
}



//--------------------------------------------------------------------------------
BitmapData* LoadJPEG(const Path& in_file)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	
	FILE *infile;
	JSAMPARRAY buffer;

	FOPEN(&infile, in_file.GetData(), L("rb"));
								  
	if (infile == nullptr)
	{
		Assert(false)
		return nullptr;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return nullptr;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, true);
	jpeg_start_decompress(&cinfo);

	BufferFormat eFormat = BufferFormat::INVALID_FORMAT;
	switch (cinfo.out_color_space)
	{
	case JCS_EXT_ABGR:	eFormat = BufferFormat::RGBA_U32; break;
	case JCS_EXT_BGRA:	eFormat = BufferFormat::ARGB_U32; break;
	case JCS_EXT_RGBA:	eFormat = BufferFormat::ABGR_U32; break;
	case JCS_RGB:		eFormat = BufferFormat::BGR_U24; break;
	case JCS_EXT_BGR:	eFormat = BufferFormat::RGB_U24; break;
	//case JCS_RGB565:	eFormat = BufferFormat::R5G6B5_U16; break;
	case JCS_GRAYSCALE: eFormat = BufferFormat::A_U8; break;
	default:
		Assert(false);
		return nullptr;
	}

	BitmapData* pData = new BitmapData(cinfo.output_width, cinfo.output_height, eFormat);

	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, pData->GetBufferPitch(), 1);

	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy((u8*)&(pData->GetBuffer()[(cinfo.output_scanline-1) * pData->GetBufferPitch()]), buffer[0], pData->GetBufferPitch());
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	Assert(jerr.pub.num_warnings == 0);

	return pData;
}

#ifdef _MSC_VER
#pragma warning(default:4611)
#pragma warning(default:4324)
#endif /* VC++ */