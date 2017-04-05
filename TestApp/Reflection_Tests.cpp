#include "../Base/Reflection.h"
#include "../Base/ErrorHandler.h"

struct OtherType
{
	OtherType() { w = -50.f; }
	float w;
};

struct NotDestructibleClass
{
	NotDestructibleClass() { w = -50.f; }
	float Test() { w = 50; return w; }
	float w;
private:
	NotDestructibleClass(const NotDestructibleClass&) {}
	virtual ~NotDestructibleClass() {}
};

struct Foo
{
	int a = 0;
	virtual void foo() {}
};

struct DerivedFoo : public Foo
{
	int b = 100;
	virtual void foo() {}
};

struct MyTypeBase
{
	MyTypeBase() { cache = 0xFAFBFCFD; }
	virtual ~MyTypeBase() { cache = 0x0; }
	virtual float TestModifyMember() { return 0.f; }
	u32 cache;
};

enum class TestEnumType { VAL_A, VAL_B, VAL_C };

struct MyType : MyTypeBase
{
	REFLECTABLE(MyType)

	MyType()
	{

	}
	MyType(int i, float f, char c) 
	{
		x = i; y = f; z = c; private_enum = TestEnumType::VAL_C; 
		vInts.push_back(314); vInts.push_back(159); 
		k = new int[10]; k[0] = 999;
		other_ptr = new OtherType;
		other_ptr4 = new OtherType***;
		other_ptr4[0] = new OtherType**;
		other_ptr4[0][0] = new OtherType*;
		other_ptr4[0][0][0] = new OtherType;
		other_ptr4[0][0][0][0].w = 9999;
		vFoos.push_back(new Foo[10]);    //TODO : See?  It might be 10 objects!  Not just one!
		vFoos.push_back(new DerivedFoo);
	}

	~MyType() { x = 9999999; }
	float			TestModifyMember() { x = 100; return 1.f; }
	void			TestPassReference(int& i) { x = 200; i = 314159; }
	int				TestPassPointer(void* in_p) { *((unsigned int*)in_p) = 123; return 12; }
	int				TestPassBaseClass(MyType* in_p) { in_p->x = 0x100; return 356; }

	int x;
	float y;
	char z;
	int* k = nullptr;
	std_string name = L("PoopFace");
	OtherType other;
	OtherType* other_ptr = nullptr;
	OtherType**** other_ptr4 = nullptr;
	std::vector<int> vInts;
	std::vector<Foo*> vFoos;

	int numbers[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
private:
	TestEnumType private_enum;
};

struct NoOperatorsTest
{
	int opTest = 65535;
private:
	NoOperatorsTest() = delete;
	NoOperatorsTest(const NoOperatorsTest&) = delete;
	~NoOperatorsTest() = delete;
	NoOperatorsTest& operator=(const NoOperatorsTest&) = delete;
};

struct NoDtorTest
{
private:
	~NoDtorTest() = delete;
};

struct NoCopyCtor
{
	int value = 0;
	NoCopyCtor() : value(0) {}
private:
	NoCopyCtor(const NoCopyCtor&) = delete;
};

struct EmptyCopyCtor
{
	int value = 0;
	EmptyCopyCtor() : value(0) {}
	EmptyCopyCtor(const EmptyCopyCtor&) {}
};

struct NoAssignmentOperator
{
	int value = 0;
	NoAssignmentOperator& operator=(const NoAssignmentOperator&) = delete;
};

struct CustomCtor
{
	int value = 0;
	CustomCtor() {};
	CustomCtor(int in) { value = in; } 
};

struct BaseTestType
{
	int value = 50;

	void TestBaseMethod() {}
	virtual void TestVirtual() { value = 700; }
};

struct TestType : public BaseTestType
{	
	static int TestStatic(int& value) { int tmp = value; value = 775; return tmp; }
	virtual void TestVirtual() { value = 500; }

	int& TestReturnRef() { return value; }
	int* TestReturnPtr() { return &value; }
	Variant TestReturnVariant() { return value; }
	VariantRef TestReturnVariantRef() { return value; }

	void TestPassRef(int& in) { in = 100; }
	void TestPassPtr(int* in) { *in = 1000; }
	
