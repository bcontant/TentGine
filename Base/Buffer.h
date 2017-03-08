#pragma once

#include "StringUtils.h"

//--------------------------------------------------------------------------------
class Buffer
{
public:
	Buffer() {};
	virtual ~Buffer() {};

	// Read/Write
	virtual unsigned int Advance(unsigned int size) = 0;
	virtual unsigned int SeekTo(unsigned int offset) = 0;
	virtual const void* PeekAt(unsigned int offset) const = 0;

	virtual unsigned int Read(void* buffer, unsigned int bufferSize) = 0;
	virtual unsigned int Read(Buffer* buffer, unsigned int bufferSize) = 0;
	virtual unsigned int Write(const void* buffer, unsigned int bufferSize) = 0;
	virtual unsigned int Write(Buffer* buffer, unsigned int bufferSize) = 0;

	// Specialized methods to write/read string to binary files
	virtual unsigned int Read(StdString& in_string) = 0;
	virtual unsigned int Write(const StdString& in_string) = 0;

	// Read/Write to text files
	virtual unsigned int Print(const StringChar* buffer, ...) = 0;

	// Info
	virtual unsigned long int Size() const = 0;
};