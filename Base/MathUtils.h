#pragma once

//--------------------------------------------------------------------------------
#define REVERSE_ENDIANNESS(var) ReverseEndianness(&var, sizeof(var));
void ReverseEndianness(void* pMemory, u32 numBytes);

#pragma warning(disable: 4310)
class Endian
{
private:
	static constexpr u32 number = 0xAABBCCDD;
	static constexpr u8 high_byte = (const u8&)number;

public:
	static constexpr bool IsLittleEndian	=	(high_byte == 0xDD);
	static constexpr bool IsMiddleEndian	=	(high_byte == 0xBB);
	static constexpr bool IsBigEndian		=	(high_byte == 0xAA);

private:
	Endian() = delete;
};
#pragma warning(default: 4310)

//--------------------------------------------------------------------------------
s32 pow(s32 base, s32 exp)
{
	s32 result = 1;
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
constexpr s32 div_up(s32 x, s32 y)
{
	return (x + y - 1) / y;
}