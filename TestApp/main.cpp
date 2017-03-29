#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "../Base/Types.h"

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Profiler.h"
#include "../Base/Path.h"

#include "../OS_Base/System.h"
#include "../OS_Base/FontBuilder.h"
#include "../OS_Base/Window.h"

#include "../Renderer_Base/Renderer.h"
#include "../Renderer_Base/Font.h"
#include "../Renderer_Base/Quad.h"
#include "../Renderer_Base/Texture.h"
#include "../Renderer_Base/Text.h"
#include "../Renderer_Base/Shader.h"

#include "../Base/JPEGFile.h"
#include "../Base/PNGFile.h"
#include "../Base/BitmapData.h"

#include "../Base/XMLFile.h"

#include "../Base/Streamable.h"
#include "../Base/Reflection.h"

struct OtherType
{
	OtherType()
	{
		w = -50.f;
	}
	float w;
};

struct MyTypeBase
{
	MyTypeBase()
	{
		cache = 0xFAFBFCFD;
	}

	virtual ~MyTypeBase()
	{
		cache = 0x0;
	}

	virtual float Test2()
	{
		cache = 10;
		return 10.f;
	}
	u32 cache;
};

enum class TestEnumType
{
	VAL_A,
	VAL_B,
	VAL_C
};

struct MyType : MyTypeBase
{
	REFLECTABLE(MyType)
		
	MyType()
	{
		x = 0;
		y = 1.f;
		z = 2;
	}

	float Test() 
	{
		//x = 100;
		return 1.f;
	}

	virtual float Test2()
	{
		y = 100.f;
		return y;
	}

	int Test3(void* in_p)
	{
		*((unsigned int*)in_p) = 123;
		return 12;
	}

	bool Test4()
	{
		z = 'A';
		return true;
	}

	int x;
	float y;
	char z;
	
	OtherType other;
	OtherType* other_ptr = nullptr;

	std::vector<int> vInts;

private:
	u64 ohshit_private;
	TestEnumType private_enum;
};

DECLARE_TYPE(MyTypeBase)
	ADD_PROP(cache)
END_DECLARE(MyTypeBase)

DECLARE_TYPE(MyType)
	BASE_CLASS(MyTypeBase)
	ADD_PROP(x)
	ADD_PROP(y)
	ADD_PROP(z)
	ADD_PROP(other)
	ADD_PROP(other_ptr)
	ADD_PROP(vInts)
	ADD_PROP(ohshit_private)
	ADD_PROP(private_enum)
	ADD_FUNC(Test)
	ADD_FUNC(Test2)
	ADD_FUNC(Test3)
END_DECLARE(MyType)

DECLARE_TYPE(TestEnumType)
	ADD_ENUM_VALUE(VAL_A)
	ADD_ENUM_VALUE(VAL_B)
	ADD_ENUM_VALUE(VAL_C)
END_DECLARE(TestEnumType)

DECLARE_TYPE(Type)
	ADD_PROP(base_type)
	ADD_PROP(name)
	ADD_PROP(vProperties)
	//ADD_PROP(vFunctions)
