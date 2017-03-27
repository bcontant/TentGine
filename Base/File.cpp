#include "precompiled.h"

#include "File.h"

static const std::map<FileMode, std_string> kFileModes =
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
u32 File::Size() const
{
	if (!m_FileHandle)
		return 0;

	// Save current cursor position in stream
	u32 filePos = ftell(m_FileHandle);

	// Sets the position indicator to the end of file
	s32 ret = fseek(m_FileHandle, 0, SEEK_END);
	// Save end of file position
	u32 u32FileBufferSize = ftell(m_FileHandle);

	// Replace cursor position to initial one
	ret = fseek(m_FileHandle, filePos, SEEK_SET);

	return u32FileBufferSize;
}

//--------------------------------------------------------------------------------
u32 File::Advance(u32 size)
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
u32 File::SeekTo(u32 offset)
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
const void* File::PeekAt(u32 /*offset*/) const
{
	Assert(false);
	return nullptr;
}

static u8* pTmpBuffer = new u8[1024];
static u32 uiTmpBufferSize = 1024;

//--------------------------------------------------------------------------------
u32 File::Read(Buffer* buffer, u32 bufferSize)
{ 
	if (bufferSize > uiTmpBufferSize)
	{
		delete[] pTmpBuffer;
		pTmpBuffer = new u8[bufferSize];
		uiTmpBufferSize = bufferSize;
	}
	Read(pTmpBuffer, bufferSize);
	return buffer->Write(pTmpBuffer, bufferSize);
}

//--------------------------------------------------------------------------------
u32 File::Write(Buffer* buffer, u32 bufferSize) 
{ 
	if (bufferSize > uiTmpBufferSize)
	{
		delete[] pTmpBuffer;
		pTmpBuffer = new u8[bufferSize];
		uiTmpBufferSize = bufferSize;
	}
	buffer->Read(pTmpBuffer, bufferSize);
	return Write(pTmpBuffer, bufferSize);;
}

//--------------------------------------------------------------------------------
u32 File::Read(void* buffer, u32 bufferSize)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have something to read
	if (!m_FileHandle)
		return 0;

	// Read in the data
	u32 sizeRead = static_cast<u32>(fread(buffer, 1, bufferSize, m_FileHandle));
	return sizeRead;
}

//--------------------------------------------------------------------------------
u32 File::Write(const void* buffer, u32 bufferSize)
{
	AssertMsg(m_FileMode == FileMode::WriteOnly || m_FileMode == FileMode::Append || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	// Write the data
	return static_cast<u32>(fwrite(buffer, 1, bufferSize, m_FileHandle));
}

//--------------------------------------------------------------------------------
u32 File::Read(std_string& in_string)
{
	AssertMsg(m_FileMode == FileMode::ReadOnly || m_FileMode == FileMode::ReadWrite || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to read
	if (!m_FileHandle)
		return 0;

#define MAX_STR_LEN 2048
	u32 sizeRead = 0;
	u32 size = 0;
	sizeRead += static_cast<u32>(fread(&size, sizeof(u32), 1, m_FileHandle));
	AssertMsg(size < MAX_STR_LEN, L("Trying to read a string that is bigger than our fixed size buffer."));

	char tempStr[MAX_STR_LEN + 1];
	sizeRead += static_cast<u32>(fread(tempStr, 1, sizeof(char) * (size + 1), m_FileHandle));
	in_string = FROM_STRING(tempStr);

	return sizeRead;
}


//--------------------------------------------------------------------------------
u32 File::Write(const std_string& in_string)
{
	AssertMsg(m_FileMode == FileMode::WriteOnly || m_FileMode == FileMode::Append || m_FileMode == FileMode::ReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	u32 sizeWritten = 0;
	u32 size = static_cast<u32>(in_string.size());
	sizeWritten += static_cast<u32>(fwrite(&size, 1, sizeof(u32), m_FileHandle));

	sizeWritten += static_cast<u32>(fwrite(TO_STRING(in_string).c_str(), 1, sizeof(char) * (size + 1), m_FileHandle));

	return sizeWritten;
}

//--------------------------------------------------------------------------------
u32 File::Print(const string_char* format, ...)
{
	AssertMsg(m_FileMode == FileMode::WriteText || m_FileMode == FileMode::WriteTextAppend, L("Invalid Filemode"));

	va_list args;
	va_start(args, format);

	//s32 count = VFPRINTF(m_FileHandle, format, args);
	s32 count = vfprintf_s(m_FileHandle, TO_STRING(format).c_str(), args);
	if (count < 0)
	{
		count = 0;
	}

	va_end(args);

	// Returns the number of bytes written to the file
	return static_cast<u32>(count);
}
