#include "precompiled.h"

#include <algorithm>

#include "FontBuilder.h"
#include "../Base/BitmapData.h"
#include "../Base/GraphicUtils.h"
#include "../Base/TGAFile.h"
#include "../Base/PNGFile.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#define GLYPH_TEXTURE_PADDING 0

struct Glyph
{
	~Glyph() { delete m_pData; }

	FontDataFile::GlyphInfo m_Info;
	BitmapData* m_pData;
};

//--------------------------------------------------------------------------------
bool OS::BuildFont(const Path& in_FontFile, unsigned int in_uiFontSize, unsigned int in_uiTextureSize, unsigned int in_uiFlags, const Path& in_FontDataFileName)
{
	PROFILE_BLOCK;

	//Init the string we'll use for creating the FontFileTexture
	//TODO : Parametrize
	wchar_t wszTextureText[512] = {};
	for (wchar_t i = 1; i < 256; i++)
		wszTextureText[i - 1] = i;
	unsigned int cTextureTextLength = (unsigned int)wcslen(wszTextureText);

	FT_Library library;
	FT_Error error;
	error = FT_Init_FreeType(&library);

	FT_Face face;
	error = FT_New_Face(library, TO_STRING(in_FontFile.GetData()).c_str(), 0, &face);
	error = FT_Set_Char_Size(face, 0, in_uiFontSize * 64, 96, 96);  //This is correct on my (96DPI) display.  Glyph are the proper size (compared with MS Word)


	unsigned int uiLoadFlags = FT_LOAD_DEFAULT;
	if (in_uiFlags & eFF_Mono)			
		uiLoadFlags |= FT_LOAD_TARGET_MONO;
	if (in_uiFlags & eFF_ForceAutoHint)	
		uiLoadFlags |= FT_LOAD_FORCE_AUTOHINT;

	FT_Render_Mode eRenderMode = FT_RENDER_MODE_NORMAL;
	if (in_uiFlags & eFF_Mono)			
		eRenderMode = FT_RENDER_MODE_MONO;

	BufferFormat eBufferFormat = BufferFormat::A_U8;
	if (in_uiFlags & eFF_Mono)			
		eBufferFormat = BufferFormat::A_U1;

	std::map<unsigned int, Glyph> mGlyphs;
	std::map<wchar_t, Glyph*> mCharacters;
	std::vector<PackRect> vUnpackRects;

	for (unsigned int i = 0; i < cTextureTextLength; i++)
	{
		unsigned int glyphIndex = FT_Get_Char_Index(face, wszTextureText[i]);

		error = FT_Load_Glyph(face, glyphIndex, uiLoadFlags);
		error = FT_Render_Glyph(face->glyph, eRenderMode);

		Assert(error == 0);
		if (error == 0)
		{
			if (mGlyphs.find(glyphIndex) == mGlyphs.end())
			{
				mGlyphs[glyphIndex].m_Info.m_GlyphBox.m_left = face->glyph->bitmap_left;
				mGlyphs[glyphIndex].m_Info.m_GlyphBox.m_right = face->glyph->bitmap_left + face->glyph->bitmap.width;
				mGlyphs[glyphIndex].m_Info.m_GlyphBox.m_top = face->glyph->bitmap_top;
				mGlyphs[glyphIndex].m_Info.m_GlyphBox.m_bottom = face->glyph->bitmap_top - face->glyph->bitmap.rows;
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

	BitmapData* pFontTexture = new BitmapData(in_uiTextureSize, in_uiTextureSize, eBufferFormat);
	for (size_t i = 0; i < vPackRects.size(); i++)
	{
		pFontTexture->Blit(vPackRects[i].x, vPackRects[i].y, ((Glyph*)vPackRects[i].user_data)->m_pData);
	}

	std::vector<FontDataFile::GlyphInfo*> vGlyphs;
	std::map<wchar_t, FontDataFile::GlyphInfo*> mCharacterMap;

	for (auto glyphIt = mGlyphs.begin(); glyphIt != mGlyphs.end(); glyphIt++)
	{
		Glyph* pCurrentGlyph = &glyphIt->second;
		auto packRectIt = std::find_if(vPackRects.begin(), vPackRects.end(), [pCurrentGlyph](const PackRect& a) { return a.user_data == pCurrentGlyph; });
		if (packRectIt == vPackRects.end())
		{
			Assert(false);
			return false;
			continue;
		}

		FontDataFile::GlyphInfo* pNewInfo = new FontDataFile::GlyphInfo;
		pNewInfo->m_GlyphBox = pCurrentGlyph->m_Info.m_GlyphBox;
		pNewInfo->advance = pCurrentGlyph->m_Info.advance;

		pNewInfo->m_UVs.m_left = float(packRectIt->x) / in_uiTextureSize;
		pNewInfo->m_UVs.m_right = float(packRectIt->x + packRectIt->size_x) / in_uiTextureSize;
		pNewInfo->m_UVs.m_top = float(packRectIt->y) / in_uiTextureSize;
		pNewInfo->m_UVs.m_bottom = float(packRectIt->y + packRectIt->size_y) / in_uiTextureSize;

		vGlyphs.push_back(pNewInfo);

		auto charIt = mCharacters.begin();
		for(;;)
		{
			charIt = std::find_if(charIt, mCharacters.end(), [pCurrentGlyph](const auto& a) { return a.second == pCurrentGlyph; });
			if (charIt == mCharacters.end())
				break;

			mCharacterMap[charIt->first] = pNewInfo;
			charIt++;
		}
	}

	StdString fontName = Format(L("%s %s %dpt%s"), FROM_STRING(face->family_name).c_str(), FROM_STRING(face->style_name).c_str(), in_uiFontSize, in_uiFlags & eFF_Mono ? L(" Mono") : L(""));

	Path fontDataFile = in_FontDataFileName;
	if (fontDataFile.GetFileName() == L(""))
	{
		fontDataFile = fontDataFile.GetFolder() + fontName + L(".font");
	}
	
	FontDataFile* newFont = new FontDataFile;
	newFont->m_FontName = fontName;
	newFont->m_FontSize = in_uiFontSize;
	newFont->m_LineHeight = face->size->metrics.height / 64;
	newFont->m_MaxAscender = face->size->metrics.ascender / 64;

	newFont->m_vGlyphs = vGlyphs;
	newFont->m_mCharacterMap = mCharacterMap;

	newFont->m_FontTextureFilename = fontDataFile;
	newFont->m_FontTextureFilename.SetExtension(L(".png"));

	pFontTexture->ConvertTo(BufferFormat::A_U8);
	newFont->m_pFontTextureData = pFontTexture;

	newFont->Save(fontDataFile);
	delete newFont;

	error = FT_Done_FreeType(library);

	return true;
}