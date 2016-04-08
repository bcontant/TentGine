#include "Font.h"
#include "Renderer.h"
#include "Texture.h"

#include "../Base/FontDataFile.h"

Font::Font(Renderer* pOwner) 
:RendererObject(pOwner)
,m_FontDataFile(NULL)
,m_pFontTexture(NULL) 
{
}

Font::~Font()
{
	delete m_FontDataFile;
	delete m_pFontTexture;
}

void Font::Load(std::wstring in_filename)
{
	m_FontDataFile = new FontDataFile();
	m_FontDataFile->Load(in_filename);
	m_pFontTexture = GetOwner()->CreateTexture(m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureData());
}