#include "precompiled.h"

#include "FontDataFile.h"
#include "BitmapData.h"
#include "PNGFile.h"

//--------------------------------------------------------------------------------
FontDataFile::FontDataFile()
	: m_FontSize(0)
	, m_pFontTextureData(nullptr)
{
}

//--------------------------------------------------------------------------------
FontDataFile::~FontDataFile()
{
	delete m_pFontTextureData;
	m_pFontTextureData = nullptr;
}

//--------------------------------------------------------------------------------
void FontDataFile::Save(const Path& in_filename) const
{
	PROFILE_BLOCK;

	File fontFile;
	if (!fontFile.Open(in_filename, FileMode::WriteOnly))
		return;

	fontFile.Write(m_FontName);
	fontFile.Write(&m_FontSize, sizeof(float));

	fontFile.Write(m_FontTextureFilename.GetStdString());

	int glyphCount = (int)m_vGlyphs.size();
	fontFile.Write(&glyphCount, sizeof(unsigned int));

	for (unsigned int i = 0; i < m_vGlyphs.size(); i++)
	{
		fontFile.Write(m_vGlyphs[i], sizeof(GlyphInfo));
	}

	int characterCount = (int)m_mCharacterMap.size();
	fontFile.Write(&characterCount, sizeof(unsigned int));
	
	for (auto it = m_mCharacterMap.cbegin(); it != m_mCharacterMap.cend(); it++)
	{
		auto glyphIt = std::find(m_vGlyphs.cbegin(), m_vGlyphs.cend(), it->second);
		Assert(glyphIt != m_vGlyphs.cend());

		size_t glyphIndex = glyphIt - m_vGlyphs.cbegin();

		fontFile.Write(&it->first, sizeof(wchar_t));
		fontFile.Write(&glyphIndex, sizeof(size_t));
	}

	fontFile.Close();

	SavePNG(m_FontTextureFilename, m_pFontTextureData);
}

//--------------------------------------------------------------------------------
void FontDataFile::Load(const Path& in_filename)
{
	delete[] m_pFontTextureData;
	m_pFontTextureData = nullptr;

	File fontFile;
	if (!fontFile.Open(in_filename, FileMode::ReadOnly))
		return;

	fontFile.Read(m_FontName);
	fontFile.Read(&m_FontSize, sizeof(float));

	StdString filename;
	fontFile.Read(filename);
	m_FontTextureFilename = filename;

	unsigned int glyphCount;
	fontFile.Read(&glyphCount, sizeof(unsigned int));

	for (unsigned int i = 0; i < glyphCount; i++)
	{
		GlyphInfo* newInfo = new GlyphInfo;
		fontFile.Read(newInfo, sizeof(GlyphInfo));

		m_vGlyphs.push_back(newInfo);
	}

	unsigned int characterCount;
	fontFile.Read(&characterCount, sizeof(unsigned int));

	for (unsigned int i = 0; i < characterCount; i++)
	{
		wchar_t characterCode;
		fontFile.Read(&characterCode, sizeof(wchar_t));

		size_t glyphIndex;
		fontFile.Read(&glyphIndex, sizeof(size_t));

		Assert(glyphIndex < m_vGlyphs.size());

		m_mCharacterMap[characterCode] = m_vGlyphs[glyphIndex];
	}

	m_pFontTextureData = LoadPNG(m_FontTextureFilename);

	fontFile.Close();
}