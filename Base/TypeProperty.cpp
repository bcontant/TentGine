#include "precompiled.h"

#include "TypeProperty.h"

void* TypeProperty::GetPtr(void* obj) const
{ 
	if (m_bIsStatic)
		return (void*)m_Offset;

	CHECK_ERROR_MSG(ErrorCode::NullInstanceForProperty, obj != nullptr, L("Tried to get a property's address with a null obj pointer.  Nothing good will come of this."));
	return (u8*)obj + m_Offset; 
}