	void Test2Params(int& in1, int& in2) { in1 = 10; in2 = 100; }
	void Test3Params(int& in1, int& in2, int& in3) { in1 = 10; in2 = 100; in3 = 1000; }
	void Test4Params(int& in1, int& in2, int& in3, int& in4) { in1 = 10; in2 = 100; in3 = 1000; in4 = 10000; }
	void Test5Params(int& in1, int& in2, int& in3, int& in4, int& in5) { in1 = 10; in2 = 100; in3 = 1000; in4 = 10000; in5 = 100000; }
	void Test6Params(int& in1, int& in2, int& in3, int& in4, int& in5, int& in6) { in1 = 10; in2 = 100; in3 = 1000; in4 = 10000; in5 = 100000; in6 = 1000000; }
	void Test7Params(int& in1, int& in2, int& in3, int& in4, int& in5, int& in6, int& in7) { in1 = 10; in2 = 100; in3 = 1000; in4 = 10000; in5 = 100000; in6 = 1000000; in7 = 10000000; }
};

DECLARE_TYPE(NoOperatorsTest)
END_DECLARE(NoOperatorsTest)

DECLARE_TYPE(NoDtorTest)
END_DECLARE(NoDtorTest)

DECLARE_TYPE(NoCopyCtor)
END_DECLARE(NoCopyCtor)

DECLARE_TYPE(EmptyCopyCtor)
END_DECLARE(EmptyCopyCtor)

DECLARE_TYPE(NoAssignmentOperator)
END_DECLARE(NoAssignmentOperator)

DECLARE_TYPE(CustomCtor)
	SET_DEFAULT_CTOR_ARGS(150)
END_DECLARE(CustomCtor)

DECLARE_TYPE(Foo)
	ADD_PROP(a)
END_DECLARE(Foo)

DECLARE_TYPE(DerivedFoo)
	BASE_CLASS(Foo)
	ADD_PROP(b)
END_DECLARE(DerivedFoo)

DECLARE_TYPE(BaseTestType)
	ADD_FUNC(TestBaseMethod)
	ADD_FUNC(TestVirtual)	
END_DECLARE(BaseTestType)

DECLARE_TYPE(TestType)
	BASE_CLASS(BaseTestType)
	ADD_FUNC(TestStatic)
	ADD_FUNC(TestReturnRef)
	ADD_FUNC(TestReturnPtr)
	ADD_FUNC(TestReturnVariant)
	ADD_FUNC(TestReturnVariantRef)
	ADD_FUNC(TestPassRef)
	ADD_FUNC(TestPassPtr)
	ADD_FUNC(Test2Params)
	ADD_FUNC(Test3Params)
	ADD_FUNC(Test4Params)
	ADD_FUNC(Test5Params)
	ADD_FUNC(Test6Params)
//	ADD_FUNC(Test7Params) //Static assert
END_DECLARE(TestType)

DECLARE_TYPE(NotDestructibleClass)
	ADD_PROP(w)
	ADD_FUNC(Test)
END_DECLARE(NotDestructibleClass)

DECLARE_TYPE(OtherType)
	ADD_PROP(w)
END_DECLARE(OtherType)

DECLARE_TYPE(MyTypeBase)
	ADD_PROP(cache)
END_DECLARE(MyTypeBase)

DECLARE_TYPE(MyType)
	BASE_CLASS(MyTypeBase)
	SET_DEFAULT_CTOR_ARGS(100, 100.f, 100)
		ADD_PROP(vFoos)
	ADD_PROP(x)
	ADD_PROP(y)
	ADD_PROP(z)
	ADD_PROP(k)
	ADD_PROP(name)
	ADD_PROP(other)
	ADD_PROP(other_ptr)
	ADD_PROP(other_ptr4)
	ADD_PROP(vInts)
	
