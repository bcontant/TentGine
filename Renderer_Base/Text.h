#pragma once

#include "RendererObject.h"
#include "../Base/Rect.h"

class Font;
class VertexBuffer;

class Text : public RendererObject
{
public:
	Text(Renderer* pOwner);
	virtual ~Text();

	virtual void Draw() = 0;

	virtual void SetPosition(float in_x, float in_y);
	virtual void SetWidth(float in_w);
	virtual void SetHeight(float in_h);
	virtual void SetFont(const Font* in_pFont);
	virtual void SetText(const std_string& in_strText);
	virtual void SetColor(u32 in_Color);

protected:
	virtual void UpdateText();

private:
	enum ETokenType
	{
		eTT_Text,
		eTT_Space,
		eTT_Linebreak,
		eTT_Invalid
	};

	struct TextToken
	{
		ETokenType eType;
		std_string text;
		float width;
	};

	struct TextLine
	{
		std::vector<TextToken> vTokens;
	};

	struct Character
	{
		Rect<s32> m_Position;
		Rect<float> m_UVs;
	};


	void BuildTokens();
	void BuildLines();
	void BuildCharacters();
	void UpdateVertexBuffer();

protected:
	const Font* m_pFont = nullptr;
	VertexBuffer* m_pVertexBuffer = nullptr;

	float m_xPos = 0.f;
	float m_yPos = 0.f;

private:
	enum EInternalState
	{
		eIS_TextDirty =  1 << 0,
		eIS_VBDirty = 1 << 1
	};
	u32 m_InternalState = 0;

	u32 m_Color = 0xff000000;

	float m_width = FLT_MAX;
	float m_height = FLT_MAX;

	std_string m_strText = L("");
	std::vector<TextToken> m_vTokens;
	std::vector<TextLine> m_vLines;
	std::vector<Character> m_vCharacters;
};
