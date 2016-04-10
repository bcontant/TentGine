#include "precompiled.h"

#include "FontDataFile.h"

FontDataFile::FontDataFile()
	: m_FontSize(0.f)
	, m_TextureSize(0)
	, m_pFontTextureData(NULL)
{
}

FontDataFile::FontDataFile(const StdString& in_FontName, float in_FontSize, unsigned int in_TextureSize, std::map<wchar_t, GlyphInfo> in_mGlyphs, const unsigned char* in_pData)
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

void FontDataFile::Save(const Path& in_filename) const
{
	File fontFile;
	fontFile.Open(in_filename, File::fmWriteOnly);

	fontFile.Write(m_FontName);
	fontFile.Write(&m_FontSize, sizeof(float));

	int glyphCount = (int)m_mGlyphs.size();
	fontFile.Write(&glyphCount, sizeof(unsigned int));

	auto it = m_mGlyphs.cbegin();
	for (it; it != m_mGlyphs.cend(); ++it)
	{
		fontFile.Write(&it->first, sizeof(wchar_t));
		fontFile.Write(&it->second, sizeof(GlyphInfo));
	}

	fontFile.Write(&m_TextureSize, sizeof(unsigned int));
	fontFile.Write(m_pFontTextureData, sizeof(unsigned char) * m_TextureSize * m_TextureSize);
	fontFile.Close();
}

void FontDataFile::Load(const Path& in_filename)
{
	delete[] m_pFontTextureData;
	m_pFontTextureData = NULL;

	File fontFile;
	fontFile.Open(in_filename, File::fmReadOnly);

	fontFile.Read(m_FontName);
	fontFile.Read(&m_FontSize, sizeof(float));

	unsigned int glyphCount;
	fontFile.Read(&glyphCount, sizeof(unsigned int));

	for (unsigned int i = 0; i < glyphCount; i++)
	{
		wchar_t glyphCharacter;
		fontFile.Read(&glyphCharacter, sizeof(wchar_t));

		GlyphInfo glyphInfo;
		fontFile.Read(&glyphInfo, sizeof(GlyphInfo));
		m_mGlyphs[glyphCharacter] = glyphInfo;
	}

	fontFile.Read(&m_TextureSize, sizeof(unsigned int));
	m_pFontTextureData = new unsigned char[m_TextureSize * m_TextureSize];
	fontFile.Read(m_pFontTextureData, sizeof(unsigned char) * m_TextureSize * m_TextureSize);

	fontFile.Close();
}