#include "precompiled.h"

#include "GraphicUtils.h"
#include <algorithm>

//--------------------------------------------------------------------------------
void PackTextureRecurse(std::vector<PackRect>& vRects, u32 in_uiTextureWidth, u32 in_uiTextureHeight, u32 in_uiPadding, u32 current_x, u32 current_y, bool in_bStaticHeight, bool in_bStaticWidth, std::vector<PackRect>& in_vPackedRects)
{
	AssertMsg(static_cast<s32>(in_uiTextureWidth) >= 0 && static_cast<s32>(in_uiTextureHeight) >= 0, L("Invalid region size, something went wrong with PackTexture()"));

	auto it = std::find_if(vRects.begin(), vRects.end(), [in_uiTextureWidth, in_uiTextureHeight, in_uiPadding](const PackRect& a) { return a.size_x + in_uiPadding <= static_cast<u32>(in_uiTextureWidth) && a.size_y + in_uiPadding <= static_cast<u32>(in_uiTextureHeight); });
	if (it == vRects.end())
		return;

	PackRect texRc = { current_x, current_y, it->size_x, it->size_y, it->user_data };
	in_vPackedRects.push_back(texRc);
	vRects.erase(it);

	u32 paddedSize_x = texRc.size_x + in_uiPadding;
	u32 paddedSize_y = texRc.size_y + in_uiPadding;

	//Filling a row
	if (in_bStaticHeight)
	{
		//Pack underneath
		PackTextureRecurse(vRects, paddedSize_x, in_uiTextureHeight - paddedSize_y, in_uiPadding, current_x, current_y + paddedSize_y, false, true, in_vPackedRects);
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - paddedSize_x, in_uiTextureHeight, in_uiPadding, current_x + paddedSize_x, current_y, true, false, in_vPackedRects);
	}
	//Filling a column
	else if (in_bStaticWidth)
	{
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - paddedSize_x, paddedSize_y, in_uiPadding, current_x + paddedSize_x, current_y, true, false, in_vPackedRects);
		//Pack underneath
		PackTextureRecurse(vRects, in_uiTextureWidth, in_uiTextureHeight - paddedSize_y, in_uiPadding, current_x, current_y + paddedSize_y, false, true, in_vPackedRects);
	}
	//Filling a region
	else
	{
		//Pack rows
		PackTextureRecurse(vRects, in_uiTextureWidth - paddedSize_x, paddedSize_y, in_uiPadding, current_x + paddedSize_x, current_y, true, false, in_vPackedRects);

		//Pack columns
		PackTextureRecurse(vRects, paddedSize_x, in_uiTextureHeight - paddedSize_y, in_uiPadding, current_x, current_y + paddedSize_y, false, true, in_vPackedRects);

		//Pack remaining region
		PackTextureRecurse(vRects, in_uiTextureWidth - paddedSize_x, in_uiTextureHeight - paddedSize_y, in_uiPadding, current_x + paddedSize_x, current_y + paddedSize_y, false, false, in_vPackedRects);
	}
}

//--------------------------------------------------------------------------------
std::vector<PackRect> PackTexture(std::vector<PackRect>& io_vRects, u32 in_uiTextureWidth, u32 in_uiTextureHeight, u32 in_uiPadding)
{
	PROFILE_BLOCK;

	std::vector<PackRect> vPackedRects;

	//Do nothing with 0 size rects and pretend they were successfully packed (it's not false)
	for (auto it = io_vRects.begin(); it != io_vRects.end(); )
	{
		if (it->size_x == 0 || it->size_y == 0)
		{
			vPackedRects.push_back(*it);
			it = io_vRects.erase(it);
		}
		else
		{
			it++;
		}
	}

	
	std::sort(io_vRects.begin(), io_vRects.end(), [](const PackRect& a, const PackRect& b) { return a.size_x + a.size_y > b.size_x + b.size_y;  });
	PackTextureRecurse(io_vRects, in_uiTextureWidth - in_uiPadding, in_uiTextureHeight - in_uiPadding, in_uiPadding, in_uiPadding, in_uiPadding, false, false, vPackedRects);

	return vPackedRects;
}