/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ***************************** HTTP::ContentEncoding ****************************
     ********************************************************************************
     */
    inline ContentEncoding::ContentEncoding (AtomType a)
        : fRep_{a}
    {
    }
    template <Characters::IConvertibleToString STRING_LIKE>
    inline ContentEncoding::ContentEncoding (STRING_LIKE&& name)
        : fRep_{forward<STRING_LIKE> (name)}
    {
    }
    inline String ContentEncoding::ToString () const
    {
        return Characters::ToString (fRep_);
    }
    const inline ContentEncoding ContentEncoding::kCompress{"compress"sv};
    const inline ContentEncoding ContentEncoding::kDeflate{"deflate"sv};
    const inline ContentEncoding ContentEncoding::kGZip{"gzip"sv};
    const inline ContentEncoding ContentEncoding::kIdentity{"identity"sv};

    /*
     ********************************************************************************
     ****************************** HTTP::ContentEncodings **************************
     ********************************************************************************
     */
    inline ContentEncodings::ContentEncodings (const Traversal::Iterable<ContentEncoding>& src)
        : Sequence<ContentEncoding>{src}
    {
    }
    inline ContentEncodings::ContentEncodings (ContentEncoding tc)
        : Sequence<ContentEncoding>{tc}
    {
    }

}