	ADD_PROP(numbers)
	ADD_PROP(private_enum)
	ADD_FUNC(TestModifyMember)
	ADD_FUNC(TestPassReference)
	ADD_FUNC(TestPassPointer)
	ADD_FUNC(TestPassBaseClass)
END_DECLARE(MyType)

DECLARE_TYPE(TestEnumType)
	ADD_ENUM_VALUE(VAL_A)
	ADD_ENUM_VALUE(VAL_B)
	ADD_ENUM_VALUE(VAL_C)
END_DECLARE(TestEnumType)

DECLARE_TYPE(Type)
	ADD_PROP(base_type)
	ADD_PROP(m_Name)
	ADD_PROP(vProperties)
	ADD_PROP(vFunctions)
END_DECLARE(Type)

void UnregisterClasses();
void TestVariants();
void TestOperators();
void TestProperties();
void TestMethods();
void TestEnums();

void TestReflection()
{
	ErrorManager::GetInstance()->SetErrorHandler(ErrorHandler::SilentHandler);

	TestVariants();
	TestOperators();
	TestProperties();
	TestMethods();
	TestEnums();

	Variant vContainer;

	XMLSerializer s;

	std::vector<float> vFloat = { 1.f, 2.f, 3.f, 5.f, 7.f, 11.f };
	vContainer = vFloat;
	vContainer.Serialize(&s);

	std::vector< std::vector<float> > vvFloat = { { 10.f, 20.f, 30.f },{ 100.f, 200.f, 300.f },{ 0.1f, 0.2f, 0.3f } };
	vContainer = vvFloat;
	vContainer.Serialize(&s);

	NotDestructibleClass* nodtor = new NotDestructibleClass;
	Variant nodtor_v(nodtor);
	nodtor_v.Serialize(&s);

	vContainer = TypeDB::GetInstance()->GetType<Type>();
	vContainer.Serialize(&s);

	MyType obj(564, 992.132f, 127);

/*
	TypeInfo::Get<DerivedFoo>();
	TypeInfo::Get<DerivedFoo*>();
*/

	vContainer = obj;
	vContainer.Serialize(&s);

	UnregisterClasses();

	Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
	ErrorManager::GetInstance()->m_vErrorCodes.clear();

	ErrorManager::GetInstance()->SetErrorHandler(ErrorHandler::AssertHandler);
}

void TestVariants()
{
	{
		//Testing Variant with references
		//Should behave as if VariantTests was a int;
		int val = 100;									int valInt = 100;
		int& refVal = val;								int& refInt = valInt;

		Variant VariantTests = refVal;					int VariantTestsInt = refInt;
		int refVal1 = VariantTests.As<int>();			int refValInt1 = (int)VariantTestsInt;
		int refVal2 = VariantTests.As<int&>();			int refValInt2 = (int&)VariantTestsInt;
		int& refVal4 = (int&)VariantTests;				int& refValInt4 = (int&)VariantTestsInt;

		val = 200;										valInt = 200;
		Assert(refVal1 == 100);							Assert(refValInt1 == 100);
		Assert(refVal2 == 100);							Assert(refValInt2 == 100);
		Assert(refVal4 == 100);							Assert(refValInt4 == 100);
		refVal4 = 500;									refValInt4 = 500;
		Assert(val == 200);								Assert(valInt == 200);
		Assert(refVal == 200);							Assert(refInt == 200);
		Assert(500 == VariantTests.As<int>());			Assert(500 == VariantTestsInt);
	}

	{
		//Testing VariantRef with references (init with copy ctor)
		//Should behave as if VariantTests was a int&;
		int val = 100;									int valInt = 100;
		int& refVal = val;								int& refInt = valInt;

		VariantRef VariantTests = refVal;				int& VariantTestsInt = refInt;
		int refVal1 = VariantTests.As<int>();			int refValInt1 = (int)VariantTestsInt;
		int refVal2 = VariantTests.As<int&>();			int refValInt2 = (int&)VariantTestsInt;
		int& refVal4 = (int&)VariantTests;				int& refValInt4 = (int&)VariantTestsInt;

		val = 200;										valInt = 200;
		Assert(refVal1 == 100);							Assert(refValInt1 == 100);
		Assert(refVal2 == 100);							Assert(refValInt2 == 100);
		Assert(refVal4 == 200);							Assert(refValInt4 == 200);
		refVal4 = 500;									refValInt4 = 500;
		Assert(val == 500);								Assert(valInt == 500);
		Assert(refVal == 500);							Assert(refInt == 500);
		Assert(500 == VariantTests.As<int&>());			Assert(500 == VariantTestsInt);
	}

	{
		//Testing VariantRef with references (init with assignment operator)
		//Should behave as if VariantTests was a int&;
		int val = 100;									int valInt = 100;
		int& refVal = val;								int& refInt = valInt;

		VariantRef VariantTests;						int& VariantTestsInt = refInt;
		VariantTests = refVal;							
		int refVal1 = VariantTests.As<int>();			int refValInt1 = (int)VariantTestsInt;
		int refVal2 = VariantTests.As<int&>();			int refValInt2 = (int&)VariantTestsInt;
		int& refVal4 = (int&)VariantTests;				int& refValInt4 = (int&)VariantTestsInt;

		val = 200;										valInt = 200;
		Assert(refVal1 == 100);							Assert(refValInt1 == 100);
		Assert(refVal2 == 100);							Assert(refValInt2 == 100);
		Assert(refVal4 == 200);							Assert(refValInt4 == 200);
		refVal4 = 500;									refValInt4 = 500;
		Assert(val == 500);								Assert(valInt == 500);
		Assert(refVal == 500);							Assert(refInt == 500);
		Assert(500 == VariantTests.As<int&>());			Assert(500 == VariantTestsInt);
	}

	{
		//Assignation from an uninitialized Variant (null)
		//Will assert (FailedCast), won't crash.
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
		Variant nullVar;
		int val = nullVar;
		Assert(val == 0);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedCast);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
		int* pVal = nullVar;
		Assert(pVal == nullptr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedCast);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
		int& refVal = nullVar;
		Assert(refVal == 0);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedCast);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
		refVal = 1000;
		refVal = nullVar;
		Assert(refVal == 0);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedCast);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();
	}

	{
		//Test pointers casting
		MyType* myTypePtr = (MyType*) nullptr + 0xcdcdcdcd;
		Variant v = myTypePtr;
		MyTypeBase* pBase = v;
		MyType* p = v;
		void* pVoid = v;
		Assert(pBase == myTypePtr);
		Assert(p == myTypePtr);
		Assert(pVoid == myTypePtr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 0);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		MyTypeBase* myBasePtr = (MyTypeBase*) nullptr + 0xdddddddd;
		v = myBasePtr;
		pBase = v;
		p = v;	//Assert
		pVoid = v;
		Assert(pBase == myBasePtr);
		Assert(p == myBasePtr);
		Assert(pVoid == myBasePtr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedCast);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();
	}

	{
		//Test const Variants
		float f = 500.f;
		const Variant v = f;
		//v = 200.f;  //Doesn't compile
		f = v;
		//float &fref = v;  //Doesn't compile
		const float &fcref = v;
		Assert(fcref == 500.f);
	}

	{
		//Test Variant copy constructor
		MyType obj(1000, 100.f, 10);
		Variant vTest1 = obj;
		Variant vTest2 = vTest1;

		Assert(vTest1.GetData() != vTest2.GetData());
		Assert(vTest1.As<MyType>().x == 1000 );
		Assert(vTest2.As<MyType>().z == 10 );
	}


	{
		//Test with complex types that don't agree with just being memcpy'ed around
		std_string str = L("My spoon is too big!");
		std::vector<std_string> vec = { L("My"), L("spoon"), L("is"), L("too"), L("big!") };
		{
			Variant strVar = str;
			Variant vecVar = vec;
			VariantRef strVarRef = str;
			VariantRef vecVarRef = vec;

			strVarRef.As<std_string>() = L("Your spoon is too small.");
			Assert(str == L("Your spoon is too small."));
			Assert(strVar.As<std_string>() == L("My spoon is too big!"));

			vecVarRef.As<std::vector<std_string>>().pop_back();
			Assert(vec.size() == 4);
			Assert(vecVar.As<std::vector<std_string>>().size() == 5);
		}
		Assert(str == L("Your spoon is too small."));
		Assert(vec.size() == 4);
	}
}

