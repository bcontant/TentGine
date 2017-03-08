#pragma once

#include "Buffer.h"

class Path;

//--------------------------------------------------------------------------------
class File : public Buffer
{
public:
	// File modes supported
	enum FileMode
	{
		fmReadOnly,			// Open a file for reading.
		fmWriteOnly,		// Create an empty file for writing.
		fmAppend,			// Append to an existing (or not) file.
		fmReadWrite,		// Create an empty file for both reading and writing.
		fmReadAppend,		// Open a file for reading and appending.
		fmWriteText,		// Open a file for printing text.
		fmWriteTextAppend,	// Append to an exisiting (or not) text file.
		fmReadText,			// Open a file for reading text
		fmCount
	};

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

	static const StringChar* s_FileModes[File::fmCount];
};

