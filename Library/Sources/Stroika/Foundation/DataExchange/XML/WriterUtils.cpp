/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <ostream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Throw.h"

#include "WriterUtils.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

namespace {
    // XML 1.0's NameStartChar and NameChar (https://www.w3.org/TR/xml/#NT-NameStartChar), less ':' - ranges of code points
    constexpr bool IsNCNameStartChar_ (Character ch)
    {
        char32_t c = ch.GetCharacterCode ();
        return (c >= 'A' and c <= 'Z') or c == '_' or (c >= 'a' and c <= 'z') or (c >= 0xC0 and c <= 0xD6) or (c >= 0xD8 and c <= 0xF6) or
               (c >= 0xF8 and c <= 0x2FF) or (c >= 0x370 and c <= 0x37D) or (c >= 0x37F and c <= 0x1FFF) or (c >= 0x200C and c <= 0x200D) or
               (c >= 0x2070 and c <= 0x218F) or (c >= 0x2C00 and c <= 0x2FEF) or (c >= 0x3001 and c <= 0xD7FF) or
               (c >= 0xF900 and c <= 0xFDCF) or (c >= 0xFDF0 and c <= 0xFFFD) or (c >= 0x10000 and c <= 0xEFFFF);
    }
    constexpr bool IsNCNameChar_ (Character ch)
    {
        char32_t c = ch.GetCharacterCode ();
        return IsNCNameStartChar_ (ch) or c == '-' or c == '.' or (c >= '0' and c <= '9') or c == 0xB7 or (c >= 0x300 and c <= 0x36F) or
               (c >= 0x203F and c <= 0x2040);
    }

    // XML 1.0's Char (https://www.w3.org/TR/xml/#NT-Char): what a document can hold at all, literally or as a character reference
    constexpr bool IsXMLChar_ (char32_t c)
    {
        return c == '\t' or c == '\n' or c == '\r' or (c >= 0x20 and c <= 0xD7FF) or (c >= 0xE000 and c <= 0xFFFD) or (c >= 0x10000 and c <= 0x10FFFF);
    }

    // c - a character with no entity of its own - spelled so a parser reads it back as c: in ASCII, so anything else as a
    // reference to its code point; and as a reference too, what a parser would change - a CR (line ends are normalized to LF),
    // and in an attribute a TAB or LF (attribute values are normalized, each to a space). One XML 1.0 cannot hold throws
    void AppendCharacter_ (string* r, char32_t c, bool inAttribute)
    {
        if (not IsXMLChar_ (c)) {
            Execution::Throw (
                BadFormatException{"U+{:04X} is not a character XML 1.0 can hold, so cannot be written as XML"_f(static_cast<uint32_t> (c))});
        }
        if (c > 0x7F or c == '\r' or (inAttribute and (c == '\t' or c == '\n'))) {
            *r += CString::Format ("&#%u;", static_cast<unsigned int> (c));
        }
        else {
            r->push_back (static_cast<char> (c));
        }
    }
}

/*
 ********************************************************************************
 ************************************ IsNCName **********************************
 ********************************************************************************
 */
bool XML::IsNCName (const String& s)
{
    bool first = true;
    for (Character c : s) {
        if (not(first ? IsNCNameStartChar_ (c) : IsNCNameChar_ (c))) {
            return false;
        }
        first = false;
    }
    return not s.empty ();
}

/*
 ********************************************************************************
 ****************************** QuoteForXMLAttribute ****************************
 ********************************************************************************
 */
string XML::QuoteForXMLAttribute (const string& s)
{
    string r;
    r.reserve (Containers::Support::ReserveTweaks::GetScaledUpCapacity (6 * s.size () / 5)); // most chars just written as character, but some take 5 characters, so WAG about percentage
    for (auto i = s.begin (); i != s.end (); ++i) {
        switch (*i) {
            case '&':
                r += "&amp;";
                break;
            case '<':
                r += "&lt;";
                break;
            case '>':
                r += "&gt;";
                break;
            case '\"':
                r += "&quot;";
                break;
            case '\'':
                r += "&apos;";
                break;
            default:
                r.push_back (*i);
                break;
        }
    }
    return r;
}

string XML::QuoteForXMLAttribute (const wstring& s)
{
    return QuoteForXMLAttribute (String{s});
}

