#include "precompiled.h"

#include "MemoryBuffer.h"

//--------------------------------------------------------------------------------
u32 MemoryBuffer::ClampSize(u32 in_size) const
{
	if (m_uiCurrentPosition + in_size > m_uiBufferSize)
		return m_uiBufferSize - m_uiCurrentPosition;

	return in_size;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Advance(u32 size)
{
	size = ClampSize(size);

	m_uiCurrentPosition += size;
	return size;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::SeekTo(u32 offset)
{
	m_uiCurrentPosition = 0;
	offset = ClampSize(offset);
	m_uiCurrentPosition = offset;
	return m_uiCurrentPosition;
}

//--------------------------------------------------------------------------------
const void* MemoryBuffer::PeekAt(u32 offset) const
{
	Assert(offset < m_uiBufferSize);
	return &m_pBuffer[offset];
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Read(Buffer* buffer, u32 bufferSize)
{
	bufferSize = ClampSize(bufferSize);
	return buffer->Write(m_pBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Write(Buffer* buffer, u32 bufferSize)
{
	bufferSize = ClampSize(bufferSize);
	return buffer->Read((void*)m_pBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Read(void* buffer, u32 bufferSize)
{
	bufferSize = ClampSize(bufferSize);

	memcpy(buffer, &m_pBuffer[m_uiCurrentPosition], bufferSize);
	m_uiCurrentPosition += bufferSize;
	return bufferSize;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Write(const void* buffer, u32 bufferSize)
{
	bufferSize = ClampSize(bufferSize);

	memcpy(&m_pBuffer[m_uiCurrentPosition], buffer, bufferSize);
	m_uiCurrentPosition += bufferSize;
	return bufferSize;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Read(std_string& /*in_string*/)
{
	Assert(false);
	return 0;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Write(const std_string& /*in_string*/)
{
	Assert(false);
	return 0;
}

//--------------------------------------------------------------------------------
u32 MemoryBuffer::Print(const string_char* /*buffer*/, ...)
{
	Assert(false);
	return 0;
}
