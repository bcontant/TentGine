#pragma once
#include "Variant.h"

class Function_Prototype_Base {
public:
	virtual ~Function_Prototype_Base(void) {};
};

template <typename T, typename U>
class Function_Prototype_With_Object;

template <typename T, typename ReturnType, typename... Params>
class Function_Prototype_With_Object<T, ReturnType(Params...)> : public Function_Prototype_Base
{
public:
	Function_Prototype_With_Object(const std::function<ReturnType(T*, Params...)>& f_prototype)
		: m_fct(f_prototype) {}

	ReturnType operator()(T* obj, Params... params)
	{
		return m_fct(obj, params...);
	}

private:
	std::function<ReturnType(T*, Params...)> m_fct;
};

template <typename T>
class Function_Prototype_Without_Object;

template <typename ReturnType, typename... Params>
class Function_Prototype_Without_Object<ReturnType(Params...)> : public Function_Prototype_Base
{
public:
	Function_Prototype_Without_Object(const std::function<ReturnType(Params...)>& f_prototype)
		: m_fct(f_prototype) {}

	ReturnType operator()(Params... params)
	{
		return m_fct(params...);
	}

private:
	std::function<ReturnType(Params...)> m_fct;
};

class Variant;

class Function_Prototype_Base_NoStd {
public:
	//Required for dynamic_casting
	Function_Prototype_Base_NoStd(size_t in_paramCount) : param_Count(in_paramCount) {}
	virtual ~Function_Prototype_Base_NoStd(void) {};

	//TODO : Warning on param dropping?

	virtual Variant Call(void*)	{ AssertMsg(false, L("Failed to find a suitable prototype for function call") ); return Variant(); }
	virtual Variant Call(void* p, Variant) { Assert(param_Count < 1); return Call(p); }
	virtual Variant Call(void* p, Variant v0, Variant) { Assert(param_Count < 2); return Call(p, v0); }
	virtual Variant Call(void* p, Variant v0, Variant v1, Variant) { Assert(param_Count < 3); return Call(p, v0, v1); }
	virtual Variant Call(void* p, Variant v0, Variant v1, Variant v2, Variant) { Assert(param_Count < 4); return Call(p, v0, v1, v2); }
	virtual Variant Call(void* p, Variant v0, Variant v1, Variant v2, Variant v3, Variant) { Assert(param_Count < 5); return Call(p, v0, v1, v2, v3); }
	virtual Variant Call(void* p, Variant v0, Variant v1, Variant v2, Variant v3, Variant v4, Variant) { Assert(param_Count < 6); return Call(p, v0, v1, v2, v3, v4); }

	virtual Variant Call() { AssertMsg(false, L("Failed to find a suitable prototype for function call")); return Variant(); }
	virtual Variant Call(Variant) { Assert(param_Count < 1); return Call(); }
	virtual Variant Call(Variant v0, Variant) { Assert(param_Count < 2); return Call(v0); }
	virtual Variant Call(Variant v0, Variant v1, Variant) { Assert(param_Count < 3); return Call(v0, v1); }
	virtual Variant Call(Variant v0, Variant v1, Variant v2, Variant) { Assert(param_Count < 4); return Call(v0, v1, v2); }
	virtual Variant Call(Variant v0, Variant v1, Variant v2, Variant v3, Variant) { Assert(param_Count < 5); return Call(v0, v1, v2, v3); }
	virtual Variant Call(Variant v0, Variant v1, Variant v2, Variant v3, Variant v4, Variant) { Assert(param_Count < 6); return Call(v0, v1, v2, v3, v4); }

protected:
	size_t param_Count = 0;
};

template <typename T, typename U, int K>
class Function_Prototype_NoStd;

template <typename T, typename ReturnType, typename Func, typename... Params>
typename std::enable_if<std::is_same<ReturnType, void>::value, Variant>::type
CallImpl(void* obj, Func fct, Params... params)
{
	Assert(obj != nullptr);
	Assert(fct != nullptr);

	if (obj != nullptr && fct != nullptr)
		(((T*)obj)->*fct)(std::forward<Params>(params)...);

	return Variant();
}

template <typename T, typename ReturnType, typename Func, typename... Params>
typename std::enable_if<!std::is_same<ReturnType, void>::value, Variant>::type
CallImpl(void* obj, Func fct, Params... params)
{
	Assert(obj != nullptr);
	Assert(fct != nullptr);

	if (obj != nullptr && fct != nullptr)
		return (((T*)obj)->*fct)(std::forward<Params>(params)...);

	return Variant();
}

#define DECLARE_PROTO(paramCount, callArguments, ...) \
	template <typename T, typename ReturnType, typename... Params> \
	class Function_Prototype_NoStd<T, ReturnType(Params...), paramCount> : public Function_Prototype_Base_NoStd \
	{ \
	public:\
		using FunctionPointer = ReturnType(T::*)(Params...); \
		Function_Prototype_NoStd(FunctionPointer f_prototype) : Function_Prototype_Base_NoStd(paramCount), m_fct(f_prototype) {} \
		\
		virtual Variant Call(void* obj, ##__VA_ARGS__) \
		{ \
			return CallImpl<T, ReturnType> callArguments; \
		} \
		virtual Variant Call(##__VA_ARGS__) \
		{ \
			void* obj = InstanceTypeInfo::Get<T>()->DefaultObject(); \
			return CallImpl<T, ReturnType> callArguments; \
		} \
	private: \
		FunctionPointer m_fct = nullptr; \
	}

DECLARE_PROTO(0, (obj, m_fct));
DECLARE_PROTO(1, (obj, m_fct, p0), Variant p0);
DECLARE_PROTO(2, (obj, m_fct, p0, p1), Variant p0, Variant p1);
DECLARE_PROTO(3, (obj, m_fct, p0, p1, p2)Variant p0, Variant p1, Variant p2);
DECLARE_PROTO(4, (obj, m_fct, p0, p1, p2, p3)Variant p0, Variant p1, Variant p2, Variant p3);
DECLARE_PROTO(5, (obj, m_fct, p0, p1, p2, p3, p4)Variant p0, Variant p1, Variant p2, Variant p3, Variant p4);
DECLARE_PROTO(6, (obj, m_fct, p0, p1, p2, p3, p4, p5)Variant p0, Variant p1, Variant p2, Variant p3, Variant p4, Variant p5);


