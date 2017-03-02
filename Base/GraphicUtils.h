#pragma once

#include <vector>
#include <list>

//TODO Make a proper class
struct PackRect
{
	unsigned int x;
	unsigned int y;
	unsigned int size_x;
	unsigned int size_y;

	void* user_data;
};

// @Params
// vRects : std::vector containing rects to pack.  On return, this will contain every rect that was not successfully packed into the texture.
// uiTextureWidth  : Width of the texture to pack into.
// uiTextureHeight : Height of the texture to pack into.
//
// @Return Value 
// std::vector with all the rects that were successfully packed into the texture
std::vector<PackRect> PackTexture(std::vector<PackRect>& vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight);
