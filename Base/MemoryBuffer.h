#pragma once

#include "Buffer.h"

//--------------------------------------------------------------------------------
class MemoryBuffer : public Buffer
{
public:
	MemoryBuffer(u8* in_pBuffer, u32 in_uiBufferSize) : m_pBuffer(in_pBuffer), m_uiBufferSize(in_uiBufferSize), m_uiCurrentPosition(0) {}
	virtual ~MemoryBuffer() {}

	// Read/Write
	virtual u32 Advance(u32 size);
	virtual u32 SeekTo(u32 offset);
	virtual const void* PeekAt(u32 offset) const;

	virtual u32 Read(void* buffer, u32 bufferSize);
	virtual u32 Read(Buffer* buffer, u32 bufferSize);
	virtual u32 Write(const void* buffer, u32 bufferSize);
	virtual u32 Write(Buffer* buffer, u32 bufferSize);

	// Specialized methods to write/read string to binary files
	virtual u32 Read(std_string& in_string);
	virtual u32 Write(const std_string& in_string);

	// Read/Write to text files
	virtual u32 Print(const string_char* buffer, ...);

	// Info
	virtual u32 Size() const { return m_uiBufferSize; }

private:
	u32 ClampSize(u32 in_size) const;

	u8* m_pBuffer = nullptr;
	u32 m_uiBufferSize = 0;
	u32 m_uiCurrentPosition = 0;
};