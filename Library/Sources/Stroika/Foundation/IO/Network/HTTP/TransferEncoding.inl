/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/IO/Network/HTTP/ContentEncoding.h"

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     *************************** HTTP::TransferEncoding *****************************
     ********************************************************************************
     */
    inline TransferEncoding::TransferEncoding (AtomType a)
        : fRep_{a}
    {
    }
    template <Characters::IConvertibleToString STRING_LIKE>
    inline TransferEncoding::TransferEncoding (STRING_LIKE&& name)
        : fRep_{forward<STRING_LIKE> (name)}
    {
    }
    inline String TransferEncoding::ToString () const
    {
        return Characters::ToString (fRep_);
    }
    const inline TransferEncoding TransferEncoding::kChunked{"chunked"sv};
    const inline TransferEncoding TransferEncoding::kCompress{ContentEncoding::kCompress.As<AtomType> ()};
    const inline TransferEncoding TransferEncoding::kDeflate{ContentEncoding::kDeflate.As<AtomType> ()};
    const inline TransferEncoding TransferEncoding::kGZip{ContentEncoding::kGZip.As<AtomType> ()};
    const inline TransferEncoding TransferEncoding::kIdentity{ContentEncoding::kIdentity.As<AtomType> ()};

    /*
     ********************************************************************************
     ****************************** HTTP::TransferCodings ***************************
     ********************************************************************************
     */
    inline TransferEncodings::TransferEncodings (const Traversal::Iterable<TransferEncoding>& src)
        : Sequence<TransferEncoding>{src}
    {
    }
    inline TransferEncodings::TransferEncodings (TransferEncoding tc)
        : Sequence<TransferEncoding>{tc}
    {
    }

}
