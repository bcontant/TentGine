#pragma once

#include "ErrorHandler.h"
#include "Variant.h"

class Function_Pointer_Base {
public:
	Function_Pointer_Base(size_t in_ParamCount) : m_ParamCount(in_ParamCount) {}
	virtual ~Function_Pointer_Base(void) {};

	virtual AutoVariant Call(void*) { AssertMsg(false, L("Failed to find a suitable prototype for function call")); return Variant(); }
	virtual AutoVariant Call(void* p, VariantRef) { Assert(m_ParamCount < 1); return Call(p); }
	virtual AutoVariant Call(void* p, VariantRef v0, VariantRef) { Assert(m_ParamCount < 2); return Call(p, v0); }
	virtual AutoVariant Call(void* p, VariantRef v0, VariantRef v1, VariantRef) { Assert(m_ParamCount < 3); return Call(p, v0, v1); }
	virtual AutoVariant Call(void* p, VariantRef v0, VariantRef v1, VariantRef v2, VariantRef) { Assert(m_ParamCount < 4); return Call(p, v0, v1, v2); }
	virtual AutoVariant Call(void* p, VariantRef v0, VariantRef v1, VariantRef v2, VariantRef v3, VariantRef) { Assert(m_ParamCount < 5); return Call(p, v0, v1, v2, v3); }
	virtual AutoVariant Call(void* p, VariantRef v0, VariantRef v1, VariantRef v2, VariantRef v3, VariantRef v4, VariantRef) { Assert(m_ParamCount < 6); return Call(p, v0, v1, v2, v3, v4); }

protected:
	size_t m_ParamCount = 0;
};

template <typename T, typename U, int K>
class Function_Pointer;

template <typename T, typename ReturnType, typename Func, typename... Params>
typename std::enable_if<std::is_same<ReturnType, void>::value, AutoVariant>::type
CallImpl(void* in_pInstance, Func in_pFunction, Params&&... in_Params)
{
	CHECK_ERROR(ErrorCode::NullFunctionPointer, in_pFunction != nullptr);
	CHECK_ERROR(ErrorCode::NullInstanceForMethod, in_pInstance != nullptr);

	if (/*in_pInstance != nullptr &&*/ in_pFunction != nullptr)
		(((T*)in_pInstance)->*in_pFunction)(std::forward<Params>(in_Params)...);

	return AutoVariant(0);
}

template <typename T, typename ReturnType, typename Func, typename... Params>
typename std::enable_if<!std::is_same<ReturnType, void>::value, AutoVariant>::type
CallImpl(void* in_pInstance, Func in_pFunction, Params&&... in_Params)
{
	CHECK_ERROR(ErrorCode::NullFunctionPointer, in_pFunction != nullptr);
	CHECK_ERROR(ErrorCode::NullInstanceForMethod, in_pInstance != nullptr);

	if (/*in_pInstance != nullptr && */in_pFunction != nullptr)
		return (((T*)in_pInstance)->*in_pFunction)(std::forward<Params>(in_Params)...);

	return AutoVariant(0);
}

template <typename T, typename ReturnType, typename... Params, int K>
class Function_Pointer<T, ReturnType(Params...), K> : public Function_Pointer_Base
{ 
public:
	Function_Pointer(ReturnType(T::*)(Params...)) : Function_Pointer_Base(0) {}
};

