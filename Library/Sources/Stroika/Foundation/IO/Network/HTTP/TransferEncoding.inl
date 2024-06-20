/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/ToString.h"

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
    const inline TransferEncoding TransferEncoding::kCompress{"compress"sv};
    const inline TransferEncoding TransferEncoding::kDeflate{"deflate"sv};
    const inline TransferEncoding TransferEncoding::kGZip{"gzip"sv};
    const inline TransferEncoding TransferEncoding::kIdentity{"identity"sv};

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
