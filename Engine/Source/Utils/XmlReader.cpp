#include "XmlReader.h"

#include <Sorex/FileSystem/SxFile.h>
#include <Sorex/Utils/SxString.h>

#include <ctype.h>

namespace
{
  wchar_t ReadUtf8Char(const unsigned char* buf, const size_t len, int& bytes)
  {
    bytes = 0;

    if (len < 1)
      return -1;

    int ch1 = buf[0];

    if ((ch1 & 0x80) == 0)
    {
      bytes = 1;
      return ch1;
    }
    else if ((ch1 & 0xE0) == 0xC0)
    {
      if (len < 2)
        return -1;

      int ch2 = buf[1];

      if ((ch2 & 0xC0) == 0x80)
      {
        bytes = 2;
        return ((ch1 & 0x1F) << 6) | (ch2 & 0x3F);
      }
    }
    else if ((ch1 & 0xF0) == 0xE0)
    {
      if (len < 2)
        return -1;

      int ch2 = buf[1];

      if ((ch2 & 0xC0) == 0x80)
      {
        if (len < 3)
          return -1;

        int ch3 = buf[2];

        if ((ch3 & 0xC0) == 0x80)
        {
          bytes = 3;
          return ((ch1 & 0x0F) << 12) | ((ch2 & 0x3F) << 6) | (ch3 & 0x3F);
        }
      }
    }
    else if ((ch1 & 0xF8) == 0xF0)
    {
      if (len < 2)
        return -1;

      int ch2 = buf[1];

      if ((ch2 & 0xC0) == 0x80)
      {
        if (len < 3)
          return -1;

        int ch3 = buf[2];

        if ((ch3 & 0xC0) == 0x80)
        {
          if (len < 4)
            return -1;

          int ch4 = buf[3];

          if ((ch4 & 0xC0) == 0x80)
          {
            bytes = 4;
            return ((ch1 & 0x07) << 18) | ((ch2 & 0x3F) << 12)
                   | ((ch3 & 0x3F) << 6) | (ch4 & 0x3F);
          }
        }
      }
    }

    return -1;  // Bad UTF8 character.
  }

}

namespace Sorex::Utils
{
  XmlReader::XmlReader(const String& path)
    : encoding(XMLENC_Unknown)
    , nodeType(XMLNODE_None)
    , fileName(path)
    , ch(nullptr)
    , lineNum(1)
    , depth(0)
    , dataLength(0)
    , failed(false)
    , doFakeEndElement(false)
  {
    File file(path);
    if (file.IsOpen())
      file.ReadAll(fileBuffer);
  }

  XmlReader::XmlReader(Stream& stream)
    : encoding(XMLENC_Unknown)
    , nodeType(XMLNODE_None)
    , fileName()
    , ch(nullptr)
    , lineNum(1)
    , depth(0)
    , dataLength(0)
    , failed(false)
    , doFakeEndElement(false)
  {
    stream.ReadAll(fileBuffer);
  }

  XmlReader::XmlReader(const TVector<byte>& data)
    : encoding(XMLENC_Unknown)
    , nodeType(XMLNODE_None)
    , fileName()
    , fileBuffer(data)
    , ch(nullptr)
    , lineNum(1)
    , depth(0)
    , dataLength(0)
    , failed(false)
    , doFakeEndElement(false)
  {}

  XmlReader::XmlReader(TVector<byte>& data, bool swapData)
    : encoding(XMLENC_Unknown)
    , nodeType(XMLNODE_None)
    , fileName()
    , ch(nullptr)
    , lineNum(1)
    , depth(0)
    , dataLength(0)
    , failed(false)
    , doFakeEndElement(false)
  {
    if (!data.empty())
    {
      fileBuffer = data;
      if (swapData)
      {
        data.clear();
      }
    }
  }

  float XmlReader::GetReadingProgress() const
  {
    if (ch != nullptr && dataLength > 0)
    {
      intptr_t pos = ch - (char*)(&fileBuffer[0]);
      return (float)pos / (float)dataLength;
    }
    return 0.f;
  }

  int XmlReader::GetAttribute(const String& name)
  {
    for (int i = 0; i < (int)attributes.size(); ++i)
      if (attributes[i].name == name)
        return i;

    return -1;
  }

  int XmlReader::GetAttributeNoCase(const String& name)
  {
    for (int i = 0; i < (int)attributes.size(); ++i)
      if (0 == Utils::CompareNoCase(attributes[i].name, name))
        return i;

    return -1;
  }

