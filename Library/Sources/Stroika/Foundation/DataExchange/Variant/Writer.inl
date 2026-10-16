/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant {

    /*
     ********************************************************************************
     ***************************** DataExchange::Writer *****************************
     ********************************************************************************
     */
    inline Writer::Writer (const shared_ptr<const _IRep>& rep)
        : fRep_{rep}
    {
        RequireNotNull (rep);
    }
    inline optional<filesystem::path> Writer::GetDefaultFileSuffix () const
    {
        return fRep_->GetDefaultFileSuffix ();
    }
    inline void Writer::Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const
    {
        fRep_->Write (v, out);
    }
    inline void Writer::Write (const VariantValue& v, const Streams::OutputStream::Ptr<Characters::Character>& out) const
    {
        fRep_->Write (v, out);
    }
    inline void Variant::Writer::Write (const VariantValue& v, ostream& out) const
    {
        Write (v, _WrapBinaryOutput (out));
    }
    inline void Variant::Writer::Write (const VariantValue& v, wostream& out) const
    {
        Write (v, _WrapTextOutput (out));
    }
    inline const Writer::_IRep& Writer::_ConstGetRep () const
    {
        AssertNotNull (fRep_);
        return *fRep_;
    }
    inline Streams::OutputStream::Ptr<byte> Writer::_WrapBinaryOutput (const Streams::OutputStream::Ptr<byte>& out)
    {
        return out;
    }
    inline Streams::OutputStream::Ptr<Characters::Character> Writer::_WrapTextOutput (const Streams::OutputStream::Ptr<Characters::Character>& out)
    {
        return out;
    }

}
