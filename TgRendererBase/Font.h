#pragma once

#include "RendererObject.h"
#include <string>

class Texture;
class FontDataFile;

class Font : public RendererObject
{
public:
	Font(Renderer* pOwner);
	virtual ~Font();

	void Load(std::wstring in_filename);

private:
	FontDataFile* m_FontDataFile;
	Texture* m_pFontTexture;
};