#include "precompiled.h"

#include "GraphicUtils.h"
#include <algorithm>

void PackTextureRecurse(std::vector<PackRect>& vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight, unsigned int current_x, unsigned int current_y, bool in_bStaticHeight, bool in_bStaticWidth, std::vector<PackRect>& in_vPackedRects)
{
	auto it = std::find_if(vRects.begin(), vRects.end(), [in_uiTextureWidth, in_uiTextureHeight](const PackRect& a) { return a.size_x <= in_uiTextureWidth && a.size_y <= in_uiTextureHeight; });
	if (it == vRects.end())
		return;

	PackRect texRc = { current_x, current_y, it->size_x, it->size_y, it->user_data };
	in_vPackedRects.push_back(texRc);
	vRects.erase(it);

	//Filling a row
	if (in_bStaticHeight)
	{
		//Pack underneath
		PackTextureRecurse(vRects, texRc.size_x, in_uiTextureHeight - texRc.size_y, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, in_uiTextureHeight, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);
	}
	//Filling a column
	else if (in_bStaticWidth)
	{
		//Pack right
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, texRc.size_y, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);
		//Pack underneath
		PackTextureRecurse(vRects, in_uiTextureWidth, in_uiTextureHeight - texRc.size_y, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);
	}
	//Filling a region
	else
	{
		//Pack rows
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, texRc.size_y, current_x + texRc.size_x, current_y, true, false, in_vPackedRects);

		//Pack columns
		PackTextureRecurse(vRects, texRc.size_x, in_uiTextureHeight - texRc.size_y, current_x, current_y + texRc.size_y, false, true, in_vPackedRects);

		//Pack remaining region
		PackTextureRecurse(vRects, in_uiTextureWidth - texRc.size_x, in_uiTextureHeight - texRc.size_y, current_x + texRc.size_x, current_y + texRc.size_y, false, false, in_vPackedRects);
	}
}

std::vector<PackRect> PackTexture(std::vector<PackRect>& vRects, unsigned int in_uiTextureWidth, unsigned int in_uiTextureHeight)
{
	PROFILE_BLOCK;

	std::vector<PackRect> vPackedRects;

	std::sort(vRects.begin(), vRects.end(), [](const PackRect& a, const PackRect& b) { return a.size_x + a.size_y > b.size_x + b.size_y;  });
	PackTextureRecurse(vRects, in_uiTextureWidth, in_uiTextureHeight, 0, 0, false, false, vPackedRects);

	return vPackedRects;
}