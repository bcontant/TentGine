#include "precompiled.h"

#include "MathUtils.h"

// ------------------------------------------------------------------------------------
void ReverseEndianness(void* pMemory, u32 numBytes)
{
	AssertMsg(numBytes % 2 == 0, L("Number of bytes needs to be even for ReverseEndianness"));
	u8* pMem = reinterpret_cast<u8*>(pMemory);

	for (u32 i = 0; i < numBytes / 2; i++)
	{
		u8 tmp = pMem[i];
		pMem[i] = pMem[numBytes - i - 1];
		pMem[numBytes - i - 1] = tmp;
	}
}
