/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_WriterUtils_h_
#define _Stroika_Foundation_DataExchange_XML_WriterUtils_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Characters/String_Constant.h"
#include "../../Time/Date.h"
#include "../../Time/DateTime.h"
#include "../../Time/Duration.h"

#include "Common.h"

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

    /*
     */
    string QuoteForXMLAttribute (const string& s);
    string QuoteForXMLAttribute (const wstring& s); // encode non-ascii characters as entity-references
    string QuoteForXMLAttribute (const String& s);
    string QuoteForXMLAttribute (const optional<String>& s);

    /*
     */
    String QuoteForXMLAttributeW (const String& s);

    /*
     * This function only emits ascii characters (so makes no assumptions about the codepage used for writing xml). It
     * emits non-ascii characters as entity references.
     */
    string QuoteForXML (const string& s);
    string QuoteForXML (const wstring& s); // encode non-ascii characters as entity-references
    string QuoteForXML (const String& s);
    string QuoteForXML (const optional<String>& s);

    /*
     */
    String QuoteForXMLW (const String& s);

    class Indenter {
    public:
        Indenter (const String& indentText = Characters::String_Constant (L"\t"));

    public:
        nonvirtual void Indent (unsigned int indentLevel, ostream& out) const;
        nonvirtual void Indent (unsigned int indentLevel, wostream& out) const;

    private:
        string  fTabS_;
        wstring fTabW_;
    };

    /*
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
