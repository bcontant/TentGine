#include "precompiled.h"

#include "Shader.h"

#ifdef _DEBUG
Path shaderPath2 = L("../../data/shaders/Debug/");
#else
Path shaderPath2 = L("../../data/shaders/Release/");
#endif


//--------------------------------------------------------------------------------
Renderer::Renderer(Window* in_pWindow)
	:m_pWindow(in_pWindow)
{
}

//--------------------------------------------------------------------------------
Renderer::~Renderer()
{
	UnloadShaders();
}

//--------------------------------------------------------------------------------
void Renderer::Initialize(DisplayAdapter* /*in_pAdapter*/)
{
	LoadShaders();
}

//--------------------------------------------------------------------------------
void Renderer::LoadShaders()
{
	//Scan for .cso files
	auto files = OS::GetFileList(shaderPath2, L(".cso"), true);

	//Load them, create shaders and shaderName == fileName
	for (auto file : files)
	{
		Shader* pNewShader = CreateShader(shaderPath2 + file);
		
		//fromXML<Shader>(pNewShader, "../../data/shaders/simple_vertex.xml");
		//fromXML(pNewShader, "../../data/shaders/simple_vertex.xml");

		m_vShaders.push_back(pNewShader);
	}
}

//--------------------------------------------------------------------------------
void Renderer::UnloadShaders()
{
	for (auto shader : m_vShaders)
		delete shader;
	m_vShaders.clear();
}

//--------------------------------------------------------------------------------
Font* Renderer::LoadFont(const Path& in_Filename)
{
	Font* newFont = new Font(this);
	newFont->Load(in_Filename);
	return newFont;
}


