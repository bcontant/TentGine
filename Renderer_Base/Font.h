#pragma once

#include "RendererObject.h"
#include "../Base/StringUtils.h"
#include "../Base/Path.h"

class Texture;
class FontDataFile;

//--------------------------------------------------------------------------------
class Font : public RendererObject
{
public:
	Font(Renderer* pOwner);
	virtual ~Font();

	void Load(const Path& in_filename);

	const FontDataFile* GetFontInfo() const { return m_FontDataFile; }
	const Texture* GetTexture() const { return m_pFontTexture; }

private:
	FontDataFile* m_FontDataFile;
	Texture* m_pFontTexture;
};