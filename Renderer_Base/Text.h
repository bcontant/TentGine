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
	virtual void SetFont(const Font* in_pFont);
	virtual void SetText(const std_string& in_strText);

protected:
	struct Character
	{
		Rect<s32> m_Position;
		Rect<float> m_UVs;
	};
	virtual void UpdateText();

protected:
	const Font* m_pFont = nullptr;

	std_string m_strText = L("");

	float m_xPos;
	float m_yPos;

	std::vector<Character> m_vCharacters;

	VertexBuffer* m_pVertexBuffer = nullptr;
};
