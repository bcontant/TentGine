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
	virtual unsigned long int Size() const;

protected:
	FILE*   m_FileHandle;
	FileMode m_FileMode;
};

