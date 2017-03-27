#include "precompiled.h"

#include "XMLAttribute.h"
#include "XMLDefines.h"
#include "XMLTag.h"

//--------------------------------------------------------------------------------
XMLTag::XMLTag() 
: m_eTagType(ETagType::eTT_NormalTag)
, m_strName(L(""))
, m_strContent(L(""))
, m_pChild(nullptr)
, m_pSibling(nullptr)
{
}

//--------------------------------------------------------------------------------
XMLTag::~XMLTag()
{
    for(u32 i = 0; i < m_vAttributes.size(); i++)
        delete m_vAttributes[i];
    m_vAttributes.clear();

    delete m_pChild;
    m_pChild = nullptr;

    delete m_pSibling;
    m_pSibling = nullptr;
}

//--------------------------------------------------------------------------------
/*
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, vec3& out_Value) const
{
    const XMLAttribute* pAttribute = nullptr;

    for(u32 i = 0; i < 3; i++)
    {
        std_string attributeName = Format("%s_v%d", in_strName.c_str(), i);
        pAttribute = FindAttribute(attributeName);

        if(pAttribute == nullptr)
            return nullptr;
    }

    for(u32 i = 0; i < 3; i++)
    {
        std_string attributeName = Format("%s_v%d", in_strName.c_str(), i);
        pAttribute = FindAttribute(attributeName);
        out_Value[i] = static_cast<float>(atof(pAttribute->GetValue().c_str()));
    }

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, mat3& out_Value) const
{
    const XMLAttribute* pAttribute = nullptr;

    for(u32 i = 0; i < 3; i++)
    {
        for(u32 j = 0; j < 3; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            pAttribute = FindAttribute(attributeName);

            if(pAttribute == nullptr)
                return nullptr;
        }
    }

    for(u32 i = 0; i < 3; i++)
    {
        for(u32 j = 0; j < 3; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            const XMLAttribute* pAttribute = FindAttribute(attributeName);
            out_Value.m_Matrix[i][j] = static_cast<float>(atof(pAttribute->GetValue().c_str()));
        }
    }

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, mat4& out_Value) const
{
    const XMLAttribute* pAttribute = nullptr;

    for(u32 i = 0; i < 4; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            pAttribute = FindAttribute(attributeName);

            if(pAttribute == nullptr)
                return nullptr;
        }
    }

    for(u32 i = 0; i < 4; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            const XMLAttribute* pAttribute = FindAttribute(attributeName);
            out_Value.m_Matrix[i][j] = static_cast<float>(atof(pAttribute->GetValue().c_str()));
        }
    }

    return pAttribute;
}*/

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, Path& out_strValue) const
{
	PROFILE_BLOCK;

    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_strValue = pAttribute->GetValue();

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, std_string& out_strValue) const
{
	PROFILE_BLOCK;

    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_strValue = pAttribute->GetValue();

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, u8& out_u8Value) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_u8Value = static_cast<u8>(ATOI32(pAttribute->GetValue().c_str()));

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, u32& out_u32Value) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_u32Value = ATOI32(pAttribute->GetValue().c_str());

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, s64& out_i64Value) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_i64Value = ATOI64(pAttribute->GetValue().c_str());

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, int& out_i32Value) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_i32Value = ATOI32(pAttribute->GetValue().c_str());

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, float& out_fValue) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    out_fValue = static_cast<float>(ATOF(pAttribute->GetValue().c_str()));

    return pAttribute;
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::GetAttribute(const std_string& in_strName, bool& out_bValue) const
{
    const XMLAttribute* pAttribute = FindAttribute(in_strName);

    if(pAttribute == nullptr)
        return nullptr;

    if(pAttribute->GetValue() == L("false") || pAttribute->GetValue() == L("FALSE") ||
        pAttribute->GetValue() == L("False") || pAttribute->GetValue() == L("0"))
    {
        out_bValue = false;
    }
    else
    {
        out_bValue = true;
    }

    return pAttribute;
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(XMLAttribute* in_pAttribute)
{
    m_vAttributes.push_back(in_pAttribute);
    return true;
}

//--------------------------------------------------------------------------------
/*
bool XMLTag::AddAttribute(const std_string& in_strName, const vec3& in_Value)
{
    for(u32 i = 0; i < 3; i++)
    {
        std_string attributeName = Format("%s_v%d", in_strName.c_str(), i);
        if(FindAttribute(attributeName) != nullptr)
            return false;
    }

    for(u32 i = 0; i < 3; i++)
    {
        std_string attributeName = Format("%s_v%d", in_strName.c_str(), i);
       
        string_char value[64];
        SPRINTF(value, 64, "%f", in_Value[i]);

        XMLAttribute* attribute = new XMLAttribute;
        attribute->SetName(attributeName);
        attribute->SetValue(value);
        m_vAttributes.push_back(attribute);
    }

    return true;
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const mat3& in_Value)
{
    for(u32 i = 0; i < 3; i++)
    {
        for(u32 j = 0; j < 3; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            if(FindAttribute(attributeName) != nullptr)
                return false;
        }
    }

    for(u32 i = 0; i < 3; i++)
    {
        for(u32 j = 0; j < 3; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            
            string_char value[64];
            SPRINTF(value, 64, "%f", in_Value.m_Matrix[i][j]);

            XMLAttribute* attribute = new XMLAttribute;
            attribute->SetName(attributeName);
            attribute->SetValue(value);
            m_vAttributes.push_back(attribute);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const mat4& in_Value)
{
    for(u32 i = 0; i < 4; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);
            if(FindAttribute(attributeName) != nullptr)
                return false;
        }
    }

    for(u32 i = 0; i < 4; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            std_string attributeName = Format("%s_m%d%d", in_strName.c_str(), i, j);

            string_char value[64];
            SPRINTF(value, 64, "%f", in_Value.m_Matrix[i][j]);

            XMLAttribute* attribute = new XMLAttribute;
            attribute->SetName(attributeName);
            attribute->SetValue(value);
            m_vAttributes.push_back(attribute);
        }
    }

    return true;
}*/

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const Path& in_strValue)
{
    if(FindAttribute(in_strName) != nullptr)
        return false;

    XMLAttribute* attribute = new XMLAttribute;
    attribute->SetName(in_strName);
    attribute->SetValue(in_strValue.GetString());
    m_vAttributes.push_back(attribute);
    return true;
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const std_string& in_strValue)
{
    if(FindAttribute(in_strName) != nullptr)
        return false;

    XMLAttribute* attribute = new XMLAttribute;
    attribute->SetName(in_strName);
    attribute->SetValue(in_strValue);
    m_vAttributes.push_back(attribute);
    return true;
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const u8 in_u8Value)
{
    string_char value[64];
    I32TOA(in_u8Value, value, 64);

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const u32 in_u32Value)
{
	string_char value[64];
    I32TOA(in_u32Value, value, 64);

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, s64 in_i64Value)
{
	string_char value[64];
    I64TOA(in_i64Value, value, 64);

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const int in_i32Value)
{
	string_char value[64];
    I32TOA(in_i32Value, value, 64);

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const float in_fValue)
{
	string_char value[64];
    SPRINTF(value, 64, L("%f"), in_fValue);

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
bool XMLTag::AddAttribute(const std_string& in_strName, const bool in_bValue)
{
	string_char value[64];

    if(in_bValue)
        SPRINTF(value, 64, L("true"));
    else
        SPRINTF(value, 64, L("false"));

    return AddAttribute(in_strName, std_string(value));
}

//--------------------------------------------------------------------------------
const XMLAttribute* XMLTag::FindAttribute(const std_string& in_strName) const
{
	PROFILE_BLOCK;

    for(u32 i = 0; i < m_vAttributes.size(); i++)
    {
        if(m_vAttributes[i]->GetName() == in_strName)
            return m_vAttributes[i];
    }
    return nullptr;
}

//--------------------------------------------------------------------------------
XMLTag* XMLTag::GetChild() const
{
    return m_pChild;
}

//--------------------------------------------------------------------------------
void XMLTag::AddChild(XMLTag* in_pChild)
{
    AssertMsg( m_eTagType == ETagType::eTT_NormalTag, L("Cannot add child to non-Normal tag") );

    if(m_pChild == nullptr)
        m_pChild = in_pChild;
    else
        m_pChild->AddSibling(in_pChild);
}

//--------------------------------------------------------------------------------
XMLTag* XMLTag::GetSibling()
{
    return m_pSibling;
}

//--------------------------------------------------------------------------------
void XMLTag::AddSibling(XMLTag* in_pSibling)
{
    if(m_pSibling == nullptr)
        m_pSibling = in_pSibling;
    else
        m_pSibling->AddSibling(in_pSibling);
}

//--------------------------------------------------------------------------------
void XMLTag::SetContent(const std_string& in_strContent)
{
    AssertMsg(m_eTagType == ETagType::eTT_ContentTag || m_eTagType == ETagType::eTT_CDATATag || m_eTagType == ETagType::eTT_CommentTag || m_eTagType == ETagType::eTT_DOCTYPETag,
        L("To set content to a tag, this tag must be of type eTT_ContentType, eTT_CDATATag, eTT_DOCTYPETag or eTT_CommentTag") );
    m_strContent = in_strContent;
}

//--------------------------------------------------------------------------------
const std_string& XMLTag::GetContent() const
{
    if(m_eTagType == ETagType::eTT_NormalTag)
    {
        //Check if the child of the tag is a Content tag
        if(m_pChild)
        {
            if(m_pChild->GetTagType() == ETagType::eTT_ContentTag || m_pChild->GetTagType() == ETagType::eTT_CDATATag)
            {
                return m_pChild->GetContent();
            }
        }
    }

    return m_strContent;
}

//--------------------------------------------------------------------------------
void XMLTag::Output(File* in_pFile, const u32 in_u32OutputFlags, u32 in_u32Indent) const
{
    in_pFile->Print(L("%s"), TO_STRING(GetString(in_u32OutputFlags, in_u32Indent)).c_str());
}

//--------------------------------------------------------------------------------
std_string XMLTag::GetString(const u32 in_u32OutputFlags, u32 in_u32Indent) const
{
    u32 u32IndentIncrement = 1;
    std_string outputString = L("");

    if(in_u32OutputFlags & eOF_MinimizeFileSize && m_eTagType == ETagType::eTT_CommentTag)
    {
        //Add the tag's siblings.
        if(m_pSibling)
            outputString += m_pSibling->GetString(in_u32OutputFlags, in_u32Indent);

        return outputString;
    }

    if(m_eTagType == ETagType::eTT_ContentTag)
    {
        outputString += m_strContent;

        //Add the tag siblings
        if(m_pSibling)
            outputString += m_pSibling->GetString(in_u32OutputFlags, in_u32Indent);

        return outputString;
    }

    //Add the Indent.
    for(u32 i = 0; i < in_u32Indent; i++)  outputString += XMLUtils::GetIndentString(in_u32OutputFlags);

    //Add the Opening Bracket
    outputString += L("<");

    if(m_eTagType == ETagType::eTT_CommentTag)
        outputString += L("!--");
    else if(m_eTagType == ETagType::eTT_XMLIDTag)
        outputString += L("?");

    //Add the tag name.
    outputString += m_strName;

    //Add the attributes.
    for(u32 i = 0; i < m_vAttributes.size(); i++)
    {
        outputString += m_vAttributes[i]->GetString(in_u32OutputFlags, in_u32Indent+u32IndentIncrement);
    }

    //Add special closing characters
    if(m_eTagType == ETagType::eTT_CommentTag)
    {
        outputString += m_strContent;
        outputString +=  L("--");
    }
    else if(m_eTagType == ETagType::eTT_XMLIDTag)
        outputString += L("?");
    else if(m_eTagType == ETagType::eTT_SelfCloseTag)
        outputString += L("/");

    //Add the closing bracket.
    if(in_u32OutputFlags & eOF_MinimizeFileSize)
        outputString += L(">");
    else
        outputString += L(">\n");


    //Add the tag contents
    if(m_eTagType != ETagType::eTT_CommentTag && m_strContent.size() > 0)
    {
        //Add the Indent.
        for(u32 i = 0; i < in_u32Indent+1; i++) outputString += XMLUtils::GetIndentString(in_u32OutputFlags);

        if(in_u32OutputFlags & eOF_MinimizeFileSize)
            outputString += m_strContent;
        else
            outputString += m_strContent + L("\n");
    }

    //Add the tag's children.
    if(m_pChild)
        outputString += m_pChild->GetString(in_u32OutputFlags, in_u32Indent+u32IndentIncrement);

    //Add the closing tag
    if(m_eTagType == ETagType::eTT_NormalTag)
    {
        //Add the Indent.
        for(u32 i = 0; i < in_u32Indent; i++) outputString += XMLUtils::GetIndentString(in_u32OutputFlags);

        //Add the closing tag.
        if(in_u32OutputFlags & eOF_MinimizeFileSize)
            outputString += L("</") + m_strName + L(">");
        else
            outputString += L("</") + m_strName + L(">\n");
    }

    //Add the tag's siblings.
    if(m_pSibling)
        outputString += m_pSibling->GetString(in_u32OutputFlags, in_u32Indent);

    return outputString;
}

//--------------------------------------------------------------------------------
XMLTag* XMLTag::FirstChildNamed(const std_string& tagName, bool in_bRecurse) const
{
	PROFILE_BLOCK;

    XMLTag* tagFound = nullptr;

    if(m_pChild)
    {
        if(m_pChild->m_strName.compare(tagName) == 0)
            return m_pChild;

		if (in_bRecurse)
		{
			tagFound = m_pChild->FirstChildNamed(tagName, in_bRecurse);
			if (tagFound)
				return tagFound;
		}

        XMLTag* pSibling = m_pChild->m_pSibling;
        while(pSibling)
        {
            if(pSibling->m_strName.compare(tagName) == 0)
                return pSibling;

			if (in_bRecurse)
			{
				tagFound = pSibling->FirstChildNamed(tagName, in_bRecurse);
				if (tagFound)
					return tagFound;
			}

            pSibling = pSibling->GetSibling();
        }
    }

    return tagFound;
}

//--------------------------------------------------------------------------------
XMLTag* XMLTag::FirstSiblingNamed(const std_string& tagName, bool in_bRecurse) const
{
    XMLTag* tagFound = nullptr;

    if(m_pSibling)
    {
        if(m_pSibling->m_strName.compare(tagName) == 0)
            return m_pSibling;

		if (in_bRecurse)
		{
			tagFound = m_pSibling->FirstChildNamed(tagName, in_bRecurse);
			if (tagFound)
				return tagFound;
		}

        m_pSibling->FirstSiblingNamed(tagName, in_bRecurse);
    }

    return tagFound;
}

#if DEBUG_XML_PARSER
//--------------------------------------------------------------------------------
void XMLTag::Print(int indent)
{
    string_char indentStr[128];

    indentStr[0] = 0;
    for(int i = 0; i < indent; i++)
        indentStr[i] = ' ';
    indentStr[indent] = 0;

    Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::FileAndDebug, L("%sTagName : %s  "), indentStr, m_strName.c_str());

    if(m_strContent != L(""))
    {
		Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::FileAndDebug, L("Contents : %s"), m_strContent.c_str());
    }

    for(u32 k = 0; k < m_vAttributes.size(); k++)
    {
        indentStr[0] = 0;
        for(int i = 0; i < indent+1; i++)
            indentStr[i] = ' ';
        indentStr[indent+1] = 0;

		Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::FileAndDebug, L("%s%s : %s"), indentStr, m_vAttributes[k]->GetName().c_str(), m_vAttributes[k]->GetValue().c_str());
    }

    if(m_pChild)
        m_pChild->Print(indent+2);

    if(m_pSibling)
        m_pSibling->Print(indent);
}

#endif
