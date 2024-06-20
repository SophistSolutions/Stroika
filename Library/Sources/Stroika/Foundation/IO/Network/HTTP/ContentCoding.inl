/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ***************************** HTTP::ContentCoding ******************************
     ********************************************************************************
     */
    inline ContentCoding::ContentCoding (AtomType a)
        : fRep_{a}
    {
    }
    template <Characters::IConvertibleToString STRING_LIKE>
    inline ContentCoding::ContentCoding (STRING_LIKE&& name)
        : fRep_{forward<STRING_LIKE> (name)}
    {
    }
    inline String ContentCoding::ToString () const
    {
        return Characters::ToString (fRep_);
    }
    const inline ContentCoding ContentCoding::kCompress{"compress"sv};
    const inline ContentCoding ContentCoding::kDeflate{"deflate"sv};
    const inline ContentCoding ContentCoding::kGZip{"gzip"sv};
    const inline ContentCoding ContentCoding::kIdentity{"identity"sv};

}
