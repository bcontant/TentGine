#include "precompiled.h"

#include "XMLDefines.h"

std_string XMLUtils::GetIndentString(const u32 in_u32OutputFlags)
{
    if(in_u32OutputFlags & eOF_TabIndents_1)
        return std_string(L("\t"));

    if(in_u32OutputFlags & eOF_TabIndents_2)
        return std_string(L("\t\t"));

    if(in_u32OutputFlags & eOF_SpaceIndents_2)
        return std_string(L("  "));

    if( in_u32OutputFlags & eOF_SpaceIndents_4)
        return std_string(L("    "));

    if( in_u32OutputFlags & eOF_MinimizeFileSize)
        return std_string(L(""));

    return std_string(L(" "));
}

/*

const TSpecialCharsMap& XMLUtils::GetXmlSpecialChars()
{
    static bool bMapInitialized = false;
    static TSpecialCharsMap XmlSpecialChars;

    if( !bMapInitialized )
    {
        bMapInitialized = true;
        XmlSpecialChars.insert( std::pair<char, std_string>('\"', "&quot;") );
        XmlSpecialChars.insert( std::pair<char, std_string>('&', "&amp;") );
        XmlSpecialChars.insert( std::pair<char, std_string>('\'', "&apos;") );
        XmlSpecialChars.insert( std::pair<char, std_string>('<', "&lt;") );
        XmlSpecialChars.insert( std::pair<char, std_string>('>', "&gt;") );
    }

    return XmlSpecialChars;
}
*/
