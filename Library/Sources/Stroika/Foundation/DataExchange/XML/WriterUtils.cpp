/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <ostream>

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"

#include "WriterUtils.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

/*
 ********************************************************************************
 ****************************** QuoteForXMLAttribute ****************************
 ********************************************************************************
 */
string XML::QuoteForXMLAttribute (const string& s)
{
    string r;
    r.reserve (s.size () * 6 / 5); // wild guess about good estimate
    for (auto i = s.begin (); i != s.end (); ++i) {
        Containers::ReserveSpeedTweekAdd1 (r);
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
    string r;
    r.reserve (s.size () * 7 / 5); // wild guess about good estimate
    for (auto i = s.begin (); i != s.end (); ++i) {
        Containers::ReserveSpeedTweekAdd1 (r);
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
            default: {
                wchar_t ccode = *i;
                if (ccode != '\t' and ccode != '\n' and ccode != '\r' and (ccode < 32 or ccode > 127)) {
                    r += CString::Format ("&#%d;", ccode);
                }
                else {
                    r.push_back (static_cast<char> (ccode));
                }
            } break;
        }
    }
    return r;
}

string XML::QuoteForXMLAttribute (const String& s)
{
    return QuoteForXMLAttribute (s.As<wstring> ());
}

string XML::QuoteForXMLAttribute (const optional<String>& s)
{
    if (not s.has_value ()) {
        return string ();
    }
    return QuoteForXMLAttribute (*s);
}

String XML::QuoteForXMLAttributeW (const String& s)
{
    string tmp = QuoteForXMLAttribute (s);
    return NarrowSDKStringToWide (tmp);
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
        Containers::ReserveSpeedTweekAdd1 (r);
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
    string r;
    r.reserve (s.size () * 7 / 5); // wild guess about good estimate
    for (auto i = s.begin (); i != s.end (); ++i) {
        Containers::ReserveSpeedTweekAdd1 (r);
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
            default: {
                wchar_t ccode = *i;
                if (ccode != '\t' and ccode != '\n' and ccode != '\r' and (ccode < 32 or ccode > 127)) {
                    r += CString::Format ("&#%d;", ccode);
                }
                else {
                    r.push_back (static_cast<char> (ccode));
                }
            } break;
        }
    }
    return r;
}

String XML::QuoteForXMLW (const String& s)
{
    string tmp = QuoteForXML (s);
    return NarrowSDKStringToWide (tmp);
}

string XML::QuoteForXML (const String& s)
{
    return QuoteForXML (s.As<wstring> ());
}

string XML::QuoteForXML (const optional<String>& s)
{
    if (not s.has_value ()) {
        return string ();
    }
    return QuoteForXML (*s);
}

/*
 ********************************************************************************
 ****************************** XML::Indenter ***********************************
 ********************************************************************************
 */
Indenter::Indenter (const String& indentText)
    : fTabS_ (indentText.AsUTF8<string> ())
    , fTabW_ (indentText.As<wstring> ())
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