  bool XmlReader::Read()
  {
    if (HasFailed() || EndOfFile())
      return false;

    if (ch == nullptr)
    {
      LoadDataFromFileBuffer();

      if (HasFailed())
        return false;
    }
    if (nodeType == XMLNODE_Element)
      depth++;

    if (doFakeEndElement)
    {
      SRX_ASSERT(nodeType == XMLNODE_Element);
      doFakeEndElement = false;
      nodeType         = XMLNODE_EndElement;
      value            = "";
      attributes.clear();
      depth--;
      return true;
    }

    if (depth == 0)
      SkipWhitespace();

    nodeType = XMLNODE_None;
    name     = String();
    value    = String();
    attributes.clear();

    if (ch[0] == '<')
    {
      if (ch[1] == '/')
        ParseEndElement();
      else if (ch[1] == '?')
        ParseInstruction();
      else if (ch[1] == '!' && ch[2] == '-' && ch[3] == '-')
        ParseComment();
      else
        ParseElement();
    }
    else
    {
      ParseCharData();
    }

    return !HasFailed();
  }

  void XmlReader::ReadStartElement()
  {
    if (MoveToElement())
      Read();
    else
      Fail("Start of an element expected.");
  }

  void XmlReader::ReadEndElement()
  {
    if (MoveToEndElement())
      Read();
    else
      Fail("End of an element expected.");
  }

  void XmlReader::Skip()
  {
    if (nodeType == XMLNODE_Element)
    {
      int d = depth;
      while (Read())
      {
        if (nodeType == XMLNODE_EndElement && depth == d)
          break;
      }
    }

    Read();
  }

  bool XmlReader::ReadElementContentAsBool()
  {
    ReadStartElement();
    bool value = ReadValueAsBool();
    ReadEndElement();
    return value;
  }

  int XmlReader::ReadElementContentAsInt()
  {
    ReadStartElement();
    int value = ReadValueAsInt();
    ReadEndElement();
    return value;
  }

  float XmlReader::ReadElementContentAsFloat()
  {
    ReadStartElement();
    float value = ReadValueAsFloat();
    ReadEndElement();
    return value;
  }

  String XmlReader::ReadElementContentAsString()
  {
    ReadStartElement();
    String res = ReadValueAsString();
    ReadEndElement();
    return res;
  }

  bool XmlReader::ReadValueAsBool()
  {
    if (MoveToValue())
    {
      if (value == "0" || value == "false")
      {
        Read();
        return false;
      }
      else if (value == "1" || value == "true")
      {
        Read();
        return true;
      }
    }

    Fail("Boolean value expected.");
    return false;
  }

  int XmlReader::ReadValueAsInt()
  {
    if (!MoveToValue())
    {
      Fail("Integer value expected.");
      return 0;
    }

    int n;
    Utils::ToInteger(value, n);

    Read();
    return n;
  }

  float XmlReader::ReadValueAsFloat()
  {
    if (!MoveToValue())
    {
      Fail("Float value expected.");
      return 0;
    }

    float f = Utils::ToFloat(value);

    Read();
    return f;
  }

  String XmlReader::ReadValueAsString()
  {
    if (!MoveToValue())
    {
      Fail("String value expected.");
      return nullptr;
    }

    String res = value;
    Read();
    return res;
  }

  void XmlReader::Fail(const String& text)
  {
    Fail(text.data());
  }

  void XmlReader::Fail(const char* text)
  {
    if (!failed)
    {
      if (!fileName.empty())
      {
        SRX_ERROR("XmlReader error: \'{}\'", fileName.data());
        if (!fileBuffer.empty())
          SRX_ERROR("({})", lineNum);
      }
      else
      {
        SRX_ERROR("XmlReader: error");
      }

      if (text != NULL)
        SRX_ERROR(": {}\n", text);
      else
        SRX_ERROR("\n");

      failed = true;
    }
  }

  void XmlReader::FailUnsupportedEncoding()
  {
    char* encodingName;
    switch (encoding)
    {
    case XMLENC_UTF8:
      encodingName = String("UTF8").data();
      break;
    case XMLENC_UTF16:
      encodingName = String("UTF16").data();
      break;
    case XMLENC_UTF16BE:
      encodingName = String("UTF16BE").data();
      break;
    case XMLENC_Windows1251:
      encodingName = String("Windows1251").data();
      break;
    default:
      encodingName = String("Unknown").data();
      break;
    }

    SRX_NOENTRY("XmlReader: unsupported encoding");
    Fail();
    // FIXME: formatting
    /* Fail(Utils::Printf("The loaded file %s is of unsupported encoding %s",
                       fileName.data(),
                       encodingName)); */
  }

