#include "precompiled.h"

// defines
#define MAX_FILENAME_LEN 512

// The number of characters at the start of an absolute filename.  e.g. in DOS,
// absolute filenames start with "X:\" so this value should be 3, in UNIX they start
// with "\" so this value should be 1.
#ifdef _WIN32
#define ABSOLUTE_NAME_START 3
#else
#define ABSOLUTE_NAME_START 1
#endif

//--------------------------------------------------------------------------------
Path::Path()
{
}

//--------------------------------------------------------------------------------
Path::Path(const string_char* in_path)
{
	m_strPath = in_path;
	CleanUpPath();
}

//--------------------------------------------------------------------------------
Path::Path(const std_string& in_path)
{
	m_strPath = in_path;
	CleanUpPath();
}

//--------------------------------------------------------------------------------
Path::Path(const Path& in_path)
{
	m_strPath = in_path.m_strPath;
	CleanUpPath();
}

//--------------------------------------------------------------------------------
Path& Path::operator=(const string_char* in_path)
{
	m_strPath = in_path;
	CleanUpPath();
	return *this;
}

//--------------------------------------------------------------------------------
Path& Path::operator=(const std_string& in_path)
{
	m_strPath = in_path;
	CleanUpPath();
	return *this;
}

//--------------------------------------------------------------------------------
Path& Path::operator=(const Path& in_path)
{
	m_strPath = in_path.m_strPath;
	CleanUpPath();
	return *this;
}

//--------------------------------------------------------------------------------
Path Path::operator+(const Path& in_path) const
{
	Path path(*this);
	path.m_strPath += in_path.m_strPath;
	path.CleanUpPath();
	return path;
}

//--------------------------------------------------------------------------------
Path& Path::operator+=(const Path& in_path)
{
	m_strPath += in_path.m_strPath;
	CleanUpPath();
	return *this;
}

//--------------------------------------------------------------------------------
bool Path::operator==(const Path& in_path) const
{
	return m_strPath == in_path.m_strPath;
}

//--------------------------------------------------------------------------------
bool Path::operator!=(const Path& in_path) const
{
	return m_strPath != in_path.m_strPath;
}

//--------------------------------------------------------------------------------
bool Path::operator<(const Path& in_path) const
{
	return m_strPath < in_path.m_strPath;
}

//--------------------------------------------------------------------------------
Path& Path::SetFolder(const Path& in_Folder)
{
	AssertMsg(in_Folder.m_strPath.find_last_of('/') == in_Folder.m_strPath.size() - 1 || in_Folder.m_strPath.find_last_of('\\') == in_Folder.m_strPath.size() - 1 || in_Folder == L(""), L("Invalid folder (%s)"), in_Folder.GetData());

	m_Folder = in_Folder.GetData();
	BuildPath();

	return *this;
}

//--------------------------------------------------------------------------------
Path& Path::SetName(const Path& in_Name)
{
	AssertMsg(in_Name.m_strPath.find_first_of('/') == std_string::npos, L("Path::SetName : Invalid name (%s)"), in_Name.GetData());
	AssertMsg(in_Name.m_strPath.find_first_of('\\') == std_string::npos, L("Path::SetName : Invalid name (%s)"), in_Name.GetData());

	m_Name = in_Name.GetData();
	BuildPath();

	return *this;
}

//--------------------------------------------------------------------------------
Path& Path::SetExtension(const Path& in_Extension)
{
	AssertMsg(in_Extension.m_strPath.find_first_of('/') == std_string::npos, L("Invalid extension (%s)"), in_Extension.GetData());
	AssertMsg(in_Extension.m_strPath.find_first_of('\\') == std_string::npos, L("Invalid extension (%s)"), in_Extension.GetData());
	AssertMsg(in_Extension.m_strPath.size() == 0 || in_Extension.m_strPath[0] == '.', L("Invalid extension (%s)"), in_Extension.GetData());

	m_Extension = in_Extension.GetData();
	BuildPath();

	return *this;
}

