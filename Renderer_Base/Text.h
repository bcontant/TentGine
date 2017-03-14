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
		//position (pixels)
		/*int left;
		int right;
		int top;
		int bottom;

		//texcoords (0..1)
		float left_uv;
		float right_uv;
		float top_uv;
		float bottom_uv;*/

		Rect<int> m_Position;
		Rect<float> m_UVs;
	};
	virtual void UpdateText();

protected:
	const Font* m_pFont = nullptr;

	StdString m_strText = "";

	float m_xPos;
	float m_yPos;

	std::vector<Character> m_vCharacters;
};
