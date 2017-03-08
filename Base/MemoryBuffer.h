#pragma once

#include "Buffer.h"

//--------------------------------------------------------------------------------
class MemoryBuffer : public Buffer
{
public:
	MemoryBuffer(unsigned char* in_pBuffer, unsigned int in_uiBufferSize) : m_pBuffer(in_pBuffer), m_uiBufferSize(in_uiBufferSize), m_uiCurrentPosition(0) {}
	virtual ~MemoryBuffer() {}

	// Read/Write
	virtual unsigned int Advance(unsigned int size);
	virtual unsigned int SeekTo(unsigned int offset);
	virtual const void* PeekAt(unsigned int offset) const;

	virtual unsigned int Read(void* buffer, unsigned int bufferSize);
	virtual unsigned int Read(Buffer* buffer, unsigned int bufferSize);
	virtual unsigned int Write(const void* buffer, unsigned int bufferSize);
	virtual unsigned int Write(Buffer* buffer, unsigned int bufferSize);

	// Specialized methods to write/read string to binary files
	virtual unsigned int Read(StdString& in_string);
	virtual unsigned int Write(const StdString& in_string);

	// Read/Write to text files
	virtual unsigned int Print(const StringChar* buffer, ...);

	// Info
	virtual unsigned long int Size() const { return m_uiBufferSize; }

private:
	unsigned int ClampSize(unsigned int in_size) const;

	unsigned char* m_pBuffer = nullptr;
	unsigned int m_uiBufferSize = 0;
	unsigned int m_uiCurrentPosition = 0;
};