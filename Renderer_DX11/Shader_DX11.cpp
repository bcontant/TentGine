#include "precompiled.h"

#include "Shader_DX11.h"

Shader_DX11::Shader_DX11(Renderer* in_pOwner)
	: Shader(in_pOwner)
{
	
}

Shader_DX11::~Shader_DX11()
{
	SAFE_RELEASE(pVertexShader);
	SAFE_RELEASE(pVSConstantBuffer);
	SAFE_RELEASE(pPixelShader);
	SAFE_RELEASE(pPSConstantBuffer);
}

void Shader_DX11::Initialize(const Path& in_shaderFile, EShaderType in_eType)
{
	Shader::Initialize(in_shaderFile, in_eType);

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	File shaderFile;

	shaderFile.Open(in_shaderFile, FileMode::ReadOnly);
	u8* shaderData = new u8[shaderFile.Size()];
	shaderFile.Read(shaderData, shaderFile.Size());

	if (in_shaderFile.GetFileName() == L("simple_vertex.cso"))
	{
		HResult hr = pDX11Renderer->GetDevice()->CreateVertexShader(shaderData, shaderFile.Size(), nullptr, &pVertexShader);
		SET_D3D11_OBJECT_NAME(pVertexShader);
	}

}

void Shader_DX11::Bind()
{

}