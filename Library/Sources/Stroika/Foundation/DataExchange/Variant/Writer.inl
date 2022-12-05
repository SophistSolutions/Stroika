/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Writer_inl_
#define _Stroika_Foundation_DataExchange_Variant_Writer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
    inline void Writer::Write (const VariantValue& v, const Streams::OutputStream<std::byte>::Ptr& out)
    {
        fRep_->Write (v, out);
    }
    inline void Writer::Write (const VariantValue& v, const Streams::OutputStream<Characters::Character>::Ptr& out)
    {
        fRep_->Write (v, out);
    }
    inline void Variant::Writer::Write (const VariantValue& v, ostream& out)
    {
        Write (v, _WrapBinaryOutput (out));
    }
    inline void Variant::Writer::Write (const VariantValue& v, wostream& out)
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
    inline Streams::OutputStream<std::byte>::Ptr _WrapBinaryOutput (const Streams::OutputStream<std::byte>::Ptr& out)
    {
        return out;
    }
    inline Streams::OutputStream<Characters::Character>::Ptr _WrapTextOutput (const Streams::OutputStream<Characters::Character>::Ptr& out)
    {
        return out;
    }

}

#endif /*_Stroika_Foundation_DataExchange_Variant_Writer_inl_*/
