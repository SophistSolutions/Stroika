/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     *************************** HTTP::ContentCoding ********************************
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

}
