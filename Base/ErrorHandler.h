#pragma once

#include "Logger.h"
#include "Singleton.h"
#include "Assert.h"

enum class ErrorCode
{
	NullFunctionPointer,
	NullInstanceForMethod,
	NullMetaInfo,

	FailedConstructor,
	FailedDestructor,
	FailedAssignmentOperator,
	FailedSerialization,

	PropertyAlreadyAdded,
	EnumValueAlreadyAdded,
	FunctionAlreadyAdded,

	InvalidTypeName,
	InvalidInstanceForMethod,
	InvalidArgumentCount,
	InvalidArgumentType,
	InvalidArgumentPointerType,
	InvalidContainerIndex,

	FailedCast,
	HashCollision,

	SerializingVoid,

	Undefined
};

enum class ErrorHandler
{
	AssertHandler,
	DebugOutHandler
};

struct ReflectionError : public StaticSingleton<ReflectionError>
{
	MAKE_STATIC_SINGLETON(ReflectionError);

	ReflectionError() : m_eErrorHandler(ErrorHandler::AssertHandler) {}
	
	void SetErrorHandler(ErrorHandler in_eHandler)
	{
		m_eErrorHandler = in_eHandler;
	}
	ErrorHandler m_eErrorHandler;
};

#define CHECK_ERROR_MSG(code, condition, errorStr, ...) \
do { \
	switch(ReflectionError::GetInstance()->m_eErrorHandler) \
	{ \
	case ErrorHandler::AssertHandler: \
	{ \
		AssertMsg( (condition), errorStr, ##__VA_ARGS__ ); \
		break; \
	} \
	case ErrorHandler::DebugOutHandler: \
	{ \
		if( !(condition) ) \
		{ \
			std_string errorMsg = Format("%s (Error Code = %d)", errorStr, code); \
			DEBUG_LOG( errorMsg.c_str(), ##__VA_ARGS__ ); \
		} \
		break; \
	} \
	} \
} while(false)

#define CHECK_ERROR(code, condition) CHECK_ERROR_MSG( (code), (condition), L("") )