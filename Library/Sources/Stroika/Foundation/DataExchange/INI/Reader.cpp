/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/Format.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/String2Float.h"
#include    "../../Characters/String2Int.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"
#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Characters::String_Constant;




/*
 ********************************************************************************
 ************************** DataExchange::INI::Reader ***************************
 ********************************************************************************
 */
class   DataExchange::INI::Reader::Rep_ : public DataExchange::Reader::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return _SharedPtrIRep (new Rep_ ());    // no instance data
    }
    virtual VariantValue    Read (const Streams::BinaryInputStream& in) override
    {
        return Read (Streams::TextInputStreamBinaryAdapter (in));
    }
    virtual VariantValue    Read (const Streams::TextInputStream& in) override
    {
        Profile  p;
        Optional<String>    readingSection;
        Section             currentSection;
        for (String line : in.ReadLines ()) {
            line = line.Trim ();
            if (line.StartsWith (L"[") and line.EndsWith (L"]")) {
                if (readingSection.IsPresent ()) {
                    p.fNamedSections.Add (*readingSection, currentSection);
                    currentSection.fProperties.clear ();
                }
                readingSection = line.CircularSubString (1, -1);
            }
            else if (line.StartsWith (L";")) {
                // drop comments on the floor
            }
            else if (line.Contains (L"=")) {
                size_t  i   =   line.Find ('=');
                Assert (i != String::npos);
                String key = line.SubString (0, i).Trim ();
                String value = line.SubString (i + 1).Trim ();
                if (value.StartsWith (L"\"") and value.EndsWith (L"\"")) {
                    value = value.CircularSubString (1, -1);
                }
                if (readingSection.IsPresent ()) {
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
        return Convert (p);
    }
};
DataExchange::INI::Reader::Reader ()
    : inherited (shared_ptr<_IRep> (new Rep_ ()))
{
}
