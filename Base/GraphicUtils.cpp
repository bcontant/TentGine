#include "precompiled.h"

#include "GraphicUtils.h"
#include <algorithm>

void PackTextureRecurse(std::vector<PackRect>& vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight, unsigned int in_uiPadding, unsigned int current_x, unsigned int current_y, bool in_bStaticHeight, bool in_bStaticWidth, std::vector<PackRect>& in_vPackedRects)
{
	AssertMsg(static_cast<int>(in_uiTextureWidth) >= 0 && static_cast<int>(in_uiTextureHeight) >= 0, L("Invalid region size, something went wrong with PackTexture()"));

	auto it = std::find_if(vRects.begin(), vRects.end(), [in_uiTextureWidth, in_uiTextureHeight, in_uiPadding](const PackRect& a) { return a.size_x + in_uiPadding <= static_cast<unsigned int>(in_uiTextureWidth) && a.size_y + in_uiPadding <= static_cast<unsigned int>(in_uiTextureHeight); });
	if (it == vRects.end())
		return;

	PackRect texRc = { current_x, current_y, it->size_x + in_uiPadding, it->size_y + in_uiPadding, it->user_data };
	in_vPackedRects.push_back(texRc);
	vRects.erase(it);

	//Filling a row
	if (in_bStaticHeight)
	{
		//Pack underneath
		PackTextureRecurse(vRects, texRc.size_x, in_uiTextureHeight - texRc.size_y, in_uiPadding, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - (texRc.size_x), in_uiTextureHeight, in_uiPadding, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);
	}
	//Filling a column
	else if (in_bStaticWidth)
	{
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, texRc.size_y, in_uiPadding, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);
		//Pack underneath
		PackTextureRecurse(vRects, in_uiTextureWidth, in_uiTextureHeight - texRc.size_y, in_uiPadding, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);
	}
	//Filling a region
	else
	{
		//Pack rows
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, texRc.size_y, in_uiPadding, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);

		//Pack columns
		PackTextureRecurse(vRects, texRc.size_x, in_uiTextureHeight - texRc.size_y, in_uiPadding, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);

		//Pack remaining region
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, in_uiTextureHeight - texRc.size_y, in_uiPadding, current_x + texRc.size_x, current_y + texRc.size_y, false, false, in_vPackedRects);
	}
}

std::vector<PackRect> PackTexture(std::vector<PackRect>& io_vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight, unsigned int in_uiPadding)
{
	PROFILE_BLOCK;

	AssertMsg(in_uiPadding % 2 == 0, L("in_uiPadding should be dividable by 2"));
	
	std::vector<PackRect> vPackedRects;

	std::sort(io_vRects.begin(), io_vRects.end(), [](const PackRect& a, const PackRect& b) { return a.size_x + a.size_y > b.size_x + b.size_y;  });
	PackTextureRecurse(io_vRects, in_uiTextureWidth, in_uiTextureHeight, in_uiPadding, in_uiPadding / 2, in_uiPadding / 2, false, false, vPackedRects);

	return vPackedRects;
}