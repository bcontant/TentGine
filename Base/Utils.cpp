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

	CHECK_ERROR_MSG(ErrorCode::InvalidTypeName, typeName != L("") && typeName[0] != ' ', L("GenerateTypeName is returning a messed up type name."));

	return typeName;
}

std_string GetGlobalFunctionName(const std_string& in_scope, const std_string& object_name)
{
	std::vector<std_string> vNamespaces;
	std_string objectName = L("");

	std_string toClean = object_name;
	toClean += L("::");
	toClean += in_scope;

	size_t scope_start = 0;
	size_t scope_end = 0;
	scope_end = toClean.find(L("::"), scope_start);
	while (scope_end != std::string::npos)
	{
		vNamespaces.push_back(toClean.substr(scope_start, scope_end - scope_start));
		scope_start = scope_end+2;
		scope_end = toClean.find(L("::"), scope_start);
	}
	objectName = toClean.substr(scope_start);
	
	if(vNamespaces.size() < 2)
		return toClean;
		
	std_string last_scope = L("");
	for(auto it = vNamespaces.begin(); it != vNamespaces.end();)
	{
		if (it != vNamespaces.begin())
		{
			if (*it == last_scope)
			{
				it = vNamespaces.erase(it);
				continue;
			}
		}
		last_scope = *it;
		it++;
	}

	std_string finalName = L("");
	for (auto it = vNamespaces.begin(); it != vNamespaces.end(); it++)
	{
		finalName += *it;
		finalName += L("::");
	}
	finalName += objectName;

	return finalName;
}