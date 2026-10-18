/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_WriterUtils_h_
#define _Stroika_Foundation_DataExchange_XML_WriterUtils_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

/*
 * TODO:
 *       @todo   Short term- convert to using only Stroika string class (but make easy to still leverage from HF -
 *               think this out carefully befor change)
 *
 *       @todo   Harmonize the WriteQuoted/Format4XML routines - probably just switching to the later.
 *               Maybe add optional params for some types (control enums).
 *               Support dates, and VariantValue, and optional, etc...
 */

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;

    /**
     *  \brief Whether s is an NCName (https://www.w3.org/TR/xml-names/#NT-NCName) - an XML name with no ':', as a local name and
     *         a namespace prefix must each be
     */
    bool IsNCName (const String& s);

    /**
     *  \brief s as an attribute's value, in ASCII: each character spelled so a parser reads it back as that character
     *
     *  So a character not ASCII is a reference to its code point (one, outside the BMP too), and so is a TAB, LF or CR, which a
     *  parser would otherwise read as a space (attribute-value normalization). Throws DataExchange::BadFormatException on a
     *  character XML 1.0 cannot hold at all, not even as a reference (U+0000 to U+001F but TAB, LF and CR, U+FFFE, U+FFFF).
     *
     *  \note The string overload passes s's bytes through, quoting only & < > " and ' - an encoding it cannot know.
     */
    string QuoteForXMLAttribute (const string& s);
    string QuoteForXMLAttribute (const wstring& s); ///< \brief s as an attribute's value, in ASCII, each character as a parser reads it back
    string QuoteForXMLAttribute (const String& s); ///< \brief s as an attribute's value, in ASCII, each character as a parser reads it back
    string QuoteForXMLAttribute (const optional<String>& s); ///< \brief s as an attribute's value - nothing, where s is missing

    /**
     */
    String QuoteForXMLAttributeW (const String& s);

    /**
     *  \brief s as an element's text, in ASCII (so assuming nothing of the encoding the XML is written in): each character
     *         spelled so a parser reads it back as that character
     *
     *  So a character not ASCII is a reference to its code point (one, outside the BMP too), and so is a CR, which a parser
     *  would otherwise read as LF (line-end normalization). Throws DataExchange::BadFormatException on a character XML 1.0
     *  cannot hold at all, not even as a reference (U+0000 to U+001F but TAB, LF and CR, U+FFFE, U+FFFF).
     *
     *  \note The string overload passes s's bytes through, quoting only & < > and dashes - an encoding it cannot know.
     */
    string QuoteForXML (const string& s);
    string QuoteForXML (const wstring& s);          ///< \brief s as an element's text, in ASCII, each character as a parser reads it back
    string QuoteForXML (const String& s);           ///< \brief s as an element's text, in ASCII, each character as a parser reads it back
    string QuoteForXML (const optional<String>& s); ///< \brief s as an element's text - nothing, where s is missing

    /**
     */
    String QuoteForXMLW (const String& s);

    class Indenter {
    public:
        Indenter (const String& indentText = "\t"sv);

    public:
        nonvirtual void Indent (unsigned int indentLevel, ostream& out) const;
        nonvirtual void Indent (unsigned int indentLevel, wostream& out) const;

    private:
        string  fTabS_;
        wstring fTabW_;
    };

    /**
     * Format values for XML output.
     *      PROBABLY should map QuoteForXML to this overloaded name?
     *
     * Note - this assumes we're outputting using UTF8.
     *
     * SHOULD probably include Memory::Optiona<> variations on all these as well - so we can fairly freely just say "Foramt4XML"
     */
    string Format4XML (bool v);
    //string    Format4XML (Time::Date v);      NYI
    //string    Format4XML (Time::DateTime v);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WriterUtils.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_WriterUtils_h_*/
