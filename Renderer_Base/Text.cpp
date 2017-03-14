#include "precompiled.h"

#include "Text.h"
#include "Renderer.h"
#include "../Base/FontDataFile.h"
#include "../Base/Profiler.h"

//--------------------------------------------------------------------------------
Text::Text(Renderer* pOwner) : RendererObject(pOwner) 
{

}

//--------------------------------------------------------------------------------
Text::~Text() 
{

}

//--------------------------------------------------------------------------------
void Text::SetPosition(float in_x, float in_y) 
{
	m_xPos = in_x;
	m_yPos = in_y;

	UpdateText();
}

//--------------------------------------------------------------------------------
void Text::SetFont(const Font* in_pFont) 
{ 
	Assert(in_pFont != NULL);

	if (m_pFont == in_pFont)
		return;

	m_pFont = in_pFont; 
	UpdateText();
}

//--------------------------------------------------------------------------------
void Text::SetText(const StdString& in_strText) 
{ 
	m_strText = in_strText; 
	UpdateText();
}

//--------------------------------------------------------------------------------
void Text::UpdateText()
{
	if (m_pFont == nullptr)
		return;

	PROFILE_BLOCK;

	m_vCharacters.clear();

	const FontDataFile* pFontInfo = m_pFont->GetFontInfo();

	int current_x = 0;
	int current_y = 0;
	current_y += pFontInfo->GetBaseLineOffset();

	for (UStringChar c : m_strText)
	{
		const FontDataFile::GlyphInfo* pCharInfo = pFontInfo->GetGlyphInfo(c);
		if (pCharInfo == nullptr)
		{
			Assert(false);
			continue;
		}

		Character newChar;
		newChar.m_Position.m_left = current_x + pCharInfo->m_GlyphBox.m_left;
		newChar.m_Position.m_right = current_x + pCharInfo->m_GlyphBox.m_right;
		newChar.m_Position.m_top = current_y - pCharInfo->m_GlyphBox.m_top;
		newChar.m_Position.m_bottom = current_y - pCharInfo->m_GlyphBox.m_bottom;

		newChar.m_UVs = pCharInfo->m_UVs;
		
		//Some glyph have no size and are only useful for their advance (space)
		if(newChar.m_Position.Width() > 0 && newChar.m_Position.Height() > 0)
			m_vCharacters.push_back(newChar);

		current_x += pCharInfo->advance;
	}
}
