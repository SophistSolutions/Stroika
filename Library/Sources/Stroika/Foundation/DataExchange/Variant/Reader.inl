/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Streams/TextToBinary.h"

namespace Stroika::Foundation::DataExchange::Variant {

    /*
     ********************************************************************************
     ********************* Variant::Reader::_Rep_Cloner *****************************
     ********************************************************************************
     */
    inline Reader::_SharedPtrIRep Reader::_Rep_Cloner::operator() (const _IRep& t) const
    {
        return t.Clone ();
    }

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
    inline optional<filesystem::path> Reader::GetDefaultFileSuffix () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetDefaultFileSuffix ();
    }
    inline VariantValue Reader::Read (const Streams::InputStream::Ptr<byte>& in) const
    {
        AssertNotNull (fRep_);
        return fRep_->Read (in);
    }
    inline VariantValue Reader::Read (const Streams::InputStream::Ptr<Characters::Character>& in) const
    {
        AssertNotNull (fRep_);
        return fRep_->Read (in);
    }
    inline VariantValue Variant::Reader::Read (const Traversal::Iterable<Characters::Character>& in) const
    {
        return Read (_ToCharacterReader (in));
    }
    inline VariantValue Variant::Reader::Read (istream& in) const
    {
        return Read (_ToByteReader (in));
    }
    inline VariantValue Variant::Reader::Read (wistream& in) const
    {
        return Read (_ToCharacterReader (in));
    }
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
    inline Streams::InputStream::Ptr<byte> Reader::_ToByteReader (const Streams::InputStream::Ptr<byte>& in)
    {
        return in;
    }
    inline Streams::InputStream::Ptr<byte> Reader::_ToByteReader (const Streams::InputStream::Ptr<Characters::Character>& in)
    {
        return Streams::TextToBinary::Reader::New (in);
    }
    inline Streams::InputStream::Ptr<Characters::Character> Reader::_ToCharacterReader (const Streams::InputStream::Ptr<Characters::Character>& in)
    {
        return in;
    }

}