void TestOperators()
{
	DumpSerializer s;

	//Worst case : a class with no ctor, copy ctor, assignment operator and dtor
	{
		NoOperatorsTest* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<NoOperatorsTest>();

		//Assert : No constructor.  Still returns allocated memory. TODO : Return null?
		p = (NoOperatorsTest*)pInfo->New();
		Assert(p != nullptr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedConstructor);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		//2x Assert : No copy constructor and New + Assignment operator fails. Variant v got the correct size allocated but nothing else happened. TODO : Remain null?
		Variant v = *p;
		Assert(v.GetData() != nullptr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 2);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedConstructor);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[1] == ErrorCode::FailedAssignmentOperator);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		//2x Assert : New + Assignment operator fails. Variant v got the correct size allocated but nothing else happened. TODO : Remain null?
		Variant v2;
		v2 = v;
		Assert(v2.GetData() != nullptr);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 2);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedConstructor);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[1] == ErrorCode::FailedAssignmentOperator);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		v.Serialize(&s);

		//Assert : No destructor.  Still frees allocated memory.
		pInfo->Delete(p);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedDestructor);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();
	}

	//2x Assert : Variant destructor : no destructor for allocated type. Still frees allocated memory
	Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 2);
	Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedDestructor);
	Assert(ErrorManager::GetInstance()->m_vErrorCodes[1] == ErrorCode::FailedDestructor);
	ErrorManager::GetInstance()->m_vErrorCodes.clear();

	//No Destructor
	{
		NoDtorTest* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<NoDtorTest>();

		p = (NoDtorTest*) pInfo->New();
		Assert(p != nullptr);

		Variant v = *p;
		Assert(v.GetTypeInfo() == TypeInfo::Get<NoDtorTest>());

		v.Serialize(&s);

		//Assert : No destructor.  Still frees allocated memory.
		pInfo->Delete(p);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedDestructor);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();
	}
	//1x Assert : Variant destructor : no destructor for allocated type. Still frees allocated memory
	Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
	Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedDestructor);
	ErrorManager::GetInstance()->m_vErrorCodes.clear();

	//No Copy Ctor
	{
		NoCopyCtor* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<NoCopyCtor>();

		p = (NoCopyCtor*)pInfo->New();
		p->value = 100;

		//Calls Copy Ctor (which defaults to assignment operator)
		Variant v = *p;
		Assert(v.As<NoCopyCtor>().value == 100);

		//Calls assignment operator (unimplemented so default basic to memcpy)
		v = *p;
		Assert(v.As<NoCopyCtor>().value == 100);

		v.Serialize(&s);

		pInfo->Delete(p);
	}

	//Empty Copy Ctor
	{
		EmptyCopyCtor* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<EmptyCopyCtor>();

		p = (EmptyCopyCtor*)pInfo->New();
		p->value = 100;

		//Calls Copy Ctor (which does nothing)
		Variant v = *p;
		Assert(v.As<EmptyCopyCtor>().value == 0);

		//Calls assignment operator (unimplemented so defaults to copy)
		v = *p;
		Assert(v.As<EmptyCopyCtor>().value == 100);

		v.Serialize(&s);

		pInfo->Delete(p);
	}

	//No Assignment Operator
	{
		NoAssignmentOperator* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<NoAssignmentOperator>();

		p = (NoAssignmentOperator*)pInfo->New();
		p->value = 100;

		//Calls Copy Ctor (unimplemented -> copy)
		Variant v = *p;
		Assert(v.As<NoAssignmentOperator>().value == 100);

		//Calls assignment operator (Assert : no assignment operator.  No fallback)
		p->value = 200;
		v = *p;
		Assert(v.As<NoAssignmentOperator>().value == 100);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::FailedAssignmentOperator);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();

		v.Serialize(&s);

		pInfo->Delete(p);
	}

	//Custom ctor
	{
		CustomCtor* p = nullptr;
		const TypeInfo* pInfo = TypeInfo::Get<CustomCtor>();

		p = (CustomCtor*)pInfo->New();
		Assert(p->value == 150);

		pInfo->Delete(p);
	}
}

