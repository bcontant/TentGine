#include "precompiled.h"

const StringChar* File::s_FileModes[File::fmCount] =
{
	L("rb"),   //osfmReadOnly
	L("wb"),   //osfmWriteOnly
	L("ab"),   //osfmAppend
	L("r+b"),  //osfmReadWrite
	L("a+b"),  //osfmReadAppend
	L("w"),    //osfmWriteText
	L("a"),     //osfmWriteTextAppend
	L("r"),	//osfmReadText
};

//--------------------------------------------------------------------------------
File::File()
	:m_FileHandle(NULL)
	,m_FileMode(fmCount)
{
}

//--------------------------------------------------------------------------------
File::~File()
{
	// Make sure we don't leave dangling files
	Close();
}

//--------------------------------------------------------------------------------
bool File::Open(const Path& path, File::FileMode fileMode)
{
	// Make sure it isn't already open
	if (m_FileHandle)
		return false;

	// Translate the enum into a mode
	m_FileMode = fileMode;

	// Open the file
	errno_t ret = FOPEN(&m_FileHandle, path.GetData(), s_FileModes[m_FileMode]);

	if (ret != 0)
		Logger::GetInstance()->Log(eLC_FileSystem, eLS_Warning, eLT_File, L("Failed to open File (%s  :%s)"), path.GetData(), s_FileModes[m_FileMode]);
	else
		Logger::GetInstance()->Log(eLC_FileSystem, eLS_Message, eLT_File, L("Opening File (%s : %s)"), path.GetData(), s_FileModes[m_FileMode]);

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
	m_FileHandle = NULL;

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
long int File::Size()
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
	AssertMsg(m_FileMode == fmReadOnly || m_FileMode == fmReadWrite || m_FileMode == fmReadAppend, L("Invalid Filemode"));

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
	AssertMsg(m_FileMode == fmReadOnly || m_FileMode == fmReadWrite || m_FileMode == fmReadAppend, L("Invalid Filemode"));
	
	// Make sure we have something to read
	if (!m_FileHandle)
		return 0;

	if (fseek(m_FileHandle, offset, SEEK_SET) == 0)
		return offset;

	return 0;
}

//--------------------------------------------------------------------------------
unsigned int File::Read(void* buffer, unsigned int bufferSize)
{
	AssertMsg(m_FileMode == fmReadOnly || m_FileMode == fmReadWrite || m_FileMode == fmReadAppend, L("Invalid Filemode"));

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
	AssertMsg(m_FileMode == fmWriteOnly || m_FileMode == fmAppend || m_FileMode == fmReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	// Write the data
	return static_cast<unsigned int>(fwrite(buffer, 1, bufferSize, m_FileHandle));
}

//--------------------------------------------------------------------------------
unsigned int File::Read(StdString& in_string)
{
	AssertMsg(m_FileMode == fmReadOnly || m_FileMode == fmReadWrite || m_FileMode == fmReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

#define MAX_STR_LEN 2048

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
	AssertMsg(m_FileMode == fmWriteOnly || m_FileMode == fmAppend || m_FileMode == fmReadAppend, L("Invalid Filemode"));

	// Make sure we have somewhere to write
	if (!m_FileHandle)
		return 0;

	unsigned int sizeWritten = 0;
	unsigned int size = static_cast<unsigned int>(in_string.size());
	sizeWritten += static_cast<unsigned int>(fwrite(&size, 1, sizeof(unsigned int), m_FileHandle));

	sizeWritten += static_cast<unsigned int>(fwrite(in_string.c_str(), 1, sizeof(StringChar) * (size + 1), m_FileHandle));

	return sizeWritten;
}

//--------------------------------------------------------------------------------
unsigned int File::Print(const StringChar* format, ...)
{
	AssertMsg(m_FileMode == fmWriteText || m_FileMode == fmWriteTextAppend, L("Invalid Filemode"));

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
