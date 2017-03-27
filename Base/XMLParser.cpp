#include "precompiled.h"

#include "XMLParser.h"
#include "XMLDefines.h"
#include "XMLAttribute.h"
#include "XMLTag.h"

//Constants

//Tag Types names
#if DEBUG_XML_PARSER
const std_string XMLParser::sppcTagTypes[] =
{
    L("eTT_OpenBracket"),
	L("eTT_CloseBracket"),
	L("eTT_CloseTag"),
	L("eTT_XMLVersionID"),
	L("eTT_OpenCommentID"),
	L("eTT_CommentDelimiter"),
	L("eTT_ParamAssignment"),
	L("eTT_StringDelimiter"),
	L("eTT_String")
};
#endif

//Valid XML tag parameters
#define VALID_VERSION_PARAMETER_COUNT 2
const std_string ValidVersionParameters[VALID_VERSION_PARAMETER_COUNT] =
{
	L("version"),
	L("encoding")
};
const std_string XMLVersionTagName = L("xml");

//Error strings
const std_string strError_UnexpectedXMLIDFound = L("Unexpected XML ID Found (?).");
const std_string strError_UnexpectedEndOfFile = L("Unexpected end of file.");
const std_string strError_ExpectedOpenBracket = L("'<' expected.");
const std_string strError_ExpectedCloseBracket = L("'>' expected.");
const std_string strError_ExpectedCommentDelimiter = L("'--' expected.");
const std_string strError_ExpectedStringDelimiter = L("'\"' expected.");
const std_string strError_ExpectedParamAssignment = L("'=' expected.");
const std_string strError_ExpectedString = L("Character string expected.");
const std_string strError_UnexpectedCharacterSequence = L("Unexpected character sequence found.");
const std_string strError_MismatchingOpenCloseTag = L("Mismatching open and close tags found.");
const std_string strError_InvalidXMLTagName = L("'xml' expected. Invalid XML ID Tag Name.");
const std_string strError_UnexpectedCharacterInClosingTag = L("Unexpected character sequence found in closing tag.");
const std_string strError_UnexpectedCharacterAfterTagClose = L("Unexpected character sequence found after tag close.");
const std_string strError_InvalidXMLTagParameter = L("Invalid parameter found in XML Tag.");
const std_string strError_ExpectedXMLID = L("Expected XML ID '?'.");
const std_string strError_UnexpectedSlashFound = L("Unexpected '/'.");

#if DEBUG_XML_PARSER
void XMLParser::XMLToken::Print()
{
    string_char temp[1024];
    SPRINTF(temp, 1024, L("[%d]\t[TOKEN TYPE] = %s\t[TOKEN STRING] = %s"), m_u32StartPosition, sppcTagTypes[static_cast<int>(m_eTokenType)].c_str(), L("MISSING"));
    Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::FileAndDebug, temp);
}
#endif

void XMLParser::XMLToken::Reset()
{
    m_eTokenType = ETokenType::eTT_Invalid;
    m_u32StartPosition = 0;
}

XMLParser::XMLParser() :
m_pCurrentAttribute(0),
m_pXMLRoot(nullptr),
m_pXMLIDTag(nullptr),
m_strLastError(L("")),
m_strLastErrorLine(L("")),
m_u32OffsetInLine(0),
m_u32CurrentPosition(0),
m_eTokenizerState(ETokenizerState::eTS_Invalid),
m_TemporaryIDString(L("")),
m_bParsingString(false),
m_bIsCharWhitespace(false),
m_bStringIsOnlyWhitespace(true),
m_pcNextCharacter(0),
m_bTokenizerInterrupted(false),
m_bInVersionTag(false)
{
}

XMLParser::~XMLParser()
{
    Reset();
}

float XMLParser::GetXMLVersion()
{
    float fVersion = 0.0f;
    if(m_pXMLIDTag)
        m_pXMLIDTag->GetAttribute(ValidVersionParameters[0], fVersion);

    return fVersion;
}

void XMLParser::SetXMLRoot(XMLTag* in_pRoot)
{
    Reset();
    m_pXMLRoot = in_pRoot;
}

