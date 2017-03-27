#pragma once

class File;

class XMLAttribute
{
public:
    XMLAttribute() {};
    ~XMLAttribute() {};

    void SetName(const std_string& in_strName) { m_strName = in_strName; }
    void SetName(std_string& in_pcName) { m_strName = in_pcName; }

    void SetValue(const std_string& in_strValue) { m_strValue = in_strValue; }
    void SetValue(std_string& in_pcValue) { m_strValue = in_pcValue; }

    const std_string& GetValue() const { return m_strValue; }
    const std_string& GetName()  const { return m_strName; }

    void Print(File* in_pFile, const u32 in_u32OutputFlags, u32 in_u32Indent);
	std_string GetString(const u32 in_u32OutputFlags, u32 in_u32Indent);

private:
	std_string m_strName;
	std_string m_strValue;
};