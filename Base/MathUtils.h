#pragma once

//--------------------------------------------------------------------------------
#define REVERSE_ENDIANNESS(var) ReverseEndianness(&var, sizeof(var));
void ReverseEndianness(void* pMemory, unsigned int numBytes);

#pragma warning(disable: 4310)
class Endian
{
private:
	static constexpr unsigned int number = 0xAABBCCDD;
	static constexpr unsigned char high_byte = (const unsigned char&)number;

public:
	static constexpr bool little_endian =	(high_byte == 0xDD);
	static constexpr bool middle_endian =	(high_byte == 0xBB);
	static constexpr bool big_endian    =	(high_byte == 0xAA);

private:
	Endian() = delete;
};
#pragma warning(default: 4310)

//--------------------------------------------------------------------------------
int pow(int base, int exp)
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

//--------------------------------------------------------------------------------
constexpr int div_up(int x, int y)
{
	return (x + y - 1) / y;
}