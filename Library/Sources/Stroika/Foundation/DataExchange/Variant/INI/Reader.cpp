/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************************** DataExchange::INI::Reader ***************************
 ********************************************************************************
 */
class Variant::INI::Reader::Rep_ : public Variant::Reader::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return ".ini"sv;
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) override
    {
        return Read (Streams::TextReader::New (in));
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<Character>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::INI::Reader::Rep_::Read"};
#endif
        Profile          p;
        optional<String> readingSection;
        Section          currentSection;
        for (String line : in.ReadLines ()) {
            line = line.Trim ();
            if (line.StartsWith ("["sv) and line.EndsWith ("]"sv)) {
                if (readingSection.has_value ()) {
                    p.fNamedSections.Add (*readingSection, currentSection);
                    currentSection.fProperties.clear ();
                }
                readingSection = line.SubString (1, -1);
            }
            else if (line.StartsWith (";"sv)) {
                // drop comments on the floor
            }
            else if (line.Contains ("="sv)) {
                size_t i     = *line.Find ('=');
                String key   = line.SubString (0, i).Trim ();
                String value = line.SubString (i + 1).Trim ();
                if (value.StartsWith ("\""sv) and value.EndsWith ("\""sv)) {
                    value = value.SubString (1, -1);
                }
                if (readingSection.has_value ()) {
                    currentSection.fProperties.Add (key, value);
                }
                else {
                    p.fUnnamedSection.fProperties.Add (key, value);
                }
            }
            else {
                // @todo not sure what todo with other sorts of lines??
            }
        }
        if (readingSection.has_value ()) {
            p.fNamedSections.Add (*readingSection, currentSection);
        }
        return Convert (p);
    }
};
Variant::INI::Reader::Reader ()
    : inherited{make_shared<Rep_> ()}
{
}