  void XmlReader::LoadDataFromFileBuffer()
  {
    if (fileBuffer.size() < 4)
    {
      Fail();
      return;
    }

    bool byteOrderMarkFound = true;

    // Get first 4 bytes to detect a byte-order-mark (BOM).
    int ch1 = fileBuffer[0];
    int ch2 = fileBuffer[1];
    int ch3 = fileBuffer[2];
    int ch4 = fileBuffer[3];

    if (ch1 == 0 && ch2 == 0)
    {
      if (ch2 == 0xFE && ch3 == 0xFF)
        encoding = XMLENC_Unknown;  // Not implemented: UCS-4, big-endian
                                    // machine (1234 order) aka utf-32BE
      else if (ch2 == 0xFF && ch3 == 0xFE)
        encoding =
          XMLENC_Unknown;  // Not implemented: UCS-4, unusual octet order (2143)
      else
        byteOrderMarkFound = false;
    }
    else if (ch1 == 0xFE && ch2 == 0xFF)
    {
      if (ch3 == 0 && ch4 == 0)
        encoding =
          XMLENC_Unknown;  // Not implemented: UCS-4, unusual octet order (3412)
      else
        encoding = XMLENC_UTF16BE;  // UTF-16BE
    }
    else if (ch1 == 0xFF && ch2 == 0xFE)
    {
      if (ch3 == 0 && ch4 == 0)
        encoding = XMLENC_Unknown;  // Not implemented: UCS-4, little-endian
                                    // machine (4321 order), aka utf-32
      else
        encoding = XMLENC_UTF16;  // UTF-16 aka UnicodeFFFE
    }
    else if (ch1 == 0xEF && ch2 == 0xBB && ch3 == 0xBF)
    {
      encoding = XMLENC_UTF8;  // UTF-8
    }
    else
    {
      byteOrderMarkFound = false;

      // BOM was not found, so we expect to find '<?' as first symbols.
      // This can help us to auto-detect used encoding.
      if (ch1 == 0 && ch2 == 0x3C && ch3 == 0 && ch4 == 0x3F)
        encoding = XMLENC_UTF16BE;
      else if (ch1 == 0x3C && ch2 == 0 && ch3 == 0x3F && ch4 == 0)
        encoding = XMLENC_UTF16;
      else
      {
        // Unknown sumbols at start so we decide to use UTF8, because this
        // is a default encoding of XML format.
        encoding = XMLENC_UTF8;
      }
    }

    if (encoding != XMLENC_UTF8)
    {
      FailUnsupportedEncoding();
      return;
    }

    // Добавим несколько нулей в конец буффера, т.к. разные функции парсят пока
    // не найдут ноль.
    // FIXME: Это может привести к переалокации памяти в std::vector'е.
    // Эффективнее сразу размещать данные
    //        в буфере с запасом под эти нули. Но на практике не критично, т.к.
    //        обычно вектор выделяет память блоками
    fileBuffer.push_back(0x0000);
    fileBuffer.push_back(0x0000);

    byte* fileBufferPos      = &fileBuffer[0];
    int   fileBufferRestSize = (int)fileBuffer.size();

    if (byteOrderMarkFound)
    {
      fileBufferPos += 3;
      fileBufferRestSize -= 3;
    }
    else
    {
      // Looks like we have UTF8 here, let's parse <?xml> node and
      // see if encoding was manualy specified.
      ScanUtf8Prolog();
    }

    dataLength = fileBufferRestSize;
    ch         = (char*)fileBufferPos;
  }

