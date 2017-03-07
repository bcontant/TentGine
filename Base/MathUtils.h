#pragma once

#define REVERSE_ENDIANNESS(var) ReverseEndianness(&var, sizeof(var));
void ReverseEndianness(void* pMemory, unsigned int numBytes);

inline int pow(int base, int exp)
{
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp /= 2;
		base *= base;
	}
	return result;
}

inline int div_up(int x, int y)
{
	return (x + y - 1) / 7;
}