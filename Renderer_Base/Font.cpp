#include "precompiled.h"

//--------------------------------------------------------------------------------
Font::Font(Renderer* pOwner) 
	:RendererObject(pOwner)
	, m_FontDataFile(nullptr)
	, m_pFontTexture(nullptr)
{
}

//--------------------------------------------------------------------------------
Font::~Font()
{
	delete m_FontDataFile;
	m_FontDataFile = nullptr;

	delete m_pFontTexture;
	m_pFontTexture = nullptr;
}

//--------------------------------------------------------------------------------
void Font::Load(const Path& in_filename)
{
	m_FontDataFile = new FontDataFile();
	m_FontDataFile->Load(in_filename);
	m_pFontTexture = GetOwner()->CreateTexture(m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureSize(), m_FontDataFile->GetTextureData());
}