#include "precompiled.h"

#include "Utils.h"
#include "ErrorHandler.h"

//-----------------------------------------
// Utils
//-----------------------------------------

const char* ENUM_PREFIX = "enum ";
const char* STRUCT_PREFIX = "struct ";
const char* CLASS_PREFIX = "class ";

std_string GenerateTypeName(const char* in_typeName)
{
	std_string typeName;

	if (strncmp(in_typeName, ENUM_PREFIX, strlen(ENUM_PREFIX)) == 0)
		typeName = FROM_STRING(in_typeName + strlen(ENUM_PREFIX));

	else if (strncmp(in_typeName, STRUCT_PREFIX, strlen(STRUCT_PREFIX)) == 0)
		typeName = FROM_STRING(in_typeName + strlen(STRUCT_PREFIX));

	else if (strncmp(in_typeName, CLASS_PREFIX, strlen(CLASS_PREFIX)) == 0)
		typeName = FROM_STRING(in_typeName + strlen(CLASS_PREFIX));

	else
		typeName = FROM_STRING(in_typeName);

	CHECK_ERROR(ErrorCode::InvalidTypeName, typeName != L("") && typeName[0] != ' ');

	return typeName;
}