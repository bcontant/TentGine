#pragma once

#include "StringUtils.h"

class XMLAttribute;
class File;
class Path;

class XMLTag
{
public:
    enum class ETagType
    {
        eTT_NormalTag,
        eTT_SelfCloseTag,
        eTT_CommentTag,
        eTT_XMLIDTag,
        eTT_CDATATag,
        eTT_DOCTYPETag,
        eTT_ContentTag
    };

private:
    XMLTag(const XMLTag&);
    XMLTag operator= (const XMLTag&);

public:
    XMLTag();
    ~XMLTag();

    //Helper methods to get attributes from a tag.  Returns nullptr if no
    //attribute with the specified name is found.
    const XMLAttribute* GetAttribute(const std_string& in_strName, Path& out_strValue) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, std_string& out_strValue) const;
	const XMLAttribute* GetAttribute(const std_string& in_strName, s8& out_u8Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, u8& out_u8Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, u32& out_u32Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, s64& out_i64Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, s32& out_i32Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, float& out_fValue) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, bool& out_bValue) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, vec3& out_Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, mat3& out_Value) const;
    const XMLAttribute* GetAttribute(const std_string& in_strName, mat4& out_Value) const;

    //Helper methods to add attributes into a tag.  Fails if an attribute of the same
    //name is already present.
    bool AddAttribute(const std_string& in_strName, const Path& in_strValue);
    bool AddAttribute(const std_string& in_strName, const std_string& in_strValue);
	bool AddAttribute(const std_string& in_strName, const s8 in_u8Value);
    bool AddAttribute(const std_string& in_strName, const u8 in_u8Value);
    bool AddAttribute(const std_string& in_strName, const u32 in_u32Value);
    bool AddAttribute(const std_string& in_strName, s64 in_i64Value);
    bool AddAttribute(const std_string& in_strName, const s32 in_i32Value);
    bool AddAttribute(const std_string& in_strName, const float in_fValue);
    bool AddAttribute(const std_string& in_strName, const bool in_bValue);
    bool AddAttribute(const std_string& in_strName, const vec3& in_Value);
    bool AddAttribute(const std_string& in_strName, const mat3& in_Value);
    bool AddAttribute(const std_string& in_strName, const mat4& in_Value);

    //This method should only be used by the parser
    bool AddAttribute(XMLAttribute* in_pAttribute);

    XMLTag* GetChild() const;
    void AddChild(XMLTag* in_pChild);

    XMLTag* GetSibling();
    void AddSibling(XMLTag* in_pSibling);

    //Name accessors
    void SetName(const std_string& in_strName) { m_strName = in_strName; }
    const std_string& GetName() { return m_strName; }

    //Sets the contents of a tag
    void SetContent(const std_string& in_strContent);
    const std_string& GetContent() const;

    void SetTagType(ETagType in_eType) { m_eTagType = in_eType; }
    ETagType GetTagType() { return m_eTagType; }

    //Outputs the tag into the debugger
    void Print(s32 indent=0);
    void Output(File* in_pFile, const u32 in_u32OutputFlags, u32 in_u32Indent) const;
    std_string GetString(const u32 in_u32OutputFlags, u32 in_u32Indent) const;

    //Tag name search
    XMLTag* FirstChildNamed(const std_string& tagName, bool in_bRecurse) const;
    XMLTag* FirstSiblingNamed(const std_string& tagName) const;

private:
    //Find an attribute within the tag and returns it.  Returns nullptr if no
    //attribute with the specified name is found.
    const XMLAttribute* FindAttribute(const std_string& in_strName) const;

private:

    //The type of the tag (comment or regular)
    ETagType m_eTagType;

    //The name of the XML Tag
    std_string m_strName;

    //The contents of the tag (Text between the <> and </> for regular tags,
    //Text inside the tag for comments)
    std_string m_strContent;

    //Attribute for the tag
    std::vector<XMLAttribute*> m_vAttributes;

    //Pointer to the first childer of the tag
    XMLTag* m_pChild;

    //Pointer to the first sibling of the tag
    XMLTag* m_pSibling;
};
