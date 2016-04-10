#include "precompiled.h"

Font::Font(Renderer* pOwner) 
:RendererObject(pOwner)
,m_FontDataFile(NULL)
,m_pFontTexture(NULL) 
{
}

Font::~Font()
{
	delete m_FontDataFile;
	m_FontDataFile = NULL;

	delete m_pFontTexture;
	m_pFontTexture = NULL;
}

void Font::Load(const Path& in_filename)
{
	m_FontDataFile = new FontDataFile();
	m_FontDataFile->Load(in_filename);
	m_pFontTexture = GetOwner()->CreateTexture(m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureData());
}