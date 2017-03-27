#pragma once

#include "StringUtils.h"

#define XML_OPEN_BRACKET '<'
#define XML_CLOSE_BRACKET '>'
#define XML_PARAM_ASSIGNMENT '='
#define XML_TAG_CLOSE '/'
#define XML_VERSION_TAG_IDENTIFIER '?'
#define XML_COMMENT_STARTER '!'
#define XML_PARAM_DELIMITER '"'
#define XML_COMMENT_DELIMITER '-'

#ifdef _DEBUG
#define DEBUG_XML_PARSER    1
#endif

enum EXMLOutputFlags
{
    //Indent Group.  Mutually exclusive flags.
    eOF_TabIndents_1            = 1 << 0,
    eOF_TabIndents_2            = 1 << 1,
    eOF_SpaceIndents_2          = 1 << 2,
    eOF_SpaceIndents_4          = 1 << 3,

    //Change linefor tag parameters. Mutually exclusive flags.
    eOF_NewLineForAnyParam      = 1 << 4,
    eOF_NewLineFor2PlusParams   = 1 << 5,

    //Minimize file size.  Mutually exclusive with ALL flags.
    eOF_MinimizeFileSize        = 1 << 6,

    eOF_Force32Bits             = 0xffffffff
};

namespace XMLUtils
{
    std_string GetIndentString(const u32 in_u32OutputFlags);
    //const TSpecialCharsMap& GetXmlSpecialChars();
};