string XML::QuoteForXMLAttribute (const String& s)
{
    u32string u = s.As<u32string> (); // code points, not code units - so a character outside the BMP is one reference, not two
    string    r;
    r.reserve (Containers::Support::ReserveTweaks::GetScaledUpCapacity (6 * u.size () / 5)); // most chars just written as character, but some take 5 characters, so WAG about percentage
    for (char32_t c : u) {
        switch (c) {
            case '&':
                r += "&amp;";
                break;
            case '<':
                r += "&lt;";
                break;
            case '>':
                r += "&gt;";
                break;
            case '\"':
                r += "&quot;";
                break;
            case '\'':
                r += "&apos;";
                break;
            default:
                AppendCharacter_ (&r, c, true);
                break;
        }
    }
    return r;
}

string XML::QuoteForXMLAttribute (const optional<String>& s)
{
    if (not s.has_value ()) {
        return string{};
    }
    return QuoteForXMLAttribute (*s);
}

String XML::QuoteForXMLAttributeW (const String& s)
{
    string tmp = QuoteForXMLAttribute (s);
    return String::FromNarrowSDKString (tmp);
}

/*
 ********************************************************************************
 ******************************** QuoteForXML ***********************************
 ********************************************************************************
 */
string XML::QuoteForXML (const string& s)
{
    string r;
    r.reserve (s.size () * 6 / 5); // wild guess about good estimate
    for (auto i = s.begin (); i != s.end (); ++i) {
        switch (*i) {
            case '&':
                r += "&amp;";
                break;
            case '<':
                r += "&lt;";
                break;
            case '>':
                r += "&gt;";
                break;
            case '-': {
                // A 'dash' or 'minus-sign' can be dangerous in XML - if you get two in a row (start/end comment designator).
                // So avoid any leading/trailing dash, and any double-dashes.
                //
                // NB: This code WOULD be simpler if we just always mapped, but then much ordinary and safe text with dashes becomes
                // less readable (and produces huge diffs in my regression tests - but thats a one-time annoyance).
                //
                if ((i == s.begin ()) or (i + 1 == s.end ()) or (*(i - 1) == '-')) {
                    r += "&#45;";
                }
                else {
                    r.push_back ('-');
                }
            } break;
            default:
                r.push_back (*i);
                break;
        }
    }
    return r;
}

string XML::QuoteForXML (const wstring& s)
{
    return QuoteForXML (String{s});
}

String XML::QuoteForXMLW (const String& s)
{
    string tmp = QuoteForXML (s);
    return String::FromNarrowSDKString (tmp);
}

string XML::QuoteForXML (const String& s)
{
    u32string u = s.As<u32string> (); // code points, not code units - so a character outside the BMP is one reference, not two
    string    r;
    r.reserve (Containers::Support::ReserveTweaks::GetScaledUpCapacity (6 * u.size () / 5)); // most chars just written as character, but some take 5 characters, so WAG about percentage
    for (auto i = u.begin (); i != u.end (); ++i) {
        switch (*i) {
            case '&':
                r += "&amp;";
                break;
            case '<':
                r += "&lt;";
                break;
            case '>':
                r += "&gt;";
                break;
            case '-': {
                // A 'dash' or 'minus-sign' can be dangerous in XML - if you get two in a row (start/end comment designator).
                // So avoid any leading/trailing dash, and any double-dashes.
                //
                // NB: This code WOULD be simpler if we just always mapped, but then much ordinary and safe text with dashes becomes
                // less readable (and produces huge diffs in my regression tests - but thats a one-time annoyance).
                //
                if ((i == u.begin ()) or (i + 1 == u.end ()) or (*(i - 1) == '-')) {
                    r += "&#45;";
                }
                else {
                    r.push_back ('-');
                }
            } break;
            default:
                AppendCharacter_ (&r, *i, false);
                break;
        }
    }
    return r;
}

string XML::QuoteForXML (const optional<String>& s)
{
    if (not s.has_value ()) {
        return string{};
    }
    return QuoteForXML (*s);
}

/*
 ********************************************************************************
 ****************************** XML::Indenter ***********************************
 ********************************************************************************
 */
Indenter::Indenter (const String& indentText)
    : fTabS_{indentText.AsUTF8<string> ()}
    , fTabW_{indentText.As<wstring> ()}
{
}

void Indenter::Indent (unsigned indentLevel, ostream& out) const
{
    for (unsigned int i = 0; i < indentLevel; ++i) {
        out << fTabS_;
    }
}

void Indenter::Indent (unsigned int indentLevel, wostream& out) const
{
    for (unsigned int i = 0; i < indentLevel; ++i) {
        out << fTabW_;
    }
}

string XML::Format4XML (bool v)
{
    return v ? "true" : "false";
}