END_DECLARE(Type)

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, s32)
{
	Logger::CreateInstance();
	Profiler::CreateInstance();

	MyType obj1;

	u8* objBuf = nullptr;
	Type* t = TypeDB::GetInstance()->GetType(L("MyType"));
	objBuf = new u8[t->size];
	t->constructor(objBuf);

	Variant v(&obj1);
	MyType* obj2 = v.GetValue<MyType*>();
	v = 17;
	int bla = v.GetValue<int>();
	v = 500.f;
	float fla = v.GetValue<float>();

	t->AddFunction(TypeFunction(std::make_pair(Name(L("Test4")), &MyType::Test4)));

	float fRet = FLT_MAX;
	fRet = Invoker<float()>::invoke(t->GetFunction(L("Test")));

	MyType* myObj = (MyType*)objBuf;
	fRet = Invoker<float()>::invoke(myObj, t->GetFunction(L("Test2")));
	fRet = Invoker<float()>::invoke((void*)objBuf, t->GetFunction(L("Test2")));
	fRet = Invoker<float()>::invoke(objBuf, t->GetFunction(L("Test2")));
	fRet = Invoker<float()>::invoke(t->GetFunction(L("Test2")));

	unsigned int i = 0;
	int iRet;
	iRet = Invoker<int(void*)>::invoke(myObj, t->GetFunction(L("Test3")), &i);
	iRet = Invoker<int(void*)>::invoke((void*)objBuf, t->GetFunction(L("Test3")), &i);
	iRet = Invoker<int(void*)>::invoke(objBuf, t->GetFunction(L("Test3")), &i);
	iRet = Invoker<int(void*)>::invoke(t->GetFunction(L("Test3")), &i);

	TypeProperty* f = t->GetProperty(L("y"));
	float *pf = (float*)f->GetPtr(objBuf);
	*pf = 3.f;

	TypeProperty* f2 = t->GetProperty(L("other_ptr"));
	u8* obj2Buf = new u8[f2->type->size];
	void** ppf = (void**)f2->GetPtr(objBuf);
	*ppf = obj2Buf;
	f2->type->constructor(obj2Buf);

	t->destructor(objBuf);

	CoInitialize(nullptr);

	XMLFile xmlfile;
	xmlfile.Parse(Path(L("../../data/shaders/simple_vertex.xml")));
	xmlfile.Output(Path(L("../../data/shaders/simple_vertex_parsed.xml")), eOF_SpaceIndents_2);

	Shader* pShader = XMLSerializer::Deserialize(&pShader, L("../../data/shaders/simple_vertex.xml"));

	//OS::BuildFont(L("../../data/Comic.ttf"), 36, 1024, OS::eFF_ForceAutoHint /*| OS::eFF_Mono*/, L("../../data/"));
	//OS::BuildFont(L("../../data/FFF_Tusj.ttf"), 72, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	OS::BuildFont(L("../../data/COURBD.TTF"), 24, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	//OS::BuildFont(L("../../data/COUR.TTF"), 24, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	OS::BuildFont(L("../../data/HyliaSerifBeta-Regular.otf"), 48, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));

	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	//OS::GetDisplayAdapters();
	
	Renderer* pRenderer = Renderer::CreateRenderer(nullptr, pWindow); 

	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.png"), EAddressingMode::eWrap, EFilteringMode::ePoint, EFilteringMode::ePoint, EFilteringMode::ePoint);
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.png"), EAddressingMode::eWrap, EFilteringMode::ePoint, EFilteringMode::ePoint, EFilteringMode::ePoint);
	Texture* pTexture3 = pRenderer->CreateTexture(L("../../data/red.png"), EAddressingMode::eWrap, EFilteringMode::ePoint, EFilteringMode::ePoint, EFilteringMode::ePoint);
	
	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	Font* pFont = pRenderer->LoadFont(L("../../data/Courier New Bold 24pt.font"));
	Font* pFont2 = pRenderer->LoadFont(L("../../data/Hylia Serif Beta Regular 48pt.font"));

	Text* pText = pRenderer->CreateText(0.0f, 0.0f, pFont, L("0 FPS"));
	pText->SetColor(0xff0000ff);

	Text* pText2 = pRenderer->CreateText(0.2f, 0.2f, pFont2, L("C'etait\n donc\n une\n\n fable, de la foret verte, allons y tous ensemble, la porte est grand ouverte.  Avec la joie et l'amour, dans tous les coeurs."));
	pText2->SetColor(0xffffffff);
	pText2->SetWidth(800.f);
	
	s64 lastTime = OS::GetTickCount();
	u32 frameCount = 0;

	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		/*pQuad->Draw();
		pQuad2->Draw();
		pQuad3->Draw();*/
		pText->Draw();
		pText2->Draw();

		pRenderer->EndFrame();

		s64 time = OS::GetTickCount();

		frameCount++;
		if ((time - lastTime) > OS::GetTickFrequency())
		{
			lastTime = time;
			pText->SetText(Format(L("%d FPS"), frameCount));
			frameCount = 0;
		}
	}

	delete pText2;
	delete pText;
	delete pQuad;
	delete pQuad2;
	delete pQuad3;
	delete pTexture1;
	delete pTexture2;
	delete pTexture3;
	delete pFont;
	delete pFont2;
	delete pRenderer;
	delete pWindow;

	Profiler::DestroyInstance();
	Logger::DestroyInstance();
}