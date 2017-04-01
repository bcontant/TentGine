#include "precompiled.h"

#include "Shader.h"

ShaderConstant::~ShaderConstant()
{
	delete[] m_Data;
}

Shader::Shader(Renderer* in_pOwner)
	: RendererObject(in_pOwner)
{
}

bool Shader::SetConstant(const std_string& in_ConstantName, void* in_pConstantValue)
{
	for (auto constant : m_vConstants)
	{
		if (constant.m_Name == in_ConstantName)
		{
			memcpy(constant.m_Data, in_pConstantValue, constant.m_DataSize);
			return true;
		}
	}
	return false;
}

void Shader::RegisterConstant(const std_string& in_ConstantName, u32 in_DataSize)
{
	ShaderConstant newConstant;
	newConstant.m_Name = in_ConstantName;
	newConstant.m_DataSize = in_DataSize;
	newConstant.m_Data = new u8[in_DataSize];

	m_vConstants.push_back(newConstant);
}

void Shader::Initialize(const Path& in_shaderFile, EShaderType in_eType)
{
	m_eType = in_eType;
	m_Name = in_shaderFile.GetName().GetData();
}
