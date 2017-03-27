#pragma once

#include "StringUtils.h"

//--------------------------------------------------------------------------------
class Path
{
public:
	Path();
	Path(const string_char* in_path);
	Path(const std_string& in_path);
	Path(const Path& in_path);

	Path& operator=(const string_char* in_path);
	Path& operator=(const std_string& in_path);
	Path& operator=(const Path& in_path);

	Path operator+(const Path& in_path) const;
	//Path operator+(const string_char* in_path) const { return this->operator+(Path(in_path)); }
	Path& operator+=(const Path& in_path);
	bool operator==(const Path& in_path) const;
	bool operator!=(const Path& in_path) const;
	bool operator<(const Path& in_path) const;

	const std_string& GetString() const { return m_strPath; }
	const string_char* GetData() const { return m_strPath.c_str(); }

	Path GetFolder() const { return Path(m_Folder); }
	Path GetName() const { return Path(m_Name); }
	Path GetExtension() const { return Path(m_Extension); }
	Path GetFileName() const { return Path(m_Name + m_Extension); }

	Path& SetFolder(const Path& in_Folder);
	Path& SetName(const Path& in_Name);
	Path& SetExtension(const Path& in_Extension);

	Path GetRelativePath(const Path& in_WorkingDirectory) const;
	Path GetParentFolder() const;

private:
	void BuildPath();
	void CleanUpPath();

	std_string m_strPath;

	std_string m_Folder;
	std_string m_Name;
	std_string m_Extension;
};

//Path operator+(const string_char* in_Path1, const Path& in_path2) { return Path(in_Path1) + in_path2; }