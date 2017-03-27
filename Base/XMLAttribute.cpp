#include "precompiled.h"

#include "XMLAttribute.h"
#include "XMLDefines.h"

void XMLAttribute::Print(File* in_pFile, const u32 in_u32OutputFlags, u32 in_u32Indent)
{
    in_pFile->Print(L("%s"), GetString(in_u32OutputFlags, in_u32Indent).c_str());
}

std_string XMLAttribute::GetString(const u32 in_u32OutputFlags, u32 in_u32Indent)
{
    std_string outputString = L("");

    if(in_u32OutputFlags & eOF_NewLineForAnyParam)
    {
        //Add the new line to the string
        outputString += L("\n");

        //Add indentation
        for(u32 i = 0; i < in_u32Indent; i++)
        {
            outputString += XMLUtils::GetIndentString(in_u32OutputFlags);
        }
    }
    else
    {
        //Add a space
        outputString += L(" ");
    }

    outputString += m_strName + L("=\"") + m_strValue + L("\"");

    return outputString;
}
