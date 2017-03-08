#pragma once

#include "StringUtils.h"

//--------------------------------------------------------------------------------
class Path
{
public:
	Path();
	Path(const StringChar* in_path);
	Path(const StdString& in_path);
	Path(const Path& in_path);

	Path& operator=(const StringChar* in_path);
	Path& operator=(const StdString& in_path);
	Path& operator=(const Path& in_path);

	Path operator+(const Path& in_path) const;
	Path& operator+=(const Path& in_path);
	bool operator==(const Path& in_path) const;
	bool operator!=(const Path& in_path) const;
	bool operator<(const Path& in_path) const;

	const StdString& GetStdString() const { return m_strPath; }
	const StringChar* GetData() const { return m_strPath.c_str(); }

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

	StdString m_strPath;

	StdString m_Folder;
	StdString m_Name;
	StdString m_Extension;
};
