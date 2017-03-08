#include "precompiled.h"

#include "MemoryBuffer.h"

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::ClampSize(unsigned int in_size) const
{
	if (m_uiCurrentPosition + in_size > m_uiBufferSize)
		return m_uiBufferSize - m_uiCurrentPosition;

	return in_size;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Advance(unsigned int size)
{
	size = ClampSize(size);

	m_uiCurrentPosition += size;
	return size;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::SeekTo(unsigned int offset)
{
	m_uiCurrentPosition = 0;
	offset = ClampSize(offset);
	m_uiCurrentPosition = offset;
	return m_uiCurrentPosition;
}

//--------------------------------------------------------------------------------
const void* MemoryBuffer::PeekAt(unsigned int offset) const
{
	Assert(offset < m_uiBufferSize);
	return &m_pBuffer[offset];
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Read(Buffer* buffer, unsigned int bufferSize)
{
	bufferSize = ClampSize(bufferSize);
	return buffer->Write(m_pBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Write(Buffer* buffer, unsigned int bufferSize)
{
	bufferSize = ClampSize(bufferSize);
	return buffer->Read((void*)m_pBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Read(void* buffer, unsigned int bufferSize)
{
	bufferSize = ClampSize(bufferSize);

	memcpy(buffer, &m_pBuffer[m_uiCurrentPosition], bufferSize);
	m_uiCurrentPosition += bufferSize;
	return bufferSize;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Write(const void* buffer, unsigned int bufferSize)
{
	bufferSize = ClampSize(bufferSize);

	memcpy(&m_pBuffer[m_uiCurrentPosition], buffer, bufferSize);
	m_uiCurrentPosition += bufferSize;
	return bufferSize;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Read(StdString& /*in_string*/)
{
	Assert(false);
	return 0;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Write(const StdString& /*in_string*/)
{
	Assert(false);
	return 0;
}

//--------------------------------------------------------------------------------
unsigned int MemoryBuffer::Print(const StringChar* /*buffer*/, ...)
{
	Assert(false);
	return 0;
}
