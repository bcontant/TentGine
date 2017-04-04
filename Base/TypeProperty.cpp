#include "precompiled.h"

#include "TypeProperty.h"

void* TypeProperty::GetPtr(void* object) const
{ 
	return (u8*)object + offset; 
}
