/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Streams/TextWriter.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Traversal::Iterable;
using Traversal::Iterator;
using namespace DataExchange::Variant;

/*
 ********************************************************************************
 *********************** DataExchange::Variant::INI::Writer *********************
 ********************************************************************************
 */
class INI::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ ()
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return ".ini"sv;
    }
    virtual void Write (const VariantValue& v, const OutputStream<byte>::Ptr& out) override
    {
        Write (v, TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude));
    }
    virtual void Write (const VariantValue& v, const OutputStream<Character>::Ptr& out) override
    {
        Write (Convert (v), out);
    }
    nonvirtual void Write (const Profile& profile, const OutputStream<Characters::Character>::Ptr& out) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::INI::Writer::Rep_::Write"};
#endif
        Write (nullopt, profile.fUnnamedSection, out);
        for (const auto& sectionKVP : profile.fNamedSections) {
            Write (sectionKVP.fKey, sectionKVP.fValue, out);
        }
    }
    nonvirtual void Write (const optional<String>& sectionName, const Section& profile, const OutputStream<Characters::Character>::Ptr& out) const
    {
        StringBuilder sb;
        if (sectionName) {
            sb << "[" << *sectionName << "]" << Characters::GetEOL<wchar_t> ();
        }
        for (const Common::KeyValuePair<String, String>& kvp : profile.fProperties) {
            sb << kvp.fKey << "=" << kvp.fValue << Characters::GetEOL<wchar_t> ();
        }
        out.WriteLn (sb.str ());
    }
};

INI::Writer::Writer ()
    : inherited{make_shared<Rep_> ()}
{
}

void INI::Writer::Write (const Profile& profile, const OutputStream<std::byte>::Ptr& out)
{
    return Write (profile, Streams::TextWriter::New (out));
}

void INI::Writer::Write (const Profile& profile, const OutputStream<Characters::Character>::Ptr& out)
{
    Debug::UncheckedDynamicCast<Rep_&> (_GetRep ()).Write (profile, out);
}

Memory::BLOB INI::Writer::WriteAsBLOB (const Profile& profile)
{
    return _WriteAsBLOBHelper ([&profile, this] (const OutputStream<std::byte>::Ptr& out) { Write (profile, out); });
}

String INI::Writer::WriteAsString (const Profile& profile)
{
    return _WriteAsStringHelper ([&profile, this] (const OutputStream<Characters::Character>::Ptr& out) { Write (profile, out); });
}
