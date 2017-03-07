#include "precompiled.h"

#include "MathUtils.h"

// ------------------------------------------------------------------------------------
void ReverseEndianness(void* pMemory, unsigned int numBytes)
{
	AssertMsg(numBytes % 2 == 0, L("Number of bytes needs to be even for ReverseEndianness"));
	char* pMem = reinterpret_cast<char*>(pMemory);

	for (unsigned int i = 0; i < numBytes / 2; i++)
	{
		char tmp = pMem[i];
		pMem[i] = pMem[numBytes - i - 1];
		pMem[numBytes - i - 1] = tmp;
	}
}
