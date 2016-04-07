#include "FontDataFile.h"


FontDataFile::FontDataFile(const std::wstring& in_FontName, float in_FontSize, unsigned int in_TextureSize, std::map<wchar_t, GlyphInfo> in_mGlyphs, const unsigned char* in_pData)
	: m_FontName(in_FontName)
	, m_FontSize(in_FontSize)
	, m_TextureSize(in_TextureSize) 
{
	m_pFontTextureData = new unsigned char[m_TextureSize * m_TextureSize];
	memcpy(m_pFontTextureData, in_pData, m_TextureSize * m_TextureSize);

	m_mGlyphs = in_mGlyphs;
}

FontDataFile::~FontDataFile()
{
	delete[] m_pFontTextureData;
	m_pFontTextureData = NULL;
}

void FontDataFile::Save(std::wstring in_filename) const
{
	FILE *fp = NULL;
	_wfopen_s(&fp, in_filename.c_str(), L"wb");

	int fontNameLen = (int)m_FontName.size();
	fwrite(&fontNameLen, sizeof(unsigned int), 1, fp);
	fwrite(m_FontName.c_str(), sizeof(wchar_t), fontNameLen, fp);
	fwrite(&m_FontSize, sizeof(float), 1, fp);

	int glyphCount = (int)m_mGlyphs.size();
	fwrite(&glyphCount, sizeof(unsigned int), 1, fp);

	std::map<wchar_t, GlyphInfo>::const_iterator it = m_mGlyphs.cbegin();
	for (it; it != m_mGlyphs.cend(); ++it)
	{
		fwrite(&it->first, sizeof(wchar_t), 1, fp);
		fwrite(&it->second, sizeof(GlyphInfo), 1, fp);
	}

	fwrite(&m_TextureSize, sizeof(unsigned int), 1, fp);
	fwrite(m_pFontTextureData, sizeof(unsigned char), m_TextureSize * m_TextureSize, fp);
	fclose(fp);
}

void FontDataFile::Load(std::wstring  in_filename)
{
	FILE *fp = NULL;
	_wfopen_s(&fp, in_filename.c_str(), L"rb");

	unsigned int fontNameLen;
	fread(&fontNameLen, sizeof(unsigned int), 1, fp);
	wchar_t* fontName = new wchar_t[fontNameLen + 1];
	fontName[fontNameLen] = 0;
	fread(fontName, sizeof(wchar_t), fontNameLen, fp);
	m_FontName = fontName;
	delete[] fontName;

	fread(&m_FontSize, sizeof(float), 1, fp);

	unsigned int glyphCount;
	fread(&glyphCount, sizeof(unsigned int), 1, fp);

	for (unsigned int i = 0; i < glyphCount; i++)
	{
		wchar_t glyphCharacter;
		fread(&glyphCharacter, sizeof(wchar_t), 1, fp);

		GlyphInfo glyphInfo;
		fread(&glyphInfo, sizeof(GlyphInfo), 1, fp);
		m_mGlyphs[glyphCharacter] = glyphInfo;
	}

	fread(&m_TextureSize, sizeof(unsigned int), 1, fp);
	m_pFontTextureData = new unsigned char[m_TextureSize * m_TextureSize];
	fread(m_pFontTextureData, sizeof(unsigned char), m_TextureSize * m_TextureSize, fp);

	fclose(fp);
}