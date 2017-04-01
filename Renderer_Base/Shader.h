#pragma once

#include "RendererObject.h"

enum class EShaderType
{
	Vertex,
	Pixel,
	Invalid
};

class ShaderConstant
{
public:
	virtual ~ShaderConstant();

	std_string m_Name = L("");
	std_string m_Type = L("");

	void* m_Data = nullptr;
	u32 m_DataSize = 0;
};

class ShaderCode
{
public:
	std_string m_Type;
	std_string m_Code;
};

class Shader : public RendererObject
{
public:
	Shader() : RendererObject(nullptr) {};
	Shader(Renderer* in_pOwner);
	virtual ~Shader() {}

	bool SetConstant(const std_string& in_ConstantName, void* in_ConstantValue);
	void RegisterConstant(const std_string& in_ConstantName, u32 in_DataSize);

	virtual void Initialize(const Path& in_shaderFile, EShaderType in_eType);
	virtual void Bind() {};

protected:
	std_string m_Name = L("");
	EShaderType m_eType = EShaderType::Invalid;
	Path m_HLSLCompiledFile = L("");

	ShaderCode m_ShaderCode;

	std::vector<ShaderConstant> m_vConstants;
};
