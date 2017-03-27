#include "precompiled.h"

#include "XMLFile.h"
#include "XMLParser.h"
#include "XMLTag.h"

#include "MemoryBuffer.h"

XMLFile::XMLFile()
{
    m_pXMLParser = new XMLParser();
}

XMLFile::~XMLFile()
{
    delete m_pXMLParser;
}

bool XMLFile::Parse(const Path& in_strFilename)
{
	PROFILE_BLOCK;

    File file;
    if(!file.Open(in_strFilename, FileMode::ReadOnly))
        return false;

    u32 u32FileBufferSize = file.Size();
    char* pFileBuffer = new char[u32FileBufferSize];
    if(pFileBuffer == nullptr)
        return false;

    file.Read(pFileBuffer, u32FileBufferSize);
    file.Close();

	std_string buffer = FROM_STRING( std::string(pFileBuffer, u32FileBufferSize) );
    m_pXMLParser->Parse(buffer);

    delete[] pFileBuffer;
    pFileBuffer = nullptr;

    return m_pXMLParser->GetXMLRoot() ? true : false;
}

bool XMLFile::Parse(const std_string& in_Buffer)
{
    m_pXMLParser->Parse(in_Buffer);

    return m_pXMLParser->GetXMLRoot() ? true : false;
}

bool XMLFile::Parse(string_char* in_pBuffer, u32 in_i32BufferSize)
{
    std_string buffer(in_pBuffer, in_i32BufferSize);
    m_pXMLParser->Parse(buffer);

    return m_pXMLParser->GetXMLRoot() ? true : false;
}

bool XMLFile::Output(const Path& in_strFilename, const u32 in_u32OutputFlags) const
{
    File file;
    if(!file.Open(in_strFilename, FileMode::WriteText))
        return false;

    if(!m_pXMLParser->GetXMLRoot())
        return false;

    if(m_pXMLParser->GetXMLVersion() != 0.0f)
    {
        XMLTag* pXMLIDTag = new XMLTag;
        pXMLIDTag->SetTagType(XMLTag::ETagType::eTT_XMLIDTag);
        pXMLIDTag->SetName(L("xml"));
        pXMLIDTag->AddAttribute(L("version"), m_pXMLParser->GetXMLVersion());
        pXMLIDTag->Output(&file, 0, 0);
        delete pXMLIDTag;
    }

    if(!ValidateOutputFlags(in_u32OutputFlags))
        return false;

    m_pXMLParser->GetXMLRoot()->Output(&file, in_u32OutputFlags, 0);

    file.Close();

    return true;
}

bool XMLFile::ValidateOutputFlags(const u32 in_u32OutputFlags) const
{
    bool bIndentFlagFound = false;
    bool bNewlineFlagFound = false;

    if(in_u32OutputFlags & eOF_TabIndents_1)
        bIndentFlagFound = true;

    if(in_u32OutputFlags & eOF_TabIndents_2)
    {
        if(bIndentFlagFound) return false;
        bIndentFlagFound = true;
    }

    if(in_u32OutputFlags & eOF_SpaceIndents_2)
    {
        if(bIndentFlagFound) return false;
        bIndentFlagFound = true;
    }

    if(in_u32OutputFlags & eOF_SpaceIndents_4)
    {
        if(bIndentFlagFound) return false;
        bIndentFlagFound = true;
    }

    if(in_u32OutputFlags & eOF_NewLineForAnyParam)
    {
        bNewlineFlagFound = true;
    }

    if(in_u32OutputFlags & eOF_NewLineFor2PlusParams)
    {
        if(bNewlineFlagFound) return false;
        bNewlineFlagFound = true;
    }

    if(in_u32OutputFlags & eOF_MinimizeFileSize)
    {
        if(in_u32OutputFlags != eOF_MinimizeFileSize) return false;
    }

    return true;
}

const std_string& XMLFile::GetLastError() const
{
    return m_pXMLParser->GetLastError();
}

const std_string& XMLFile::GetLastErrorLine() const
{
    return m_pXMLParser->GetLastErrorLine();
}

XMLTag* XMLFile::GetRoot()
{
    return m_pXMLParser->GetXMLRoot();
}

void XMLFile::SetRoot(XMLTag* in_pRoot)
{
    m_pXMLParser->SetXMLRoot(in_pRoot);
}

float XMLFile::GetVersion() const
{
    return m_pXMLParser->GetXMLVersion();
}
