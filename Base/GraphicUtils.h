#pragma once

#include <vector>
#include <list>

//TODO Make a proper class
struct PackRect
{
	u32 x;
	u32 y;
	u32 size_x;
	u32 size_y;

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
std::vector<PackRect> PackTexture(std::vector<PackRect>& io_vRects, u32 in_uiTextureWidth, u32 in_uiTextureHeight, u32 in_uiPadding);
