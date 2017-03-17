#include "precompiled.h"

#include "Text.h"
#include "Renderer.h"
#include "../Base/FontDataFile.h"
#include "../Base/Profiler.h"

#include "VertexBuffer.h"

//--------------------------------------------------------------------------------
struct SimpleVertex
{
	vec3 Pos;
	vec2 UV;
};

//--------------------------------------------------------------------------------
Text::Text(Renderer* pOwner) 
	: RendererObject(pOwner) 
	, m_pFont(nullptr)
	, m_strText(L(""))
	, m_xPos(0.f)
	, m_yPos(0.f)
	, m_pVertexBuffer(nullptr)
{
}

//--------------------------------------------------------------------------------
Text::~Text() 
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;
}

//--------------------------------------------------------------------------------
void Text::SetPosition(float in_x, float in_y) 
{
	if (m_xPos == in_x && m_yPos == in_y)
		return;

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
void Text::SetText(const std_string& in_strText) 
{ 
	if (m_strText == in_strText)
		return;

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

	s32 current_x = 0;
	s32 current_y = 0;
	current_y += pFontInfo->GetBaseLineOffset();

	for (ustring_char c : m_strText)
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

	if (m_pVertexBuffer == nullptr)
	{
		m_pVertexBuffer = GetOwner()->CreateVertexBuffer(static_cast<u32>(m_vCharacters.size() * 6), VertexBuffer::eVB_Position | VertexBuffer::eVB_2D_TexCoords, EPrimitiveType::ePT_TriangleList, nullptr, eUT_CPU_Writable);
	}
	else if (m_pVertexBuffer->GetVertexCount() != m_vCharacters.size() * 6)
	{
		m_pVertexBuffer->SetVertexCount(static_cast<u32>(m_vCharacters.size() * 6));
	}

	if (m_vCharacters.size() == 0)
		return;

	//move to base class
	SimpleVertex* pVBData = new SimpleVertex[6 * static_cast<u32>(m_vCharacters.size())];

	for (u32 i = 0; i < m_vCharacters.size(); i++)
	{
		Character& currentChar = m_vCharacters[i];
		SimpleVertex* pVB = &pVBData[i * 6];

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;

		//Top Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Bottom Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;
	}

	void* pVB = m_pVertexBuffer->Lock();
	memcpy(pVB, pVBData, 6 * sizeof(SimpleVertex) * static_cast<u32>(m_vCharacters.size()));
	m_pVertexBuffer->Unlock();

	delete[] pVBData;
}
