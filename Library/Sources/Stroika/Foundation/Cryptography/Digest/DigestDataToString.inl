/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_
#define _Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Streams/iostream/SerializeItemToBLOB.h"

#include "../Format.h"

namespace Stroika::Foundation::Cryptography::Digest {

    template <typename DIGESTER, typename T, typename RESULT_TYPE>
    RESULT_TYPE DigestDataToString (const T& t)
    {
        return Format<RESULT_TYPE, typename DIGESTER::ReturnType> (DIGESTER::ComputeDigest (Streams::iostream::SerializeItemToBLOB (t)));
    }
}
#endif /*_Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_*/
