#pragma once

#include "../Renderer_Base/Shader.h"

class Shader_DX11 : public Shader
{
public:
	Shader_DX11(Renderer* in_pOwner);
	virtual ~Shader_DX11();

	virtual void Initialize(const Path& in_shaderFile, EShaderType in_eType);
	virtual void Bind();

private:
	ID3D11VertexShader* pVertexShader = nullptr;
	ID3D11Buffer* pVSConstantBuffer = nullptr;

	ID3D11PixelShader* pPixelShader = nullptr;
	ID3D11Buffer* pPSConstantBuffer = nullptr;
};