void XMLParser::Reset()
{
    m_u32CurrentPosition = 0;
    m_eTokenizerState = ETokenizerState::eTS_Invalid;
    m_bTokenizerInterrupted = false;
    m_bInVersionTag = false;
    m_bParsingString = false;
    m_bStringIsOnlyWhitespace = true;
    delete m_pXMLRoot;
    m_pXMLRoot = nullptr;
    m_pXMLIDTag = nullptr;
    m_strLastError = L("");
    m_strLastErrorLine = L("");
    m_u32OffsetInLine = 0;

    ResetTempIDString();

    m_CurrentToken.Reset();
    m_CurrentStringToken.Reset();

    m_TagList.clear();
}

XMLParser::XMLToken* XMLParser::GetNextToken(const std_string& in_buffer)
{
    while(m_u32CurrentPosition < in_buffer.size() || m_bTokenizerInterrupted)
    {
        if(!m_bTokenizerInterrupted)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_Invalid;

            m_pcNextCharacter = GetNextCharacter(in_buffer);

            if(m_eTokenizerState == ETokenizerState::eTS_InsideCommentPossibleDelimiter && m_pcNextCharacter != XML_COMMENT_DELIMITER)
            {
                m_eTokenizerState = ETokenizerState::eTS_InsideCommentContent;
            }

            LookForSingleCharacterTokens();

            if(m_bParsingString && CheckForStringTokenEnd())
            {
                if(!m_bStringIsOnlyWhitespace)
                {
                    m_bTokenizerInterrupted = true;
                    m_bStringIsOnlyWhitespace = true;
                    return &m_CurrentStringToken;
                }
                else
                {
                    m_bStringIsOnlyWhitespace = true;
                    ResetTempIDString();
                }
            }
        }
        else
        {
            ResetTempIDString();
        }
        m_bTokenizerInterrupted = false;

        if(m_bParsingString)
        {
            ContinueCurrentStringToken();
        }

        if(m_CurrentToken.m_eTokenType != ETokenType::eTT_Invalid)
        {
            m_CurrentToken.m_u32StartPosition = m_u32CurrentPosition-1;
            return &m_CurrentToken;
        }
    }

    return nullptr;
}
void XMLParser::LookForSingleCharacterTokens()
{
    m_bIsCharWhitespace = false;

    if(m_bParsingString && m_eTokenizerState == ETokenizerState::eTS_InsideParamValue)
    {
        if(m_pcNextCharacter == XML_PARAM_DELIMITER)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_StringDelimiter;
            m_eTokenizerState = ETokenizerState::eTS_InsideTag;
        }
        return;
    }

    //Look for single character tokens
    switch(m_pcNextCharacter)
    {
    case XML_OPEN_BRACKET:
        m_CurrentToken.m_eTokenType = ETokenType::eTT_OpenBracket;
        m_eTokenizerState = ETokenizerState::eTS_InsideTag;
        break;
    case XML_CLOSE_BRACKET:
        m_CurrentToken.m_eTokenType = ETokenType::eTT_CloseBracket;
        m_eTokenizerState = ETokenizerState::eTS_Content;
        m_bParsingString = true;
        break;
    case XML_PARAM_ASSIGNMENT:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideTag)
            m_CurrentToken.m_eTokenType = ETokenType::eTT_ParamAssignment;
        break;
    case XML_TAG_CLOSE:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideTag)
            m_CurrentToken.m_eTokenType = ETokenType::eTT_CloseTag;
        break;
    case XML_VERSION_TAG_IDENTIFIER:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideTag)
            m_CurrentToken.m_eTokenType = ETokenType::eTT_XMLVersionID;
        break;
    case XML_COMMENT_STARTER:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideTag)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_OpenCommentID;
            m_eTokenizerState = ETokenizerState::eTS_InsideCommentTag;
        }
        break;
    case XML_PARAM_DELIMITER:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideTag)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_StringDelimiter;
            m_eTokenizerState = ETokenizerState::eTS_InsideParamValue;
            m_bParsingString = true;
        }
        else if(m_eTokenizerState == ETokenizerState::eTS_InsideParamValue)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_StringDelimiter;
            m_eTokenizerState = ETokenizerState::eTS_InsideTag;
        }
        break;
    case XML_COMMENT_DELIMITER:
        if(m_eTokenizerState == ETokenizerState::eTS_InsideCommentTag || m_eTokenizerState == ETokenizerState::eTS_InsideCommentContent)
        {
            m_eTokenizerState = ETokenizerState::eTS_InsideCommentPossibleDelimiter;
        }
        else if(m_eTokenizerState == ETokenizerState::eTS_InsideCommentPossibleDelimiter)
        {
            m_CurrentToken.m_eTokenType = ETokenType::eTT_CommentDelimiter;
            m_eTokenizerState = ETokenizerState::eTS_InsideCommentContent;
            m_bParsingString = true;

            //Erase the last character in the current string, because it is the last '-' we caught
            if(m_TemporaryIDString.size() > 0)
                m_TemporaryIDString.erase(m_TemporaryIDString.size()-1, 1);
        }
        break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        m_bIsCharWhitespace = true;
        break;
    default:
        m_bParsingString = true;
        break;
    }
}

