
/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "SxXmlFontDataLoader.h"

#include <Sorex/Utils/SxString.h>

#include "tinyxml2.h"

namespace
{
  using namespace Sorex;
  bool ParseFontMetrics(const tinyxml2::XMLElement&  xmlMetrics,
                        Graphics::FontData::Metrics& metrics)
  {
    SRX_CHECK(Utils::CompareNoCase(xmlMetrics.Name(), "Metrics"));

    int value = 0;
    xmlMetrics.QueryIntAttribute("top", &value);
    metrics.top = static_cast<int16>(value);

    if (metrics.top <= 0)
      return false;

    xmlMetrics.QueryIntAttribute("bottom", &value);
    metrics.bottom = static_cast<int16>(value);
    xmlMetrics.QueryIntAttribute("ascent", &value);
    metrics.ascent = static_cast<int16>(value);
    xmlMetrics.QueryIntAttribute("descent", &value);
    metrics.descent = static_cast<int16>(value);
    xmlMetrics.QueryIntAttribute("leading", &value);
    metrics.leading = static_cast<int16>(value);

    if (metrics.ascent <= 0)
      metrics.ascent = metrics.top;
    if (metrics.leading <= 0)
      metrics.leading = metrics.top;

    return true;
  }

  bool ParseFontGlyph(const tinyxml2::XMLElement& xmlChar,
                      Graphics::FontGlyph&        glyph)
  {
    SRX_CHECK(Utils::CompareNoCase(xmlChar.Name(), "Char"));
    // Parse attributes using tinyxml2
    const char* codeAttr = xmlChar.Attribute("code");
    if (!codeAttr)
      return false;

    int value  = 0;
    glyph.code = Utils::ReadUtf8Char(codeAttr, strlen(codeAttr), value);
    if (value == 0)
      return false;

    if (xmlChar.QueryIntAttribute("width", &value) == tinyxml2::XML_SUCCESS)
      glyph.rect.width = value;
    if (xmlChar.QueryIntAttribute("height", &value) == tinyxml2::XML_SUCCESS)
      glyph.rect.height = value;
    if (xmlChar.QueryIntAttribute("x", &value) == tinyxml2::XML_SUCCESS)
      glyph.rect.x = value;
    if (xmlChar.QueryIntAttribute("y", &value) == tinyxml2::XML_SUCCESS)
      glyph.rect.y = value;
    if (xmlChar.QueryIntAttribute("bx", &value) == tinyxml2::XML_SUCCESS)
      glyph.bearing.x = value;
    if (xmlChar.QueryIntAttribute("by", &value) == tinyxml2::XML_SUCCESS)
      glyph.bearing.y = value;
    if (xmlChar.QueryIntAttribute("advance", &value) == tinyxml2::XML_SUCCESS)
      glyph.advance = value;

    if (!glyph.advance)
      glyph.advance = glyph.rect.width;

    return (glyph.rect.x >= 0 && glyph.rect.y >= 0)
           && glyph.rect.GetSize().IsValid();
  }

  bool ParseFontAttribs(tinyxml2::XMLElement& xmlFont, Graphics::FontData& font)
  {
    const bool isFontElement = Utils::CompareNoCase(xmlFont.Name(), "Font");
    SRX_CHECK(isFontElement);
    if (!isFontElement)
      return false;

    if (xmlFont.Attribute("size") == nullptr)
      return false;

    const char* attrValue = xmlFont.Attribute("family");
    if (attrValue)
      font.family = attrValue;

    attrValue  = xmlFont.Attribute("style");
    font.style = attrValue ? attrValue : "Regular";

    int fontSize = 0;
    if (xmlFont.QueryIntAttribute("size", &fontSize) != tinyxml2::XML_SUCCESS
        || fontSize <= 0)
      return false;

    font.size = static_cast<uint16>(fontSize);
    font.type = Graphics::EFontType::Bitmap;  // Default type
    attrValue = xmlFont.Attribute("type");
    if (attrValue && Utils::CompareNoCase(attrValue, "sdf"))
      font.type = Graphics::EFontType::Signed_Distance_Field;

    return true;
  }
}

namespace Sorex::Resource
{
  TPair<TUniquePointer<Graphics::FontData>,
        TUniquePointer<Graphics::TextureBitmap>>
  XMLFontDataLoader::LoadFont(Stream& stream, Status* status)
  {
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement* xmlRoot;
    {
      TVector<byte> xmlBuffer;
      ssize_t       bytesRead = stream.ReadAll(xmlBuffer);
      if (bytesRead == SRX_READ_ERROR)
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::Invalid_Argument,
                           "failed to read xml font data stream");
        return std::make_pair(nullptr, nullptr);
      }

      if (xml.Parse(reinterpret_cast<const char*>(xmlBuffer.data()),
                    xmlBuffer.size())
          != tinyxml2::XML_SUCCESS)
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::Bad_File,
                           "failed to parse xml font data");
        return std::make_pair(nullptr, nullptr);
      }
    }

    xmlRoot                                 = xml.RootElement();
    TUniquePointer<Graphics::FontData> font = MakeUnique<Graphics::FontData>();
    if (xmlRoot == nullptr || !ParseFontAttribs(*xmlRoot, *font))
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "xml font root element failed");
      return std::make_pair(nullptr, nullptr);
    }

    const tinyxml2::XMLElement* xmlElement =
      xmlRoot->FirstChildElement("Metrics");
    if (xmlElement == nullptr || !ParseFontMetrics(*xmlElement, font->metrics))
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "parsing xml font metrics failed");
      return std::make_pair(nullptr, nullptr);
    }

    xmlElement = xmlRoot->FirstChildElement("Glyphs");

    // Iterate through all glyphs
    Graphics::FontGlyph      glyph;
    const tinyxml2::XMLNode* xmlNode =
      xmlElement ? xmlElement->FirstChild() : nullptr;
    const tinyxml2::XMLElement* xmlChar = nullptr;
    while (xmlNode)
    {
      xmlChar = xmlNode->ToElement();
      if (xmlChar && ParseFontGlyph(*xmlChar, glyph))
        font->glyphs.emplace(glyph.code, glyph);

      xmlNode = xmlNode->NextSibling();
    }

    return std::make_pair(std::move(font), nullptr);
  }
}  // namespace Sorex::Resource