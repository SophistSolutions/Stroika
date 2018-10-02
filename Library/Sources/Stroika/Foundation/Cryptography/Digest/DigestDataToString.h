/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_
#define _Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdint>

#include "../../Configuration/Common.h"
#include "../../Memory/BLOB.h"
#include "../../Streams/InputStream.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   Can we overload function templates? Diff # template parms? So RESULT_TYPE is second arg? We
 *              almost always want to infer the 'T' arg. But if you specify result type you don't want to
 *              lose ability to get that inference.
 *
 *      @todo   Change default return type for DigestDataToString<> to stroika string.
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  A simple helper function to make it easier (fewer characters) for the most common
     *  case, taking a data object and using operator << to insert it into a stream, and then
     *  taking THAT and digesting it, and taking its result to compute a string of the hash (hex).
     *
     *  \par Example Usage
     *      \code
     *          SourceDefinition    tmp;    // some struct which defines ostream operator>>
     *          string  digestStr = DigestDataToString<Digester<Algorithm::MD5>> (tmp);
     *      \endcode
     */
    template <typename DIGESTER, typename T, typename RESULT_TYPE = string>
    RESULT_TYPE DigestDataToString (const T& t);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DigestDataToString.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_*/
