#pragma once

#include <string>
#include <map>

class FontDataFile
{
public:
	struct GlyphInfo
	{
		float lsb;
		float advance;
		float rsb;

		float boxX;
		float boxY;
		float boxWidth;
		float boxHeight;
	};

	FontDataFile() {}
	FontDataFile(const std::wstring& in_FontName, float in_FontSize, unsigned int in_TextureSize, std::map<wchar_t, GlyphInfo> in_mGlyphs, const unsigned char* in_pData);

	virtual ~FontDataFile();

	void Load(std::wstring in_filename);
	void Save(std::wstring in_filename) const;

	const std::wstring& GetFontName() const { return m_FontName; }
	float GetFontSize() const { return m_FontSize; }

	unsigned int GetTextureSize() const { return m_TextureSize; }
	const unsigned char* const GetTextureData() const { return m_pFontTextureData; }
		
private:
	std::wstring m_FontName;
	float m_FontSize;

	std::map<wchar_t, GlyphInfo> m_mGlyphs;

	int m_TextureSize;
	unsigned char* m_pFontTextureData;
};