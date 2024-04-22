/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant {

    /*
     ********************************************************************************
     ********************** DataExchange::Writer::_Rep_Cloner ***********************
     ********************************************************************************
     */
    inline Writer::_SharedPtrIRep Writer::_Rep_Cloner::operator() (const _IRep& t) const
    {
        return t.Clone ();
    }

    /*
     ********************************************************************************
     ***************************** DataExchange::Writer *****************************
     ********************************************************************************
     */
    inline Writer::Writer (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
    }
    inline String Writer::GetDefaultFileSuffix () const
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
    inline Writer::_IRep& Writer::_GetRep ()
    {
        EnsureNotNull (fRep_.rwget ());
        return *fRep_.rwget ();
    }
    inline const Writer::_IRep& Writer::_GetRep () const
    {
        EnsureNotNull (fRep_.cget ());
        return *fRep_.cget ();
    }
    inline Streams::OutputStream::Ptr<byte> _WrapBinaryOutput (const Streams::OutputStream::Ptr<byte>& out)
    {
        return out;
    }
    inline Streams::OutputStream::Ptr<Characters::Character> _WrapTextOutput (const Streams::OutputStream::Ptr<Characters::Character>& out)
    {
        return out;
    }

}