//--------------------------------------------------------------------------------
void Path::BuildPath()
{
	m_strPath = m_Folder + m_Name + m_Extension;
	CleanUpPath();
}

//--------------------------------------------------------------------------------
void Path::CleanUpPath()
{
	for (u32 i = 0; i < static_cast<u32>(m_strPath.size()); i++)
	{
		//Clean up the '\\' into '/'
		if (m_strPath[i] == '\\')
			m_strPath[i] = '/';
	}
	
	std_string::size_type index = 0;
	std_string::size_type searchOffset = 0;

	//Remove double slashes
	//Allow network paths (//computer/folder)
	while ((index = m_strPath.find(L("//"), 1)) != std_string::npos)
	{
		m_strPath.erase(index, 1);
	}

	//Clean up the ".." to remove them
	//First, find the start of the first ".."
	while ((index = m_strPath.find(L(".."), searchOffset)) != std_string::npos)
	{
		if (index < ABSOLUTE_NAME_START + 2)
		{
			//This looks like a relative path leading with a .., nothing to do about it
			searchOffset = index + 1;
			continue;
		}

		//Now, find the leading '/' of the preceding folder name
		std_string::size_type folderIndex = m_strPath.find_last_of('/', index - 2);
		AssertMsg(folderIndex != std_string::npos, L("Invalid path has a '..' in a weird place.. (%s)"), m_strPath.c_str());

		//Now remove it all
		m_strPath.erase(folderIndex, (index - folderIndex) + 2);
	}

	std_string::size_type lastSlashPos = m_strPath.find_last_of('/');
	std_string::size_type lastDotPos = m_strPath.find_last_of('.');
	if (lastDotPos < lastSlashPos && lastSlashPos != std_string::npos)
		lastDotPos = std_string::npos;

	if (lastSlashPos == std_string::npos)
	{
		m_Folder = L("");
		lastSlashPos = static_cast<size_t>(-1);
	}
	else
	{
		m_Folder = m_strPath.substr(0, lastSlashPos + 1);
	}

	if (lastDotPos == std_string::npos)
	{
		m_Extension = L("");
		lastDotPos = m_strPath.size();
	}
	else
	{
		m_Extension = m_strPath.substr(lastDotPos, m_strPath.size() - lastDotPos + 1);
	}

	m_Name = m_strPath.substr(lastSlashPos + 1, lastDotPos - (lastSlashPos + 1));

	AssertMsg(m_Folder.size() == 0 || m_Folder[m_Folder.size() - 1] == '/', L("Resulting m_Folder is invalid (Full Path = %s, Resulting m_Folder = %s)"), m_strPath.c_str(), m_Folder.c_str());
	AssertMsg(m_Extension.find_first_of('/') == std_string::npos, L("Resulting m_FileExtension is invalid (Full Path = %s, Resulting m_FileExtension = %s)"), m_strPath.c_str(), m_Extension.c_str());
	AssertMsg(m_Name.find_first_of('/') == std_string::npos, L("Resulting m_FileName is invalid (Full Path = %s, Resulting m_FileName = %s)"), m_strPath.c_str(), m_Name.c_str());

	AssertMsg(m_Folder + m_Name + m_Extension == m_strPath, L("Split Path is different from original path (Full Path = %s, Resulting m_Folder = %s, Resulting m_FileExtension = %s, Resulting m_FileName = %s)"), m_strPath.c_str(), m_Folder.c_str(), m_Extension.c_str(), m_Name.c_str());
}





