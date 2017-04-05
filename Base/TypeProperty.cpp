#include "precompiled.h"

#include "TypeProperty.h"

void* TypeProperty::GetPtr(void* obj) const
{ 
	return (u8*)obj + m_Offset; 
}
