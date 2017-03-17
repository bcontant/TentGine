#pragma once

#include "StringUtils.h"

//--------------------------------------------------------------------------------
class Buffer
{
public:
	Buffer() {};
	virtual ~Buffer() {};

	// Read/Write
	virtual u32 Advance(u32 size) = 0;
	virtual u32 SeekTo(u32 offset) = 0;
	virtual const void* PeekAt(u32 offset) const = 0;

	virtual u32 Read(void* buffer, u32 bufferSize) = 0;
	virtual u32 Read(Buffer* buffer, u32 bufferSize) = 0;
	virtual u32 Write(const void* buffer, u32 bufferSize) = 0;
	virtual u32 Write(Buffer* buffer, u32 bufferSize) = 0;

	// Specialized methods to write/read string to binary files
	virtual u32 Read(std_string& in_string) = 0;
	virtual u32 Write(const std_string& in_string) = 0;

	// Read/Write to text files
	virtual u32 Print(const string_char* buffer, ...) = 0;

	// Info
	virtual u32 Size() const = 0;
};