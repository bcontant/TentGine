#pragma once

#pragma warning(default : 4062)

#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgiformat.h>
#include <wincodec.h>
#include <stdint.h>

#include <memory>

#include "../Base/Types.h"

#include "../Base/StringUtils.h"
#include "../Base/Logger.h"
#include "../Base/Profiler.h"
#include "../Base/Assert.h"

#include "DXUtils.h"

#include "Renderer_DX11.h"
#include "Texture_DX11.h"
#include "Quad_DX11.h"
#include "Text_DX11.h"
#include "VertexBuffer_DX11.h"

#include "../OS_Base/Window.h"
