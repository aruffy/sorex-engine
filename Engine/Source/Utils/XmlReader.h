#pragma once

#include <Sorex/SxCoreMinimal.h>
#include <Sorex/SxStream.h>

namespace Sorex::Utils
{
  enum EXmlEncoding
  {
    XMLENC_Unknown,
    XMLENC_UTF8,
    XMLENC_UTF16,
    XMLENC_UTF16BE,
    XMLENC_Windows1251,  // Windows Cyrillic encoding.
  };

  enum EXmlNodeType
  {
    XMLNODE_None,
    XMLNODE_XmlDeclaration,  // <?xml version="1.0" encoding="UTF-16">
    XMLNODE_Instruction,     // <?name value?>
    XMLNODE_Comment,         // <!--value-->
    XMLNODE_Element,         // <name value>
    XMLNODE_EndElement,      // </name>
    XMLNODE_Text,            // value
  };

  struct XmlAttrib
  {
    String name;
    String value;
  };

  // @fixme: use third-party XML library tinyxml2, rapidXml
  class XmlReader
  {
public:
    EXmlEncoding       encoding;
    EXmlNodeType       nodeType;
    String             name;
    String             value;
    TVector<XmlAttrib> attributes;

public:
    explicit XmlReader(const String& path);
    explicit XmlReader(const TVector<byte>& data);
    XmlReader(TVector<byte>& data, bool swapData);
    explicit XmlReader(Stream& file);

    bool HasFailed() const { return failed; }
    bool EndOfFile() const { return ch && (*ch == 0x0000); }

    String GetFileName() const { return fileName; }
    int    GetLineNum() const { return lineNum; }

    float GetReadingProgress() const;
    int   GetDepth() const { return depth; }

    int GetAttribute(const String& name);
    int GetAttributeNoCase(const String& name);

    bool Read();
    void ReadStartElement();
    void ReadEndElement();

    bool MoveToElement();
    bool MoveToEndElement();
    bool MoveToValue();

    void Skip();

    bool   ReadElementContentAsBool();
    int    ReadElementContentAsInt();
    float  ReadElementContentAsFloat();
    String ReadElementContentAsString();

    bool   ReadValueAsBool();
    int    ReadValueAsInt();
    float  ReadValueAsFloat();
    String ReadValueAsString();

protected:
    String            fileName;
    std::vector<byte> fileBuffer;
    char*             ch;
    int               lineNum;
    int               depth;
    int               dataLength;
    bool              failed;
    bool              doFakeEndElement;

protected:
    void Fail(const String& err);
    void Fail(const char* err = NULL);
    void FailUnsupportedEncoding();

    void LoadDataFromFileBuffer();
    void ScanUtf8Prolog();
    bool SkipWhitespace();
    void ParseComment();
    void ParseInstruction();
    void ParseElement();
    void ParseEndElement();
    void ParseAttribute();
    void ParseCharData();
    void ParseName();

    String DecodeXmlText(char* original, int originalLength);
    void   WCharToCharCStr(char buffer[5], wchar_t& wchar);
  };
}  // namespace Sorex::Utils