void TestProperties()
{

}

void TestMethods()
{
	Type* t = TypeDB::GetInstance()->GetType(L("TestType"));

	//Testing calling static function 
	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestStatic"));
		Assert(f != nullptr);
		int param = 7760;
		int ret = invoke(f, &obj, param);
		Assert(ret == 7760);
		Assert(param == 775);
	}

	//Testing calling static function 
	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestStatic"));
		Assert(f != nullptr);
		int param = 7760;
		int ret = invoke(f, obj, param);
		Assert(ret == 7760);
		Assert(param == 775);
	}

	//Testing calling virtual function on a derived class object
	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestVirtual"));
		Assert(f != nullptr);
		invoke(f, &obj);
		Assert(obj.value == 500);
	}

	//Testing calling virtual function on a derived class object with a base class pointer
	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestVirtual"));
		Assert(f != nullptr);
		BaseTestType* pObj = &obj;
		invoke(f, pObj);
		Assert(obj.value == 500);
	}

	//Testing calling virtual function on a base class object
	{
		BaseTestType obj;
		const TypeFunction* f = t->GetFunction(L("TestVirtual"));
		Assert(f != nullptr);
		invoke(f, &obj);
		Assert(obj.value == 700);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestReturnRef"));
		Assert(f != nullptr);
		int& i = invoke(f, &obj);
		i = 400;
		Assert(obj.value == 400);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestReturnPtr"));
		Assert(f != nullptr);
		int* i = invoke(f, &obj);
		*i = 800;
		Assert(obj.value == 800);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestReturnVariant"));
		Assert(f != nullptr);
		Variant v = invoke(f, &obj);
		v.As<int>() = 400;
		Assert(obj.value == 50);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestReturnVariantRef"));
		Assert(f != nullptr);
		VariantRef v = invoke(f, &obj);
		v.As<int>() = 400;
		Assert(obj.value == 400);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestPassRef"));
		Assert(f != nullptr);
		invoke(f, &obj, obj.value);
		Assert(obj.value == 100);
	}

	{
		TestType obj;
		const TypeFunction* f = t->GetFunction(L("TestPassPtr"));
		Assert(f != nullptr);
		invoke(f, &obj, &obj.value);
		Assert(obj.value == 1000);
	}

	{
		TestType obj;
		int i[2] = {};
		const TypeFunction* f = t->GetFunction(L("Test2Params"));
		Assert(f != nullptr);
		invoke(f, &obj, i[0], i[1]);
		Assert(i[0] == 10 && i[1] == 100);
	}

	{
		TestType obj;
		int i[3] = {};
		const TypeFunction* f = t->GetFunction(L("Test3Params"));
		Assert(f != nullptr);
		invoke(f, &obj, i[0], i[1], i[2]);
		Assert(i[0] == 10 && i[1] == 100 && i[2] == 1000);
	}

	{
		TestType obj;
		int i[4] = {};
		const TypeFunction* f = t->GetFunction(L("Test4Params"));
		Assert(f != nullptr);
		invoke(f, &obj, i[0], i[1], i[2], i[3]);
		Assert(i[0] == 10 && i[1] == 100 && i[2] == 1000 && i[3] == 10000);
	}

	{
		TestType obj;
		int i[5] = {};
		const TypeFunction* f = t->GetFunction(L("Test5Params"));
		Assert(f != nullptr);
		invoke(f, &obj, i[0], i[1], i[2], i[3], i[4]);
		Assert(i[0] == 10 && i[1] == 100 && i[2] == 1000 && i[3] == 10000 && i[4] == 100000);
	}

	{
		TestType obj;
		int i[6] = {};
		const TypeFunction* f = t->GetFunction(L("Test6Params"));
		Assert(f != nullptr);
		invoke(f, &obj, i[0], i[1], i[2], i[3], i[4], i[5]);
		Assert(i[0] == 10 && i[1] == 100 && i[2] == 1000 && i[3] == 10000 && i[4] == 100000 && i[5] == 1000000);
	}

	//Invoke with nullptr
	{
		TestType* obj = nullptr;
		const TypeFunction* f = t->GetFunction(L("TestPassRef"));
		Assert(f != nullptr);
		int value = 5;
		invoke(f, obj, value);
		Assert(value == 100);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes.size() == 1);
		Assert(ErrorManager::GetInstance()->m_vErrorCodes[0] == ErrorCode::NullInstanceForMethod);
		ErrorManager::GetInstance()->m_vErrorCodes.clear();
	}
	
	//TODO : Invoke with object
	
	//TODO : Invalid parameters (too few, too many, wrong type)
	//TODO : Invalid return type
}

void TestEnums()
{

}

void UnregisterClasses()
{
	TypeDB::GetInstance()->Clear();
}