  void XmlReader::ScanUtf8Prolog()
  {
    // Scan until first '>' to see if the file starts with a valid node.
    int prologElementEndIdx = 0;

    while (prologElementEndIdx < (int)fileBuffer.size())
    {
      if (fileBuffer[prologElementEndIdx] == '>')
        break;

      ++prologElementEndIdx;
    }

    if (prologElementEndIdx == static_cast<int>(fileBuffer.size()))
      return;

    // Ok. Let's read first node and initiate its parsing.
    int prologSizeInBytes = prologElementEndIdx + 1;

    byte* fileBufferPos  = &fileBuffer[0];
    int   prologRestSize = prologSizeInBytes;

    bool doBreakAndReturn = false;
    while (prologRestSize > 0)
    {
      int bytesRead = 0;
      ReadUtf8Char(fileBufferPos, prologRestSize, bytesRead);

      if (bytesRead < 1)
      {
        doBreakAndReturn = true;
        break;
      }

      fileBufferPos += bytesRead;
      prologRestSize -= bytesRead;
      ++ch;
    }

    if (doBreakAndReturn)
      return;

    ch = (char*)&(fileBuffer[0]);

    Read();

    // If this is a valid XML declaration node then find 'encoding' attribute.
    if (nodeType == XMLNODE_XmlDeclaration)
    {
      for (int i = 0; i < (int)attributes.size(); ++i)
      {
        if (attributes[i].name != "encoding")
          continue;

        // Parse supported encodings.
        // TODO: All standard encodings here please!
        if (Utils::CompareNoCase(attributes[i].value, "utf-8"))
          encoding = XMLENC_UTF8;
        else if (Utils::CompareNoCase(attributes[i].value, "windows-1251"))
          encoding = XMLENC_Windows1251;
        else
          Fail("Unknown XML encoding specified.");

        break;
      }
    }

    ch = nullptr;
  }

  bool XmlReader::MoveToValue()
  {
    while ((nodeType == XMLNODE_Comment) || (nodeType == XMLNODE_Instruction))
    {
      if (!Read())
        return false;
    }

    if (nodeType == XMLNODE_Text)
      return true;
    else
      return false;
  }

  bool XmlReader::MoveToElement()
  {
    do
    {
      if (nodeType == XMLNODE_Element)
        return true;

      if (nodeType == XMLNODE_EndElement)
        return false;
    } while (Read());

    return false;
  }

  bool XmlReader::MoveToEndElement()
  {
    do
    {
      if (nodeType == XMLNODE_EndElement)
        return true;
      if (nodeType == XMLNODE_Element)
        return false;
    } while (Read());

    return false;
  }

  bool XmlReader::SkipWhitespace()
  {
    char* p = ch;
    for (;;)
    {
      if (*p == '\n')
        lineNum++;
      else if (*p != ' ' && *p != '\t' && *p != '\r')
        break;
      ++p;
    }

    if (p == ch)
      return false;

    ch = p;
    return true;
  }

  void XmlReader::ParseComment()
  {
    SRX_ASSERT(ch[0] == '<' && ch[1] == '!' && ch[2] == '-' && ch[3] == '-');
    ch += 4;
    char* valueStart = ch;
    for (;;)
    {
      if (ch[0] == 0x0000)
      {
        Fail("Missing end of comment '-->'.");
        return;
      }
      else if (ch[0] == '-' && ch[1] == '-')
      {
        if (ch[2] == '>')
        {
          value = String(valueStart, ch - valueStart);
          ch += 3;
          break;
        }
        else
        {
          Fail("Sequence '--' is illegal inside XML comments.");
          return;
        }
      }
      else if (ch[0] == '\n')
      {
        lineNum++;
      }
      ch++;
    }

    nodeType = XMLNODE_Comment;
  }

  void XmlReader::ParseInstruction()
  {
    SRX_ASSERT(ch[0] == '<' && ch[1] == '?');
    ch += 2;

    char* nameStart = ch;
    ParseName();
    name = String(nameStart, ch - nameStart);

    SkipWhitespace();

    char* valueStart = ch;
    if (name == "xml")
    {
      for (;;)
      {
        SkipWhitespace();

        if (ch[0] == 0x0000)
        {
          Fail("Missing end of XML declaration '?>'.");
          return;
        }
        else if (ch[0] == '?' && ch[1] == '>')
        {
          value = String(valueStart, ch - valueStart);
          ch += 2;
          break;
        }
        else
        {
          ParseAttribute();
          if (HasFailed())
            return;
        }
      }

      nodeType = XMLNODE_XmlDeclaration;
    }
    else
    {
      for (;;)
      {
        if (ch[0] == 0x0000)
        {
          Fail("Missing end of instruction '?>'.");
          return;
        }
        else if (ch[0] == '?' && ch[1] == '>')
        {
          value = String(valueStart, ch - valueStart);
          ch += 2;
          break;
        }
        else if (ch[0] == '\n')
        {
          lineNum++;
        }
        ch++;
      }

      nodeType = XMLNODE_Instruction;
    }
  }