bool XMLParser::CheckForStringTokenEnd()
{

    if(m_CurrentToken.m_eTokenType != ETokenType::eTT_Invalid && m_TemporaryIDString.size() != 0)
    {
        m_bParsingString = false;
    }

    //If the current character is whitespace, and we're not reading content, stop reading
    if( m_bIsCharWhitespace &&
        m_TemporaryIDString.size() != 0 &&
        m_eTokenizerState != ETokenizerState::eTS_Content &&
        m_eTokenizerState != ETokenizerState::eTS_InsideCommentContent &&
        m_eTokenizerState != ETokenizerState::eTS_InsideParamValue)
    {
        m_bParsingString = false;
    }

    return !m_bParsingString;
}

void XMLParser::ContinueCurrentStringToken()
{
    if(m_CurrentToken.m_eTokenType == ETokenType::eTT_Invalid)
    {
        if(m_TemporaryIDString.size() == 0)
        {
            m_CurrentStringToken.m_u32StartPosition = m_u32CurrentPosition-1;
        }

        if(!m_bIsCharWhitespace)
            m_bStringIsOnlyWhitespace = false;

        m_TemporaryIDString += m_pcNextCharacter;
    }
}

XMLTag* XMLParser::Parse(const std_string& in_buffer)
{
    Reset();

    #if DEBUG_XML_PARSER
        s64 timeStart = OS::GetTickCount();
    #endif

    XMLToken* pCurrentToken = nullptr;
    XMLTag* pCurrentTag = nullptr;

    m_CurrentStringToken.m_eTokenType = ETokenType::eTT_String;
    m_eTokenizerState = ETokenizerState::eTS_Content;

    EParserState parsingState = EParserState::ePS_NewTag;
    bool isSelfClosedTag = false;
    bool inClosingTag = false;

	pCurrentToken = GetNextToken(in_buffer);
    while( pCurrentToken )
    {
        switch(parsingState)
        {

        case EParserState::ePS_NewTag:
            //Make sure a new tag starts with an open bracket
            if(!CheckTokenForType(in_buffer, pCurrentToken, ETokenType::eTT_OpenBracket))
                return nullptr;

            //We're now parsing the tag name
            parsingState = EParserState::ePS_TagName;
            break;

        case EParserState::ePS_TagName:
            switch(pCurrentToken->m_eTokenType)
            {
            case ETokenType::eTT_CloseTag:
                //If we hit a '/' while looking for a tag name, it means we're in a closing tag.
                inClosingTag = true;
                break;

            case ETokenType::eTT_OpenCommentID:

                //If we hit a '!' while looking for a tag name, it means this should be a comment tag.
                //We'll parse and validate the entire tag in here.
                pCurrentTag = new XMLTag;

                //Make sure the next token is '--'
                if( CheckTokenForCData(in_buffer) )
                {
                    // This is a CDATA tag
                    pCurrentTag->SetTagType(XMLTag::ETagType::eTT_CDATATag);

                    // Set the content of the comment
                    pCurrentTag->SetContent(m_TemporaryIDString);

                    // Done with that
                    ResetTempIDString();
                }
                else if(CheckTokenForDoctype(in_buffer))
                {
                    //This is a DOCTYPE tag
                    pCurrentTag->SetTagType(XMLTag::ETagType::eTT_DOCTYPETag);

                    //Set the content of the comment
                    pCurrentTag->SetContent(m_TemporaryIDString);

                    //Done with that
                    ResetTempIDString();
                }
                else if( CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_CommentDelimiter) )
                {
                    // This is a comment tag
                    pCurrentTag->SetTagType(XMLTag::ETagType::eTT_CommentTag);

                    //Make sure the next token is a string (the actual comment)
                    pCurrentToken = GetNextToken(in_buffer);
                    if(!CheckTokenForType(in_buffer, pCurrentToken, ETokenType::eTT_String))
                        return nullptr;

                    // Set the content of the comment
                    pCurrentTag->SetContent(m_TemporaryIDString);

                    //Make sure the next token is '--'
                    if(!CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_CommentDelimiter))
                        return nullptr;

                    //Make sure the comment tag is closed by a '>'
                    if(!CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_CloseBracket))
                        return nullptr;
                }
                else
                {
                    return nullptr;
                }

                //Add the tag to our file, but don't push it on the stack, since comment tags are "auto-closed"
                //and cannot have children.
                AddTag(pCurrentTag, false);

                //Start looking for a new tag, because comment tags can't have content.
                parsingState = EParserState::ePS_NewTag;

                break;

            case ETokenType::eTT_XMLVersionID:
                //If we hit a '?' while looking for the tag name, it means we're in the XML tag.
                m_bInVersionTag = true;

                m_pXMLIDTag = new XMLTag;
                m_pXMLIDTag->SetTagType(XMLTag::ETagType::eTT_XMLIDTag);

                break;

            case ETokenType::eTT_String:
                //We found our tag name.

                //If we're in the version tag, we can set the tag's name.  The tag had been created already.
                if(m_bInVersionTag)
                {
                    if(m_TemporaryIDString != XMLVersionTagName)
                    {
                        Error(in_buffer, strError_InvalidXMLTagName, pCurrentToken);
                        return nullptr;
                    }

                    m_pXMLIDTag->SetName(m_TemporaryIDString);
                }

                //If we're not in a closing tag, we need to create a new tag
                //and set its name.
                else if(!inClosingTag)
                {
                    pCurrentTag = new XMLTag;
                    pCurrentTag->SetTagType(XMLTag::ETagType::eTT_NormalTag);
                    pCurrentTag->SetName(m_TemporaryIDString);
                }
                //If we're in a closing tag, make sure its name matches the current top tag on the stack
                else
                {
                    //If the name doesn't match or we don't have any tags on the stack, we hit a mismatching tag
                    //error.
                    if(m_TagList.size() == 0 || m_TagList.front()->GetName()!= m_TemporaryIDString)
                    {
                        Error(in_buffer, strError_MismatchingOpenCloseTag, pCurrentToken);
                        return nullptr;
                    }
                }

                //Now that we have the name, we're parsing the tag's attributes.
                parsingState = EParserState::ePS_Attribute;

                break;

            default:
                //If we find anything else while looking for a tag name, the XML file is invalid.
                Error(in_buffer, strError_UnexpectedCharacterSequence, pCurrentToken);
                return nullptr;
                break;
            }

            break;

        case EParserState::ePS_Attribute:
            switch(pCurrentToken->m_eTokenType)
            {
            case ETokenType::eTT_XMLVersionID:
                //If we found a '?' while looking for attribute, it means this is the end of the XML tag.

                //If we weren't in the XML Tag, this is an invalid character
                if(!m_bInVersionTag)
                {
                    Error(in_buffer, strError_UnexpectedXMLIDFound, pCurrentToken);
                    return nullptr;
                }

                //Make sure that the '?' is followed by '>'
                if(!CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_CloseBracket))
                    return nullptr;

                //We're not in the version tag anymore.
                m_bInVersionTag = false;

                //Start looking for a new tag.
                parsingState = EParserState::ePS_NewTag;
                break;

            case ETokenType::eTT_String:
                //If we found a string, that means we got an attribute.

                //Make sure we're not in a closing tag.  Closing tags are not allowed to have attributes.
                if(inClosingTag)
                {
                    Error(in_buffer, strError_UnexpectedCharacterInClosingTag, pCurrentToken);
                    return nullptr;
                }

                //Make sure we're not in a self-closed tag.  They're allowed to have attributes, but if the flag
                //is set, that means we already found the closing '/', and we're not allowed to have attributes
                //after this.
                if(isSelfClosedTag)
                {
                    Error(in_buffer, strError_UnexpectedCharacterAfterTagClose, pCurrentToken);
                    return nullptr;
                }

                //Assign the current attribute's name.
                m_pCurrentAttribute = new XMLAttribute;
                m_pCurrentAttribute->SetName(m_TemporaryIDString);
                m_pCurrentAttribute->SetValue(L(""));

                //If we're in the version tag, make sure this is a valid parameter name
                if(m_bInVersionTag)
                {
                    bool bIsValidXMLParameter = false;
                    for(u32 i = 0; i < VALID_VERSION_PARAMETER_COUNT; i++)
                    {
                        if( (m_TemporaryIDString == ValidVersionParameters[i]) == 0 )
                            bIsValidXMLParameter = true;
                    }

                    if(!bIsValidXMLParameter)
                    {
                        Error(in_buffer, strError_InvalidXMLTagParameter, pCurrentToken);
                        return nullptr;
                    }
                }

                //Make sure the next token is an assignment '='
                if(!CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_ParamAssignment))
                    return nullptr;

                //Make sure the next token is a string delimiter '"'
                if(!CheckTokenForType(in_buffer, GetNextToken(in_buffer), ETokenType::eTT_StringDelimiter))
                    return nullptr;

                //Get the next token
                pCurrentToken = GetNextToken(in_buffer);

                //Parameters can be empty ('""'), so only do this processing if the
                //current token is NOT a StringDelimiter
                if(pCurrentToken->m_eTokenType != ETokenType::eTT_StringDelimiter)
                {
                    //If it's not a string, error!
                    if(!CheckTokenForType(in_buffer, pCurrentToken, ETokenType::eTT_String))
                        return nullptr;

                    //Set the value of the current attribute
                    m_pCurrentAttribute->SetValue(m_TemporaryIDString);

                    //Hop to the next token.
                    pCurrentToken = GetNextToken(in_buffer);
                }

                //Add the attribute to the current tag
                if(pCurrentTag)
                    pCurrentTag->AddAttribute(m_pCurrentAttribute);

                if(m_bInVersionTag)
                    m_pXMLIDTag->AddAttribute(m_pCurrentAttribute);

                //Make sure the token is a string delimiter '"'
                if(!CheckTokenForType(in_buffer, pCurrentToken, ETokenType::eTT_StringDelimiter))
                    return nullptr;

                break;

            case ETokenType::eTT_CloseBracket:

                //We found our closing bracket '>'

                //If we're in the XML Tag, this file is invalid, because the XML Tag reads in its closing
                //bracket elsewhere.
                if(m_bInVersionTag)
                {
                    Error(in_buffer, strError_ExpectedXMLID, pCurrentToken);
                    return nullptr;
                }

                //If we're not in a closing tag, add this tag to the XML document.
                if(!inClosingTag)
                {
                    AddTag(pCurrentTag, !isSelfClosedTag);
                }
                //If we are in a closing, pop the front tag off the tag stack.
                else
                {
                    if(m_TagList.size() > 0)
                    {
                        m_TagList.pop_front();
                    }
                    else
                        pCurrentTag = nullptr;
                }

                //If we're not in a closing tag of a self-closed tag, look for this tag's content.
                if(!inClosingTag && !isSelfClosedTag)
                {
                    parsingState = EParserState::ePS_Content;
                }
                //Otherwise, start looking for a new tag.
                else
                {
                    if(pCurrentTag)
                        parsingState = EParserState::ePS_Content;
                    else
                        parsingState = EParserState::ePS_NewTag;
                    inClosingTag = false;
                    isSelfClosedTag = false;
                }
                break;

            case ETokenType::eTT_CloseTag:
                //We found a self-close tag token '/'

                //If we're already in a closing tag, or a self closed tag, this is one too many '/'
                if(inClosingTag || isSelfClosedTag)
                {
                    Error(in_buffer, strError_UnexpectedSlashFound, pCurrentToken);
                    return nullptr;
                }

                //Mark this tag as self-closed.
                isSelfClosedTag = true;
                pCurrentTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);

                break;

            default:
                //No other token are acceptable when parsing tag attributes.
                Error(in_buffer, strError_UnexpectedCharacterSequence, pCurrentToken);
                return nullptr;
                break;
            }
            break;
        case EParserState::ePS_Content:
            switch(pCurrentToken->m_eTokenType)
            {
            case ETokenType::eTT_String:
            {
                //This tag has content, so set it and look for a new tag.
                XMLTag *contentTag = new XMLTag;
                contentTag->SetTagType(XMLTag::ETagType::eTT_ContentTag);
                contentTag->SetContent(m_TemporaryIDString);
                AddTag(contentTag, false);
                parsingState = EParserState::ePS_NewTag;
                break;
            }
            case ETokenType::eTT_OpenBracket:
                //Oops, this tag didnt have content.  That's ok, we already found the open bracket, so skip
                //directly to parsing the tag name.
                parsingState = EParserState::ePS_TagName;
                break;
            default:
                //Anything else would be uncivilized.
                Error(in_buffer, strError_UnexpectedCharacterSequence, pCurrentToken);
                return nullptr;
                break;
            }
            break;

        default:
            //Invalid ParserState, this is really bad.
            Error(in_buffer, strError_UnexpectedCharacterSequence, pCurrentToken);
            return nullptr;
        }

		pCurrentToken = GetNextToken(in_buffer);
    }
	
    //We're done parsing, but there's one last thing.  If our tag stack is not currently empty,
    //it means all tag were not closed.  This isn't good, and makes the file invalid.
    if(m_TagList.size() != 0)
    {
        Error(in_buffer, strError_UnexpectedEndOfFile, pCurrentToken);
        return nullptr;
    }

    #if DEBUG_XML_PARSER
        float time = float(OS::GetTickCount() - timeStart) / OS::GetTickFrequency();
        Logger::GetInstance()->Log(LogCategory::FileSystem, LogSeverity::Message, LogType::FileAndDebug, L("Parse Time : %fms (Size : %d)"), time, in_buffer.size());
    #endif

    return m_pXMLRoot;
}

