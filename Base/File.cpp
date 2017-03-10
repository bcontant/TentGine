#include "precompiled.h"

#include "File.h"

static const std::map<FileMode, StdString> kFileModes =
{
	std::make_pair(FileMode::ReadOnly,			L("rb")),
	std::make_pair(FileMode::WriteOnly,			L("wb")),
	std::make_pair(FileMode::Append,			L("ab")),
	std::make_pair(FileMode::ReadWrite,			L("r+b")),
	std::make_pair(FileMode::ReadAppend,		L("a+b")),
	std::make_pair(FileMode::WriteText,			L("w")),
	std::make_pair(FileMode::WriteTextAppend,	L("a")),
	std::make_pair(FileMode::ReadText,			L("r"))
};

//--------------------------------------------------------------------------------
File::File()
	:m_FileHandle(nullptr)
	,m_FileMode(FileMode::ReadOnly)
{
}

//--------------------------------------------------------------------------------
File::~File()
{
	// Make sure we don't leave dangling files
	Close();
}

//--------------------------------------------------------------------------------
bool File::Open(const Path& path, FileMode fileMode)
{
	// Make sure it isn't already open
	if (m_FileHandle)
		return false;

	// Translate the enum into a mode
	m_FileMode = fileMode;

	// Open the file
	errno_t ret = FOPEN(&m_FileHandle, path.GetData(), kFileModes.at(m_FileMode).c_str());

	if (ret != 0)
		Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Warning, LogType::File, L("Failed to open File (%s  :%s)"), path.GetData(), kFileModes.at(m_FileMode).c_str());
	else
		Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::File, L("Opening File (%s : %s)"), path.GetData(), kFileModes.at(m_FileMode).c_str());

	return ret == 0;
}

//--------------------------------------------------------------------------------
bool File::Close()
{
	// Make sure we have something to close
	if (!m_FileHandle)
		return false;

	// Close the handle
	fclose(m_FileHandle);
	m_FileHandle = nullptr;

	return true;
}

//--------------------------------------------------------------------------------
void File::Flush()
{
	// Make sure we have something to flush
	if (!m_FileHandle)
		return;

	fflush(m_FileHandle);
}

//--------------------------------------------------------------------------------
unsigned long int File::Size() const
{
	if (!m_FileHandle)
		return 0;

	// Save current cursor position in stream
	long int filePos = ftell(m_FileHandle);

	// Sets the position indicator to the end of file
	int ret = fseek(m_FileHandle, 0, SEEK_END);
	// Save end of file position
	long int u32FileBufferSize = ftell(m_FileHandle);

	// Replace cursor position to initial one
	ret = fseek(m_FileHandle, filePos, SEEK_SET);

	return u32FileBufferSize;
}

//--------------------------------------------------------------------------------
unsigned int File::Advance(unsigned int size)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have something to read
	if (!m_FileHandle)
		return 0;

	if (fseek(m_FileHandle, size, SEEK_CUR) == 0)
		return size;

	return 0;
}

//--------------------------------------------------------------------------------
unsigned int File::SeekTo(unsigned int offset)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));
	
	// Make sure we have something to read
	if (!m_FileHandle)
		return 0;

	if (fseek(m_FileHandle, offset, SEEK_SET) == 0)
		return offset;

	return 0;
}

//--------------------------------------------------------------------------------
const void* File::PeekAt(unsigned int /*offset*/) const
{
	Assert(false);
	return nullptr;
}

static unsigned char* pTmpBuffer = new unsigned char[1024];
static unsigned int uiTmpBufferSize = 1024;

//--------------------------------------------------------------------------------
unsigned int File::Read(Buffer* buffer, unsigned int bufferSize)
{ 
	if (bufferSize > uiTmpBufferSize)
	{
		delete[] pTmpBuffer;
		pTmpBuffer = new unsigned char[bufferSize];
		uiTmpBufferSize = bufferSize;
	}
	Read(pTmpBuffer, bufferSize);
	return buffer->Write(pTmpBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
unsigned int File::Write(Buffer* buffer, unsigned int bufferSize) 
{ 
	if (bufferSize > uiTmpBufferSize)
	{
		delete[] pTmpBuffer;
		pTmpBuffer = new unsigned char[bufferSize];
		uiTmpBufferSize = bufferSize;
	}
	buffer->Read(pTmpBuffer, bufferSize);
	return Write(pTmpBuffer, bufferSize);;
}

//--------------------------------------------------------------------------------
unsigned int File::Read(void* buffer, unsigned int bufferSize)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have something to read
	if (!m_FileHandle)
		return 0;

	// Read in the data
	unsigned int sizeRead = static_cast<unsigned int>(fread(buffer, 1, bufferSize, m_FileHandle));
	return sizeRead;
}

//--------------------------------------------------------------------------------
unsigned int File::Write(const void* buffer, unsigned int bufferSize)
{
	AssertMsg(m_FileMode == FileMode::WriteOnly || m_FileMode == FileMode::Append || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	// Write the data
	return static_cast<unsigned int>(fwrite(buffer, 1, bufferSize, m_FileHandle));
}

//--------------------------------------------------------------------------------
unsigned int File::Read(StdString& in_string)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

#define MAX_STR_LEN 2048
	//TODO : This probably should be a fixed type otherwise files created with a Unicode binary won't load with a UTF8 binary and vice-versa
	unsigned int sizeRead = 0;
	unsigned int size = 0;
	sizeRead += static_cast<unsigned int>(fread(&size, sizeof(unsigned int), 1, m_FileHandle));
	AssertMsg(size < MAX_STR_LEN, L("Trying to read a string that is bigger than our fixed size buffer."));

	StringChar tempStr[MAX_STR_LEN + 1];
	sizeRead += static_cast<unsigned int>(fread(tempStr, 1, sizeof(StringChar) * (size + 1), m_FileHandle));
	in_string = tempStr;

	return sizeRead;
}


//--------------------------------------------------------------------------------
unsigned int File::Write(const StdString& in_string)
{
	AssertMsg(m_FileMode == FileMode::WriteOnly || m_FileMode == FileMode::Append || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	//TODO : This probably should be a fixed type otherwise files created with a Unicode binary won't load with a UTF8 binary and vice-versa
	unsigned int sizeWritten = 0;
	unsigned int size = static_cast<unsigned int>(in_string.size());
	sizeWritten += static_cast<unsigned int>(fwrite(&size, 1, sizeof(unsigned int), m_FileHandle));

	sizeWritten += static_cast<unsigned int>(fwrite(in_string.c_str(), 1, sizeof(StringChar) * (size + 1), m_FileHandle));

	return sizeWritten;
}

//--------------------------------------------------------------------------------
unsigned int File::Print(const StringChar* format, ...)
{
	AssertMsg(m_FileMode == FileMode::WriteText || m_FileMode == FileMode::WriteTextAppend, L("Invalid Filemode"));

	va_list args;
	va_start(args, format);

	int count = VFPRINTF(m_FileHandle, format, args);
	if (count < 0)
	{
		count = 0;
	}

	va_end(args);

	// Returns the number of bytes written to the file
	return static_cast<unsigned int>(count);
}
