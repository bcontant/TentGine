#pragma once

#include "Buffer.h"

class Path;

// File modes supported
//--------------------------------------------------------------------------------
enum class FileMode
{
	ReadOnly,			// Open a file for reading.
	WriteOnly,			// Create an empty file for writing.
	Append,				// Append to an existing (or not) file.
	ReadWrite,			// Create an empty file for both reading and writing.
	ReadAppend,			// Open a file for reading and appending.
	WriteText,			// Open a file for printing text.
	WriteTextAppend,	// Append to an exisiting (or not) text file.
	ReadText			// Open a file for reading text
};

//--------------------------------------------------------------------------------
class File : public Buffer
{
public:
	// Constructor/Destructor
	File();
	virtual ~File();

	// Open/Close a file
	virtual bool Open(const Path& relativePath, FileMode fileType);
	virtual bool Close();

	// Flush pending operations to disk
	virtual void Flush();

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
	virtual u32 Size() const;

protected:
	FILE*   m_FileHandle;
	FileMode m_FileMode;
};