  void XmlReader::ParseElement()
  {
    SRX_ASSERT(ch[0] == '<');
    ch++;

    char* nameStart = ch;
    ParseName();
    if (HasFailed())
      return;
    name = String(nameStart, ch - nameStart);


    SkipWhitespace();

    char* valueStart = ch;
    char* valueEnd;
    for (;;)
    {
      if (ch[0] == '/' && ch[1] == '>')
      {
        valueEnd         = ch;
        doFakeEndElement = true;
        ch += 2;
        break;
      }
      else if (ch[0] == '>')
      {
        valueEnd = ch;
        ch++;
        break;
      }
      else
      {
        ParseAttribute();
        if (HasFailed())
          return;
      }
    }

    value    = String(valueStart, valueEnd - valueStart);
    nodeType = XMLNODE_Element;
  }

  void XmlReader::ParseEndElement()
  {
    SRX_ASSERT(ch[0] == '<' && ch[1] == '/');
    ch += 2;

    char* nameStart = ch;
    ParseName();
    if (HasFailed())
      return;
    name = String(nameStart, ch - nameStart);

    SkipWhitespace();

    if (ch[0] != '>')
    {
      Fail("'>' expected.");
      return;
    }

    ch++;

    nodeType = XMLNODE_EndElement;
    depth--;
  }

  void XmlReader::ParseName()
  {
    if (!iswalpha(ch[0]) && ch[0] != '_')
    {
      if ((unsigned char)(ch[0]) < 128)
        Fail(/*StrUtils::Printf(
          "The character '%c' is illegal at the begining of an XML name.",
          (char)ch[0])*/);
      else
        Fail(/*Utils::Printf(
          "The character #x%04X is illegal at the begining of an XML name.",
          (int)ch[0])*/);
      return;
    }

    while (::iswalpha(ch[0]) || ::iswdigit(ch[0]) || (ch[0] == '_')
           || (ch[0] == '-') || (ch[0] == '.'))
    {
      ch++;
    }
  }

  void XmlReader::ParseAttribute()
  {
    XmlAttrib attrib;

    char* nameStart = ch;
    ParseName();
    if (HasFailed())
      return;
    attrib.name = String(nameStart, ch - nameStart);

    SkipWhitespace();
    if (*ch++ != '=')
    {
      Fail("Missing attribute value.");
      return;
    }
    SkipWhitespace();

    wchar_t quotChar = *ch++;
    if ((quotChar != '\"') && (quotChar != '\''))
    {
      Fail("Missing attribute value.");
      return;
    }

    char* valueStart = ch;
    for (;;)
    {
      if (ch[0] == 0x0000)
      {
        Fail("End of attribute value '\"' expected.");
        return;
      }
      else if (ch[0] == quotChar)
      {
        attrib.value = DecodeXmlText(valueStart, (int)(ch - valueStart));
        ch++;
        break;
      }
      else if (ch[0] == '\n')
      {
        lineNum++;
      }
      ch++;
    }

    SkipWhitespace();
    attributes.push_back(std::move(attrib));
  }

  void XmlReader::ParseCharData()
  {
    char* valueStart = ch;
    while (ch[0] != '<' && ch[0] != 0x0000)
    {
      if (ch[0] == '\n')
        lineNum++;
      ch++;
    }
    value    = String(valueStart, ch - valueStart);
    nodeType = XMLNODE_Text;
  }

