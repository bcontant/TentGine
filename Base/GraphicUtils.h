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
// io_vRects : std::vector containing rects to pack.  On return, this will contain every rect that was not successfully packed into the texture.
// in_uiTextureWidth  : Width of the texture to pack into.
// in_uiTextureHeight : Height of the texture to pack into.
// in_uiPadding : Number of pixels inserted between rects
//
// @Return Value 
// std::vector with all the rects that were successfully packed into the texture
std::vector<PackRect> PackTexture(std::vector<PackRect>& io_vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight, unsigned int in_uiPadding);