#define DECLARE_PROTO(paramCount, callArguments, ...) \
	template <typename T, typename ReturnType, typename... Params> \
	class Function_Pointer<T, ReturnType(Params...), paramCount> : public Function_Pointer_Base \
	{ \
	public:\
		Function_Pointer( ReturnType(T::*in_pFunctionPointer)(Params...) ) : Function_Pointer_Base(paramCount), m_pFunctionPointer(in_pFunctionPointer) {} \
		\
		virtual AutoVariant Call(void* in_pInstance, ##__VA_ARGS__) \
		{ \
			return CallImpl<T, ReturnType> callArguments; \
		} \
	private: \
		ReturnType(T::*m_pFunctionPointer)(Params...) = nullptr; \
	}

DECLARE_PROTO(0, (in_pInstance, m_pFunctionPointer));
DECLARE_PROTO(1, (in_pInstance, m_pFunctionPointer, p0), VariantRef p0);
DECLARE_PROTO(2, (in_pInstance, m_pFunctionPointer, p0, p1), VariantRef p0, VariantRef p1);
DECLARE_PROTO(3, (in_pInstance, m_pFunctionPointer, p0, p1, p2), VariantRef p0, VariantRef p1, VariantRef p2);
DECLARE_PROTO(4, (in_pInstance, m_pFunctionPointer, p0, p1, p2, p3), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3);
DECLARE_PROTO(5, (in_pInstance, m_pFunctionPointer, p0, p1, p2, p3, p4), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3, VariantRef p4);
DECLARE_PROTO(6, (in_pInstance, m_pFunctionPointer, p0, p1, p2, p3, p4, p5), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3, VariantRef p4, VariantRef p5);

template <typename U, int K>
class Function_Pointer_Static;

template <typename ReturnType, typename Func, typename... Params>
typename std::enable_if<std::is_same<ReturnType, void>::value, AutoVariant>::type
CallImpl_Static(Func in_pFunction, Params&&... in_Params)
{
	CHECK_ERROR(ErrorCode::NullFunctionPointer, in_pFunction != nullptr);

	if (in_pFunction != nullptr)
		(in_pFunction)(std::forward<Params>(in_Params)...);

	return AutoVariant(0);
}

template <typename ReturnType, typename Func, typename... Params>
typename std::enable_if<!std::is_same<ReturnType, void>::value, AutoVariant>::type
CallImpl_Static(Func in_pFunction, Params&&... in_Params)
{
	CHECK_ERROR(ErrorCode::NullFunctionPointer, in_pFunction != nullptr);

	if (in_pFunction != nullptr)
		return (in_pFunction)(std::forward<Params>(in_Params)...);

	return AutoVariant(0);
}

template <typename ReturnType, typename... Params, int K>
class Function_Pointer_Static<ReturnType(Params...), K> : public Function_Pointer_Base
{
public:
	Function_Pointer_Static(ReturnType(*)(Params...)) : Function_Pointer_Base(0) {}
};

#define DECLARE_PROTO_STATIC(paramCount, callArguments, ...) \
	template <typename ReturnType, typename... Params> \
	class Function_Pointer_Static<ReturnType(Params...), paramCount> : public Function_Pointer_Base \
	{ \
	public:\
		Function_Pointer_Static( ReturnType(*in_pFunctionPointer)(Params...) ) : Function_Pointer_Base(paramCount), m_pFunctionPointer(in_pFunctionPointer) {} \
		\
		virtual AutoVariant Call(void*, ##__VA_ARGS__) \
		{ \
			return CallImpl_Static<ReturnType> callArguments; \
		} \
	private: \
		ReturnType(*m_pFunctionPointer)(Params...) = nullptr; \
	}

DECLARE_PROTO_STATIC(0, (m_pFunctionPointer));
DECLARE_PROTO_STATIC(1, (m_pFunctionPointer, p0), VariantRef p0);
DECLARE_PROTO_STATIC(2, (m_pFunctionPointer, p0, p1), VariantRef p0, VariantRef p1);
DECLARE_PROTO_STATIC(3, (m_pFunctionPointer, p0, p1, p2), VariantRef p0, VariantRef p1, VariantRef p2);
DECLARE_PROTO_STATIC(4, (m_pFunctionPointer, p0, p1, p2, p3), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3);
DECLARE_PROTO_STATIC(5, (m_pFunctionPointer, p0, p1, p2, p3, p4), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3, VariantRef p4);
DECLARE_PROTO_STATIC(6, (m_pFunctionPointer, p0, p1, p2, p3, p4, p5), VariantRef p0, VariantRef p1, VariantRef p2, VariantRef p3, VariantRef p4, VariantRef p5);