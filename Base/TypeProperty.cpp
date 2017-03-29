#include "precompiled.h"

#include "TypeProperty.h"

void* TypeProperty::GetPtr(void* object) 
{ 
	return (u8*)object + offset; 
}