// Given the absolute current directory and an absolute file name, returns a relative file name.
// For example, if the current directory is C:\foo\bar and the filename C:\foo\whee\text.txt is given,
// GetRelativePath will return ..\whee\text.txt.
//--------------------------------------------------------------------------------
Path Path::GetRelativePath(const Path& in_WorkingDirectory) const
{
	if (m_strPath == L(""))
		return L("");

	const string_char* currentDirectory = in_WorkingDirectory.GetData();
	const string_char* absoluteFilename = GetData();

	// declarations - put here so this should work in a C compiler
	s32 afMarker = 0, rfMarker = 0;
	s32 cdLen = 0;
	s32 afLen = 0;
	s32 i = 0;
	s32 levels = 0;
	static string_char relativeFilename[MAX_FILENAME_LEN + 1];

	cdLen = static_cast<s32>(STRLEN(currentDirectory));
	afLen = static_cast<s32>(STRLEN(absoluteFilename));

	// make sure the names are not too long or too short
	if (cdLen > MAX_FILENAME_LEN || cdLen < ABSOLUTE_NAME_START + 1 ||
		afLen > MAX_FILENAME_LEN || afLen < ABSOLUTE_NAME_START + 1)
	{
		// not on the same drive, so only absolute filename will do
		STRCPY(relativeFilename, MAX_FILENAME_LEN, absoluteFilename);
		return relativeFilename;
	}

#ifdef _WIN32
	// Handle DOS names that are on different drives:
	if (currentDirectory[0] != absoluteFilename[0])
	{
		// not on the same drive, so only absolute filename will do
		STRCPY(relativeFilename, MAX_FILENAME_LEN, absoluteFilename);
		return relativeFilename;
	}
#endif

	// they are on the same drive, find out how much of the current directory
	// is in the absolute filename
	i = ABSOLUTE_NAME_START;
	while (i < afLen && i < cdLen && currentDirectory[i] == absoluteFilename[i])
	{
		i++;
	}

	if (i == cdLen && (absoluteFilename[i] == '/' || absoluteFilename[i - 1] == '/'))
	{
		// the whole current directory name is in the file name,
		// so we just trim off the current directory name to get the
		// current file name.
		if (absoluteFilename[i] == '/')
		{
			// a directory name might have a trailing slash but a relative
			// file name should not have a leading one...
			i++;
		}

		STRCPY(relativeFilename, MAX_FILENAME_LEN, &absoluteFilename[i]);
		return relativeFilename;
	}


	// The file is not in a child directory of the current directory, so we
	// need to step back the appropriate number of parent directories by
	// using "..\"s.  First find out how many levels deeper we are than the
	// common directory
	afMarker = i;
	levels = 1;

	// count the number of directory levels we have to go up to get to the
	// common directory
	while (i < cdLen)
	{
		i++;
		if (currentDirectory[i] == '/')
		{
			// make sure it's not a trailing slash
			i++;
			if (currentDirectory[i] != '\0')
			{
				levels++;
			}
		}
	}

	// move the absolute filename marker back to the start of the directory name
	// that it has stopped in.
	while (afMarker > 0 && absoluteFilename[afMarker - 1] != '/')
	{
		afMarker--;
	}

	// check that the result will not be too long
	if (levels * 3 + afLen - afMarker > MAX_FILENAME_LEN)
	{
		return nullptr;
	}

	// add the appropriate number of "..\"s.
	rfMarker = 0;
	for (i = 0; i < levels; i++)
	{
		relativeFilename[rfMarker++] = '.';
		relativeFilename[rfMarker++] = '.';
		relativeFilename[rfMarker++] = '/';
	}

	// copy the rest of the filename into the result string
	STRCPY(&relativeFilename[rfMarker], MAX_FILENAME_LEN, &absoluteFilename[afMarker]);

	return relativeFilename;
}

Path Path::GetParentFolder() const
{
	std_string parentFolder = m_Folder;
	parentFolder.erase(parentFolder.length() - 1, 1);

	s32 lastSlashPos = static_cast<s32>(parentFolder.find_last_of('/'));

#ifdef _WIN32
	if (lastSlashPos == -1)
	{
		parentFolder += L("/");
		return parentFolder;
	}
#else
	if (lastSlashPos == 0 && parentFolder[0] == '/')
	{
		return Path(L("/"));
	}
	if (lastSlashPos <= 0 && parentFolder[lastSlashPos - 1] == '/')
	{
		// We can't remove anymore more folders
		return Path(L(""));
	}
#endif

	lastSlashPos++;
	parentFolder.erase(lastSlashPos, parentFolder.length() - lastSlashPos);
	return parentFolder;
}
