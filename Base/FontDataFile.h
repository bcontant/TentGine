#pragma once

#include "StringUtils.h"
#include "Path.h"

#include "../OS_Base/FontBuilder.h"

class BitmapData;

class FontDataFile
{
	friend bool OS::BuildFont(const Path&, unsigned int, unsigned int, unsigned int, const Path&);

public:
	struct GlyphInfo
	{
		int left;
		int advance;
		int top;

		float boxX;
		float boxY;
		float boxWidth;
		float boxHeight;
	};

	FontDataFile();
	virtual ~FontDataFile();

	void Load(const Path& in_filename);
	void Save(const Path& in_filename) const;

	const StdString& GetFontName() const { return m_FontName; }
	unsigned int GetFontSize() const { return m_FontSize; }

	const BitmapData* const GetTextureData() const { return m_pFontTextureData; }
		
private:
	StdString m_FontName;
	unsigned int m_FontSize;

	std::vector<FontDataFile::GlyphInfo*> m_vGlyphs;
	std::map<wchar_t, FontDataFile::GlyphInfo*> m_mCharacterMap;

	Path m_FontTextureFilename;
	BitmapData* m_pFontTextureData;
};