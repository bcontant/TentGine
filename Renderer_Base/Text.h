#pragma once

#include "RendererObject.h"
#include "../Base/Rect.h"

class Font;


class Text : public RendererObject
{
public:
	Text(Renderer* pOwner);
	virtual ~Text();

	virtual void Draw() = 0;

	virtual void SetPosition(float in_x, float in_y);
	virtual void SetFont(const Font* in_pFont);
	virtual void SetText(const StdString& in_strText);

protected:
	struct Character
	{
		Rect<int> m_Position;
		Rect<float> m_UVs;
	};
	virtual void UpdateText();

protected:
	const Font* m_pFont = nullptr;

	StdString m_strText = L("");

	float m_xPos;
	float m_yPos;

	std::vector<Character> m_vCharacters;
};
