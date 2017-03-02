#pragma once

#include "StringUtils.h"
#include "Path.h"
#include <map>

class FontDataFile
{
public:
	struct GlyphInfo
	{
		float lsb;
		float advance;
		float top;

		float boxX;
		float boxY;
		float boxWidth;
		float boxHeight;
	};

	FontDataFile();
	FontDataFile(const StdString& in_FontName, float in_FontSize, unsigned int in_TextureSize, std::map<wchar_t, GlyphInfo> in_mGlyphs, const unsigned char* in_pData);

	virtual ~FontDataFile();

	void Load(const Path& in_filename);
	void Save(const Path& in_filename) const;

	const StdString& GetFontName() const { return m_FontName; }
	float GetFontSize() const { return m_FontSize; }

	unsigned int GetTextureSize() const { return m_TextureSize; }
	const unsigned char* const GetTextureData() const { return m_pFontTextureData; }
		
private:
	StdString m_FontName;
	float m_FontSize;

	std::map<wchar_t, GlyphInfo> m_mGlyphs;

	int m_TextureSize;
	unsigned char* m_pFontTextureData;
};