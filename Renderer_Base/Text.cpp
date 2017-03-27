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
	u32 Color = 0xffffffff;
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
}

//--------------------------------------------------------------------------------
void Text::SetWidth(float in_w)
{
	if (m_width == in_w)
		return;

	m_width = in_w;

	m_InternalState |= eIS_VBDirty | eIS_TextDirty;
}

//--------------------------------------------------------------------------------
void Text::SetHeight(float in_h)
{
	if (m_height == in_h)
		return;

	m_height = in_h;

	m_InternalState |= eIS_VBDirty | eIS_TextDirty;
}

//--------------------------------------------------------------------------------
void Text::SetFont(const Font* in_pFont) 
{ 
	Assert(in_pFont != nullptr);

	if (m_pFont == in_pFont)
		return;

	m_pFont = in_pFont; 

	m_InternalState |= eIS_VBDirty | eIS_TextDirty;
}

//--------------------------------------------------------------------------------
void Text::SetText(const std_string& in_strText) 
{ 
	if (m_strText == in_strText)
		return;

	m_strText = in_strText; 

	m_InternalState |= eIS_VBDirty | eIS_TextDirty;
}

//--------------------------------------------------------------------------------
void Text::SetColor(u32 in_Color)
{
	if (m_Color == in_Color)
		return;

	m_Color = in_Color;

	m_InternalState |= eIS_VBDirty;
}

//--------------------------------------------------------------------------------
/*
void Text::Draw()
{
	Shader* pShader = GetOwner()->GetShader("simple2d_ptc");
	pShader->SetConstant(WVP);

	pShader->Bind(0);
	m_pFont->GetTexture()->Bind(0);
	m_pVertexBuffer->Bind();
		
	m_pVertexBuffer->Draw();
}*/

//--------------------------------------------------------------------------------
void Text::UpdateText()
{
	if (m_pFont == nullptr)
		return;

	PROFILE_BLOCK;
			
	if (m_InternalState & eIS_TextDirty)
	{
		BuildTokens();
		BuildLines();
		BuildCharacters();

		m_InternalState &= ~eIS_TextDirty;
	}

	if (m_InternalState & eIS_VBDirty)
	{
		UpdateVertexBuffer();

		m_InternalState &= ~eIS_VBDirty;
	}
}

//--------------------------------------------------------------------------------
void Text::BuildTokens()
{
	const FontDataFile* pFontInfo = m_pFont->GetFontInfo();

	//Break up the strings into indivisible segments for line breaking purposes
	m_vTokens.clear();

	TextToken currentToken;
	currentToken.eType = eTT_Invalid;
	currentToken.text = L("");
	currentToken.width = 0.f;

	for (ustring_char c : m_strText)
	{
		const FontDataFile::GlyphInfo* pCharInfo = pFontInfo->GetGlyphInfo(c);
		if (pCharInfo == nullptr)
		{
			Assert(false);
			continue;
		}

		if ((c == '\n' || c == ' '))
		{
			if (currentToken.text != L(""))
				m_vTokens.push_back(currentToken);

			if (c == '\n')
				m_vTokens.push_back({ eTT_Linebreak, L(""), 0.f });
			if (c == ' ')
				m_vTokens.push_back({ eTT_Space, L(" "), 0.f + pCharInfo->advance });

			currentToken.eType = eTT_Invalid;
			currentToken.text = L("");
			currentToken.width = 0.f;
			continue;
		}

		//Special case.  Break in middle of a string because it's wider than the region.
		if (currentToken.width + pCharInfo->advance > m_width)
		{
			m_vTokens.push_back(currentToken);
			currentToken.eType = eTT_Invalid;
			currentToken.text = L("");
			currentToken.width = 0.f;
		}

		currentToken.eType = eTT_Text;
		currentToken.text += c;
		currentToken.width += pCharInfo->advance;
	}

	if (currentToken.eType != eTT_Invalid)
		m_vTokens.push_back(currentToken);
}

//--------------------------------------------------------------------------------
void Text::BuildLines()
{
	const FontDataFile* pFontInfo = m_pFont->GetFontInfo();

	m_vLines.clear();
	TextLine currentLine;

	s32 current_x = 0;
	s32 current_y = 0;
	current_y += pFontInfo->GetBaseLineOffset();

	// Assemble the tokens into lines
	for (TextToken seg : m_vTokens)
	{
		//Go to next line outside our zone's limits
		if (current_x + seg.width > m_width)
		{
			current_x = 0;
			current_y += pFontInfo->GetLineHeight();

			m_vLines.push_back(currentLine);
			currentLine.vTokens.clear();
		}

		if (seg.eType == eTT_Linebreak)
		{
			current_x = 0;
			current_y += pFontInfo->GetLineHeight();

			m_vLines.push_back(currentLine);
			currentLine.vTokens.clear();

			continue;
		}

		//Break out if we are outside our zone's limits
		if (current_y > m_height)
		{
			break;
		}

		if (current_x == 0 && seg.eType == eTT_Space)
		{
			continue;
		}

		current_x += (s32)seg.width;
		currentLine.vTokens.push_back(seg);
	}

	if (currentLine.vTokens.size() > 0)
		m_vLines.push_back(currentLine);
}

//--------------------------------------------------------------------------------
void Text::BuildCharacters()
{
	const FontDataFile* pFontInfo = m_pFont->GetFontInfo();

	m_vCharacters.clear();

	// Fill up the final character positions with the lines we created
	s32 current_x = 0;
	s32 current_y = 0;
	current_y += pFontInfo->GetBaseLineOffset();

	for (TextLine line : m_vLines)
	{
		for (TextToken token : line.vTokens)
		{
			for (ustring_char c : token.text)
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
				if (newChar.m_Position.Width() > 0 && newChar.m_Position.Height() > 0)
					m_vCharacters.push_back(newChar);

				current_x += pCharInfo->advance;
			}
		}

		current_x = 0;
		current_y += pFontInfo->GetLineHeight();
	}
}

//--------------------------------------------------------------------------------
void Text::UpdateVertexBuffer()
{
	if (m_pVertexBuffer == nullptr)
	{
		m_pVertexBuffer = GetOwner()->CreateVertexBuffer(static_cast<u32>(m_vCharacters.size() * 6), VertexBuffer::eVB_Position | VertexBuffer::eVB_2D_TexCoords | VertexBuffer::eVB_Color, EPrimitiveType::ePT_TriangleList, nullptr, eUT_CPU_Writable);
	}
	else if (m_pVertexBuffer->GetVertexCount() != m_vCharacters.size() * 6)
	{
		m_pVertexBuffer->SetVertexCount(static_cast<u32>(m_vCharacters.size() * 6));
	}

	if (m_vCharacters.size() == 0)
		return;

	SimpleVertex* pVBData = new SimpleVertex[6 * static_cast<u32>(m_vCharacters.size())];

	for (u32 i = 0; i < m_vCharacters.size(); i++)
	{
		Character& currentChar = m_vCharacters[i];
		SimpleVertex* pVB = &pVBData[i * 6];

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB->Color = m_Color; pVB++;

		//Top Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_top; pVB->Color = m_Color; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB->Color = m_Color; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB->Color = m_Color; pVB++;

		//Bottom Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB->Color = m_Color; pVB++;

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB->Color = m_Color; pVB++;
	}

	void* pVB = m_pVertexBuffer->Lock();
	memcpy(pVB, pVBData, 6 * sizeof(SimpleVertex) * static_cast<u32>(m_vCharacters.size()));
	m_pVertexBuffer->Unlock();

	delete[] pVBData;
}