void XMLParser::AddTag(XMLTag* in_pTag, bool in_bPushOnStack)
{
    //If we have tags on our stack, this tag is a child of the front one.
    if(m_TagList.size() > 0)
    {
        m_TagList.front()->AddChild(in_pTag);
    }
    //Otherwise, this tag is a sibling of the root tag
    else if(m_pXMLRoot != nullptr)
    {
        m_pXMLRoot->AddSibling(in_pTag);
    }

    //Get the contents
    if(in_bPushOnStack)
    {
        //Add the tag to the tag stack
        m_TagList.push_front(in_pTag);
    }

    if(m_pXMLRoot == nullptr)
        m_pXMLRoot = in_pTag;
}

void XMLParser::SetLastErrorLine(const std_string& in_buffer, u32 in_u32ErrorPostion)
{
    //Do nothing if the error position is at index 0
    if(in_u32ErrorPostion == 0)
    {
        m_strLastErrorLine = L("");
        m_u32OffsetInLine = 0;
        return;
    }

    u32 u32LineStart = in_u32ErrorPostion;
    u32 u32LineEnd = in_u32ErrorPostion;

    //Find beginning
    while(in_buffer[u32LineStart] != '\n' && in_buffer[u32LineStart] != '\r' && u32LineStart != 0)
        u32LineStart--;

    //Re-increment u32LineStart so we don't start on a '\n' or '\r'
    if(u32LineStart != 0)
        u32LineStart++;

    //Find end
    while(in_buffer[u32LineEnd] != '\n' && in_buffer[u32LineEnd] != '\r' && u32LineEnd < in_buffer.size())
        u32LineEnd++;

    if(u32LineEnd < u32LineStart)
    {
        if(u32LineEnd >= in_buffer.size())
            u32LineStart--;
        else
            u32LineEnd++;
    }

    //Convert to a tidy string
    string_char* codeLine = new string_char[u32LineEnd-u32LineStart+1];
    memcpy(codeLine, in_buffer.c_str()+u32LineStart, (u32LineEnd-u32LineStart) * sizeof(string_char));
    codeLine[u32LineEnd-u32LineStart] = '\0';

    m_strLastErrorLine = codeLine;
    m_u32OffsetInLine = in_u32ErrorPostion - u32LineStart;

    delete[] codeLine;
}

