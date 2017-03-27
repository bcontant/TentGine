#pragma once

#include "XMLDefines.h"

class XMLTag;
class Path;
class XMLParser;

class XMLFile
{
private:
    XMLFile(const XMLFile&);
    XMLFile operator= (const XMLFile&);

public:
    XMLFile();
    ~XMLFile();

	bool Parse(const Path& in_strFilename);
	bool Parse(const std_string& in_Buffer);
    bool Parse(string_char* in_pBuffer, u32 in_i32BufferSize);

    bool Output(const Path& in_strFilename, const u32 in_u32OutputFlags) const;

    XMLTag* GetRoot();
    void SetRoot(XMLTag* in_pRoot);

    float GetVersion() const;

    const std_string& GetLastError() const;
    const std_string& GetLastErrorLine() const;

private:

    bool ValidateOutputFlags(const u32 in_u32OutputFlags) const;

    XMLParser *m_pXMLParser;
};
