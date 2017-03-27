#pragma once

#include "RendererObject.h"
#include "../Base/Streamable.h"

enum class EShaderType
{
	Vertex,
	Pixel,
	Invalid
};

extern EnumStrings kShaderTypeStrings;

class ShaderConstant
{
public:
	virtual ~ShaderConstant();

	std_string m_Name = L("");
	std_string m_Type = L("");

	void* m_Data = nullptr;
	u32 m_DataSize = 0;

	DECLARE_PROPERTIES_BEGIN(ShaderConstant)
		ADD_PROPERTY(m_Name),
		ADD_PROPERTY(m_Type)
	DECLARE_PROPERTIES_END()
};

class SuperShaderConstant : public ShaderConstant
{
public:
	std_string m_SuperName = L("");

public:
	DECLARE_DERIVED_PROPERTIES_BEGIN(SuperShaderConstant, ShaderConstant)
		ADD_PROPERTY(m_SuperName)
	DECLARE_DERIVED_PROPERTIES_END()
};


class ShaderCode
{
public:
	std_string m_Type;
	std_string m_Code;

	DECLARE_PROPERTIES_BEGIN(ShaderCode)
		ADD_PROPERTY(m_Type),
		ADD_PROPERTY(m_Code)
	DECLARE_PROPERTIES_END()
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
	ShaderCode* m_pShaderCode;

	std::vector<ShaderConstant> m_vConstants;

	std::vector<ShaderConstant*> m_vConstantsTest;

public:
	DECLARE_PROPERTIES_BEGIN(Shader)
		ADD_ENUM_PROPERTY(m_eType, kShaderTypeStrings),
		ADD_PROPERTY(m_HLSLCompiledFile),
		ADD_PROPERTY(m_ShaderCode),
		ADD_PROPERTY(m_pShaderCode),
		ADD_PROPERTY(m_vConstants),
		ADD_PROPERTY(m_vConstantsTest)
	DECLARE_PROPERTIES_END()
};