bool XMLParser::CheckTokenForCData(const std_string& in_buffer)
{
    return CheckTokenForSection(in_buffer, L("[CDATA["), L("]]>"));
}

bool XMLParser::CheckTokenForDoctype(const std_string& in_buffer)
{
    return CheckTokenForSection(in_buffer, L("DOCTYPE"), L(">"));
}

bool XMLParser::CheckTokenForSection(const std_string& in_buffer, const std_string& in_sectionBeginning, const std_string& in_sectionEnding)
{
    // Check if there's even enough space to have the section
    const size_t kCharsLeft = (in_buffer.size() - m_u32CurrentPosition);
    if( kCharsLeft < (in_sectionBeginning.length() + in_sectionEnding.length()) )
        return false;

    // If it doesn't start with the section beginning
    if( in_sectionBeginning.compare(0, in_sectionBeginning.length(), &in_buffer[m_u32CurrentPosition], in_sectionBeginning.length()) != 0 )
        return false;

    // If there's no section ending
    std_string strBuffer = &in_buffer[m_u32CurrentPosition];

    u32 closePos = static_cast<u32> (strBuffer.find(in_sectionEnding));
    if(closePos == static_cast<u32>(std_string::npos))
        return false;
    closePos += m_u32CurrentPosition;

    // Copy the section in there
    const u32 startPos = static_cast<const u32>(m_u32CurrentPosition + in_sectionBeginning.length());
    const u32 numChars = (closePos - startPos);
    m_TemporaryIDString.assign( &in_buffer[startPos], numChars );

    // Move the current position forward past the section
    m_u32CurrentPosition = static_cast<u32> (closePos + in_sectionEnding.length());
    return true;
}

