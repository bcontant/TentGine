#pragma once

#include <list>

#include "XMLDefines.h"

class XMLAttribute;
class XMLTag;

class XMLParser
{
private:
    enum class ETokenType
    {
        eTT_OpenBracket,        // '<'
        eTT_CloseBracket,       // '>'
        eTT_CloseTag,           // '/'
        eTT_XMLVersionID,       // '?'
        eTT_OpenCommentID,      // '!'
        eTT_CommentDelimiter,   // "--"
        eTT_ParamAssignment,    // '='
        eTT_StringDelimiter,    // '"'
        eTT_String,             // User-defined string
        eTT_Invalid             // Invalid token
    };

#if DEBUG_XML_PARSER
    //Strings representing names of token types.
    static const std_string sppcTagTypes[static_cast<int>(ETokenType::eTT_Invalid)];
#endif

    enum class ETokenizerState
    {
        eTS_Content,
        eTS_InsideTag,
        eTS_InsideParamValue,
        eTS_InsideCommentTag,
        eTS_InsideCommentContent,
        eTS_InsideCommentPossibleDelimiter,
        eTS_Invalid
    };

    enum class EParserState
    {
        ePS_NewTag,
        ePS_TagName,
        ePS_Attribute,
        ePS_CloseTag,
        ePS_Content,
        ePS_Invalid
    };

    class XMLToken
    {
    public:
        XMLToken()
        : m_eTokenType(ETokenType::eTT_Invalid)
        , m_u32StartPosition(0)
        {
        }

        ETokenType m_eTokenType;
		u32 m_u32StartPosition;

        void Reset();
#if DEBUG_XML_PARSER
        void Print();
#endif
    };


	XMLParser(const XMLParser&) {};
	XMLParser operator= (const XMLParser&) {};

public:
    XMLParser();
    ~XMLParser();

    //Parses the XML file.  Returns the root of the XML file if successful,
    //nullptr in case of an error.
    XMLTag* Parse(const std_string& in_buffer);

    //Returns the XML File Version, if any.
    float GetXMLVersion();

    //Accessors and mutators of root
    XMLTag* GetXMLRoot() const { return m_pXMLRoot; }
    void SetXMLRoot(XMLTag* in_pRoot);

    //Returns the last error to have happened with parsing.
    const std_string& GetLastError() const { return m_strLastError; }

    //Returns the last error's associated line.
    const std_string& GetLastErrorLine() const { return m_strLastErrorLine; }

private:
    //Gets the next token in the token stream
    XMLToken* GetNextToken(const std_string& in_buffer);

    //Resets the parser's internal value
    void Reset();

    //Adds a tag to the tag stack, if necessary.
    void AddTag(XMLTag* in_pTag, bool in_bPushOnStack);

    //Token checking
    bool CheckTokenForType(const std_string& in_buffer, XMLToken* in_pToken, ETokenType in_eExpectedType);
    bool CheckTokenForCData(const std_string& in_buffer);
    bool CheckTokenForDoctype(const std_string& in_buffer);
    bool CheckTokenForSection(const std_string& in_buffer, const std_string& in_sectionBeginning, const std_string& in_sectionEnding);

    //Error methods
    bool Error(const std_string& in_buffer, const std_string& in_strErrorString, XMLToken* in_pToken);
    //Sets the last error.
    void SetLastError(std_string& in_strLastError) { m_strLastError = in_strLastError; }
    void SetLastErrorLine(const std_string& in_buffer, u32 in_u32ErrorPostion);

private:
    XMLAttribute* m_pCurrentAttribute;

    //Root Tag
    XMLTag* m_pXMLRoot;

    //XML ID Tag
    XMLTag* m_pXMLIDTag;

    //Stack of currently active tags
    std::list<XMLTag*> m_TagList;

    //Last error encountered by the tokenizer / parser
    std_string m_strLastError;
    std_string m_strLastErrorLine;
    u32 m_u32OffsetInLine;

//---------------------------------//
//[Tokenizer methods and variables]//
//---------------------------------//
private:
    void ResetTempIDString();

    //Tokenize the XML stream.
    void LookForSingleCharacterTokens();
    bool CheckForStringTokenEnd();
    void ContinueCurrentStringToken();

    //Gets the next character in the stream.
    inline string_char GetNextCharacter(const std_string& buffer);

    //Current cursor position in the file buffer
    u32 m_u32CurrentPosition;
    //Current tokenizer state
    ETokenizerState m_eTokenizerState;

    //Temporary tokens used by the Tokenizer
    XMLToken m_CurrentStringToken;
    XMLToken m_CurrentToken;

    //Temporary eTT_String string
    std_string m_TemporaryIDString;

    bool m_bParsingString;
    bool m_bIsCharWhitespace;
    bool m_bStringIsOnlyWhitespace;

    //Temporary next-character value used by the Tokenizer
    string_char m_pcNextCharacter;

    bool m_bTokenizerInterrupted;
    bool m_bInVersionTag;
};

inline string_char XMLParser::GetNextCharacter(const std_string& buffer)
{
    return buffer[m_u32CurrentPosition++];
}

inline void XMLParser::ResetTempIDString()
{
    m_TemporaryIDString = L("");
}
