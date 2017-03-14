#pragma once

#include "StringUtils.h"
#include "Path.h"

#include "../OS_Base/FontBuilder.h"
#include "Rect.h"

class BitmapData;

class FontDataFile
{
	friend bool OS::BuildFont(const Path&, unsigned int, unsigned int, unsigned int, const Path&);

public:
	struct GlyphInfo
	{
		Rect<int> m_GlyphBox;
		int advance;

		Rect<float> m_UVs;
		/*int left;
		int right;
		int top;
		int bottom;
		int advance;
		
		float boxX;
		float boxY;
		float boxWidth;
		float boxHeight;*/
	};

	FontDataFile();
	virtual ~FontDataFile();

	void Load(const Path& in_filename);
	void Save(const Path& in_filename) const;

	const StdString& GetFontName() const { return m_FontName; }
	unsigned int GetFontSize() const { return m_FontSize; }

	unsigned int GetLineHeight() const { return m_LineHeight; }
	unsigned int GetBaseLineOffset() const { return m_MaxAscender; }

	const FontDataFile::GlyphInfo* GetGlyphInfo(wchar_t in_cCharCode) const;

	const BitmapData* const GetTextureData() const { return m_pFontTextureData; }
		
private:
	StdString m_FontName;
	unsigned int m_FontSize;

	//Distance (pixels) between the baseline of two lines
	unsigned int m_LineHeight;

	//Distance (pixels) from the baseline to the top of the highest glyph
	unsigned int m_MaxAscender;

	std::vector<FontDataFile::GlyphInfo*> m_vGlyphs;
	std::map<wchar_t, FontDataFile::GlyphInfo*> m_mCharacterMap;

	Path m_FontTextureFilename;
	BitmapData* m_pFontTextureData;
};