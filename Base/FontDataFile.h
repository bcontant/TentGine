#pragma once

#include "StringUtils.h"
#include "Path.h"

#include "../OS_Base/FontBuilder.h"
#include "Rect.h"

class BitmapData;

class FontDataFile
{
	friend bool OS::BuildFont(const Path&, u32, u32, u32, const Path&);

public:
	struct GlyphInfo
	{
		Rect<s32> m_GlyphBox;
		s32 advance;

		Rect<float> m_UVs;
	};

	FontDataFile();
	virtual ~FontDataFile();

	void Load(const Path& in_filename);
	void Save(const Path& in_filename) const;

	const std_string& GetFontName() const { return m_FontName; }
	u32 GetFontSize() const { return m_FontSize; }

	u32 GetLineHeight() const { return m_LineHeight; }
	u32 GetBaseLineOffset() const { return m_MaxAscender; }

	const FontDataFile::GlyphInfo* GetGlyphInfo(wchar_t in_cCharCode) const;

	const BitmapData* const GetTextureData() const { return m_pFontTextureData; }
		
private:
	std_string m_FontName;
	u32 m_FontSize;

	//Distance (pixels) between the baseline of two lines
	u32 m_LineHeight;

	//Distance (pixels) from the baseline to the top of the highest glyph
	u32 m_MaxAscender;

	std::vector<FontDataFile::GlyphInfo*> m_vGlyphs;
	std::map<wchar_t, FontDataFile::GlyphInfo*> m_mCharacterMap;

	Path m_FontTextureFilename;
	BitmapData* m_pFontTextureData;
};