#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgiformat.h>
#include <wincodec.h>
#include <stdint.h>

#include <memory>

#include "Renderer_DX11.h"
#include "Texture_DX11.h"
#include "Quad_DX11.h"
#include "WICTextureLoader.h"

#include "../Base/StringUtils.h"
#include "../Base/Logger.h"

#include "../OS_Base/Window.h"