bool XMLParser::CheckTokenForType(const std_string& in_buffer, XMLToken* in_pToken, ETokenType in_eExpectedType)
{
    //If the token in nullptr, we hit the end of file.  Not normal.
    if(!in_pToken)
        return Error(in_buffer, strError_UnexpectedEndOfFile, in_pToken);

    if(in_pToken->m_eTokenType != in_eExpectedType)
    {
        switch(in_eExpectedType)
        {
        case ETokenType::eTT_OpenBracket:
            return Error(in_buffer, strError_ExpectedOpenBracket,in_pToken);
        case ETokenType::eTT_CloseBracket:
            return Error(in_buffer, strError_ExpectedCloseBracket,in_pToken);
        case ETokenType::eTT_CommentDelimiter:
            return Error(in_buffer, strError_ExpectedCommentDelimiter,in_pToken);
        case ETokenType::eTT_StringDelimiter:
            return Error(in_buffer, strError_ExpectedStringDelimiter,in_pToken);
        case ETokenType::eTT_String:
            return Error(in_buffer, strError_ExpectedString, in_pToken);
        case ETokenType::eTT_ParamAssignment:
            return Error(in_buffer, strError_ExpectedParamAssignment, in_pToken);
        default:
            return Error(in_buffer, strError_UnexpectedCharacterSequence, in_pToken);
        }
    }

    return true;
}

bool XMLParser::Error(const std_string& in_buffer, const std_string& in_strErrorString, XMLToken* in_pToken)
{
    //Set the last error string.
    m_strLastError = in_strErrorString;

    //Set the last error line according to what the currently parsed token was.
    SetLastErrorLine(in_buffer, in_pToken ? in_pToken->m_u32StartPosition : static_cast<u32>(in_buffer.size()) );

    return false;
}