  String XmlReader::DecodeXmlText(char* original, int originalLength)
  {
    if (originalLength <= 0)
      return String();

    char* readPointer  = original;
    char* writePointer = readPointer;

    char c;
    while ((c = *readPointer) != '\0')
    {
      if (c == '&')
      {
        // Find the nearest ';', which should terminate
        // the special XML character. The maximum distance at which
        // ';' can be located is 6 characters away. Example: &#xfffb;
        int semicolonShift =
          -1;  // Shift to the ';' character from the readPointer
        // We start the loop at 1 because at 0 shift there is only '&'
        for (int i = 1; i < 8 && readPointer[i] != '\0'; ++i)
          if (readPointer[i] == ';')
          {
            semicolonShift = i;
            break;
          }

        // If the position of ';' is -1, then it's just "&;". No need to process
        if (semicolonShift >= 2)
        {
          char* afterAmpersandPoint = readPointer + 1;
          // We confidently move to the point where ';' was
          readPointer = readPointer + semicolonShift;

          // In order to avoid unnecessary strcmp calls, it was decided to look
          // at the first character after the ampersand. If it already matches
          // the initial character of some XML special entity, we can use
          // strncmp inside. First, we check that the distance to ';'
          // matches the expected size of the special entity.

          // The length in bytes between '&' and ';'
          int specialLength = semicolonShift - 1;

          switch (afterAmpersandPoint[0])
          {
            // strncmp - compares 2 C strings, but only up to the character at
            // position N (the 3rd parameter)
          case 'a':
            if (specialLength == 4
                && strncmp("apos", afterAmpersandPoint, specialLength) == 0)
              c = '\'';
            else if (specialLength == 3
                     && strncmp("amp", afterAmpersandPoint, specialLength) == 0)
              c = '&';
            break;
          case 'c':
            if (specialLength == 2
                && strncmp("cr", afterAmpersandPoint, specialLength) == 0)
              c = '\n';
            break;
          case 'g':
            if (specialLength == 2
                && strncmp("gt", afterAmpersandPoint, specialLength) == 0)
              c = '>';
            break;
          case 'l':
            if (specialLength == 2
                && strncmp("lt", afterAmpersandPoint, specialLength) == 0)
              c = '<';
            break;
          case 'n':
            if (specialLength == 4
                && strncmp("nbsp", afterAmpersandPoint, specialLength) == 0)
              c = ' ';
            break;
          case 'q':
            if (specialLength == 4
                && strncmp("quot", afterAmpersandPoint, specialLength) == 0)
              c = '"';
            break;
          case '#':
            if (specialLength > 2)
            {
              int  n = 0;
              char buffer[10];
#ifdef _MSC_VER
              strncpy_s(buffer, afterAmpersandPoint + 1, specialLength - 1);
#else
              strncpy(buffer, afterAmpersandPoint + 1, specialLength - 1);
#endif
              buffer[specialLength - 1] = '\0';
              if (afterAmpersandPoint[1] == 'x')
              {
                // In this case, to read, you need to "discard" '#x'
#ifdef _MSC_VER
                sscanf_s(buffer + 1, "%x", &n);
#else
                sscanf(buffer + 1, "%x", &n);
#endif
              }
              else
              {
                // Here we "discard" only the '#'
#ifdef _MSC_VER
                sscanf_s(buffer, "%i", &n);
#else
                sscanf(buffer, "%i", &n);
#endif
              }
              wchar_t wchar = (wchar_t)n;
              // The maximum size of WStringToUtf8String is 4 characters.
              // No need to worry about skipping on readPointer.
              // Because the smallest size of a special character
              // starting with # is &#1; which is exactly 4 characters.
              // In this section, we put all characters from the string into
              // writePointer, except the last one. It will be added via char c.
              WCharToCharCStr(buffer, wchar);
              const int strLength = static_cast<int>(strlen(buffer));
              if (strLength > 0)
              {
                c = buffer[strLength - 1];
                for (int i = 0; i < strLength - 1; ++i)
                {
                  *writePointer = buffer[i];
                  writePointer++;
                }
              }
            }
            break;
          }
        }
      }

      *writePointer = c;
      writePointer++;
      readPointer++;

      if (readPointer - original >= originalLength)
        break;
    }

    return String(original, writePointer - original);
  }

  void XmlReader::WCharToCharCStr(char buffer[5], wchar_t& wchar)
  {
    if ((uint32)wchar < 0x0080)
    {
      buffer[0] = (char)wchar;
      buffer[1] = (char)'\0';
    }
    else if ((uint32)wchar < 0x0800)
    {
      buffer[0] = (char)(0xC0 | ((wchar >> 6) & 0x1F));
      buffer[1] = (char)(0x80 | (wchar & 0x3F));
      buffer[2] = '\0';
    }
    else if ((uint32)wchar < 0x10000)
    {
      buffer[0] = (char)(0xE0 | ((wchar >> 12) & 0x0F));
      buffer[1] = (char)(0x80 | ((wchar >> 6) & 0x3F));
      buffer[2] = (char)(0x80 | (wchar & 0x3F));
      buffer[3] = '\0';
    }
    else
    {
      buffer[0] = (char)(0xF0 | ((wchar >> 12 >> 6) & 0x07));
      buffer[1] = (char)(0x80 | ((wchar >> 12) & 0x3F));
      buffer[2] = (char)(0x80 | ((wchar >> 6) & 0x3F));
      buffer[3] = (char)(0x80 | (wchar & 0x3F));
      buffer[4] = '\0';
    }
  }
}