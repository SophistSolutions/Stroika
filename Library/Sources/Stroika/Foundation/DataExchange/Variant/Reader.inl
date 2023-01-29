/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Reader_inl_
#define _Stroika_Foundation_DataExchange_Variant_Reader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Streams/TextToByteReader.h"

namespace Stroika::Foundation::DataExchange::Variant {

    /*
     ********************************************************************************
     ********************* Variant::Reader::_Rep_Cloner *****************************
     ********************************************************************************
     */
    inline Reader::_SharedPtrIRep Reader::_Rep_Cloner::operator() (const _IRep& t) const { return t.Clone (); }

    /*
     ********************************************************************************
     ******************************* Variant::Reader ********************************
     ********************************************************************************
     */
    inline Reader::Reader (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
        RequireNotNull (rep);
    }
    inline String Reader::GetDefaultFileSuffix () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetDefaultFileSuffix ();
    }
    inline VariantValue Reader::Read (const Streams::InputStream<std::byte>::Ptr& in)
    {
        AssertNotNull (fRep_);
        return fRep_->Read (in);
    }
    inline VariantValue Reader::Read (const Streams::InputStream<Characters::Character>::Ptr& in)
    {
        AssertNotNull (fRep_);
        return fRep_->Read (in);
    }
    inline VariantValue Variant::Reader::Read (const Traversal::Iterable<Characters::Character>& in)
    {
        return Read (_ToCharacterReader (in));
    }
    inline VariantValue   Variant::Reader::Read (const Memory::BLOB& in) { return Read (_ToByteReader (in)); }
    inline VariantValue   Variant::Reader::Read (istream& in) { return Read (_ToByteReader (in)); }
    inline VariantValue   Variant::Reader::Read (wistream& in) { return Read (_ToCharacterReader (in)); }
    inline Reader::_IRep& Reader::_GetRep ()
    {
        AssertNotNull (fRep_);
        EnsureNotNull (fRep_.rwget ());
        return *fRep_.rwget ();
    }
    inline const Reader::_IRep& Reader::_GetRep () const
    {
        EnsureNotNull (fRep_.cget ());
        return *fRep_.cget ();
    }
    inline Streams::InputStream<std::byte>::Ptr Reader::_ToByteReader (const Streams::InputStream<std::byte>::Ptr& in) { return in; }
    inline Streams::InputStream<std::byte>::Ptr Reader::_ToByteReader (const Streams::InputStream<Characters::Character>::Ptr& in)
    {
        return Streams::TextToByteReader::New (in);
    }
    inline Streams::InputStream<std::byte>::Ptr Reader::_ToByteReader (const Memory::BLOB& in)
    {
        return in.As<Streams::InputStream<std::byte>::Ptr> ();
    }
    inline Streams::InputStream<Characters::Character>::Ptr Reader::_ToCharacterReader (const Streams::InputStream<Characters::Character>::Ptr& in)
    {
        return in;
    }

}

#endif /*_Stroika_Foundation_DataExchange_Variant_Reader_inl_*/
