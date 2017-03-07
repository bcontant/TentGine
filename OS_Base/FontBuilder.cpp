#include "precompiled.h"

#include <algorithm>

#include "FontBuilder.h"
#include "../Base/BitmapData.h"
#include "../Base/GraphicUtils.h"
#include "../Base/TGAFile.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#define GLYPH_TEXTURE_PADDING 2

void OS::BuildFont(const Path& in_FontFile, unsigned int in_uiFontSize, unsigned int in_uiTextureSize, unsigned int in_uiFlags, const Path& in_FontDataFileName)
{
	//Init the string we'll use for creating the FontFileTexture
	//TODO : Parametrize
	wchar_t wszTextureText[512] = {};
	for (wchar_t i = 1; i <= 255; i++)
		wszTextureText[i - 1] = i;
	unsigned int cTextureTextLength = (unsigned int)wcslen(wszTextureText);

	FT_Library library;
	FT_Error error;
	error = FT_Init_FreeType(&library);

	FT_Face face;
	error = FT_New_Face(library, TO_STRING(in_FontFile.GetData()).c_str(), 0, &face);
	error = FT_Set_Char_Size(face, 0, in_uiFontSize * 64, 96, 96);  //This is correct on my (96DPI) display.  Glyph are the proper size (compared with MS Word)

	struct Glyph
	{
		~Glyph() { delete m_pData; }

		FontDataFile::GlyphInfo m_Info;
		BitmapData* m_pData;
	};

	std::map<unsigned int, Glyph> mGlyphs;
	std::map<wchar_t, Glyph*> mCharacters;
	std::vector<PackRect> vUnpackRects;


	unsigned int uiLoadFlags = FT_LOAD_DEFAULT;
	if (in_uiFlags & eFF_Mono)			
		uiLoadFlags |= FT_LOAD_TARGET_MONO;
	if (in_uiFlags & eFF_ForceAutoHint)	
		uiLoadFlags |= FT_LOAD_FORCE_AUTOHINT;

	FT_Render_Mode eRenderMode = FT_RENDER_MODE_NORMAL;
	if (in_uiFlags & eFF_Mono)			
		eRenderMode = FT_RENDER_MODE_MONO;

	BitmapData::EBufferFormat eBufferFormat = BitmapData::eBF_A_U8;
	if (in_uiFlags & eFF_Mono)			
		eBufferFormat = BitmapData::eBF_A_U1;

	for (unsigned int i = 0; i < cTextureTextLength; i++)
	{
		unsigned int glyphIndex = FT_Get_Char_Index(face, wszTextureText[i]);

		error = FT_Load_Glyph(face, glyphIndex, uiLoadFlags);
		error = FT_Render_Glyph(face->glyph, eRenderMode);

		if (error == 0 && face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
		{
			if (mGlyphs.find(glyphIndex) == mGlyphs.end())
			{
				mGlyphs[glyphIndex].m_Info.left = face->glyph->bitmap_left;
				mGlyphs[glyphIndex].m_Info.top = face->glyph->bitmap_top;
				mGlyphs[glyphIndex].m_Info.advance = face->glyph->advance.x / 64;
				mGlyphs[glyphIndex].m_pData = new BitmapData(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, eBufferFormat, face->glyph->bitmap.pitch);

				PackRect rc = { 0, 0, mGlyphs[glyphIndex].m_pData->GetWidth(), mGlyphs[glyphIndex].m_pData->GetHeight(), &mGlyphs[glyphIndex] };
				vUnpackRects.push_back(rc);
			}
			mCharacters[wszTextureText[i]] = &mGlyphs[glyphIndex];
		}
	}

	auto vPackRects = PackTexture(vUnpackRects, in_uiTextureSize, in_uiTextureSize, GLYPH_TEXTURE_PADDING);
	Assert(vUnpackRects.size() == 0);

	BitmapData* pRects = new BitmapData(in_uiTextureSize, in_uiTextureSize, eBufferFormat);
	for (size_t i = 0; i < vPackRects.size(); i++)
	{
		pRects->Blit(vPackRects[i].x, vPackRects[i].y, ((Glyph*)vPackRects[i].user_data)->m_pData);
	}

	std::map<wchar_t, FontDataFile::GlyphInfo> mFinalGlyphs;
	for (auto charIt = mCharacters.begin(); charIt != mCharacters.end(); charIt++)
	{
		Glyph* pCurrentGlyph = charIt->second;
		auto it = std::find_if(vPackRects.begin(), vPackRects.end(), [pCurrentGlyph](const PackRect& a) { return a.user_data == pCurrentGlyph; });
		if (it == vPackRects.end())
			continue;

		//UVs
		pCurrentGlyph->m_Info.boxX = float(it->x) / in_uiTextureSize;
		pCurrentGlyph->m_Info.boxY = float(it->y) / in_uiTextureSize;
		pCurrentGlyph->m_Info.boxWidth = float(it->size_x) / in_uiTextureSize;
		pCurrentGlyph->m_Info.boxHeight = float(it->size_y) / in_uiTextureSize;

		mFinalGlyphs[charIt->first] = pCurrentGlyph->m_Info;
	}

	pRects->ConvertTo(BitmapData::eBF_A_U8);
	//SaveTGA(L("f:\\fontrect.tga"), pRects);

	StdString fontName = Format(L("%s %s %dpt%s"), FROM_STRING(face->family_name).c_str(), FROM_STRING(face->style_name).c_str(), in_uiFontSize, in_uiFlags & eFF_Mono ? L(" Mono") : L(""));
	
	FontDataFile* newFont = new FontDataFile(fontName, in_uiFontSize, in_uiTextureSize, mFinalGlyphs, (const unsigned char*)pRects->GetBuffer());

	Path fontDataFile = in_FontDataFileName;
	if (fontDataFile == L(""))
		fontDataFile = fontName + L(".font");

	newFont->Save(fontDataFile);
	delete newFont;

	error = FT_Done_FreeType(library);
}