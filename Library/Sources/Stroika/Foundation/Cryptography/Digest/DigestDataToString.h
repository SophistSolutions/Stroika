/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
 * 
 *          &&&DEPRECATED&&&&
 *          ALMOST EQUIVILENT TO just forcing return type from Digester to String (or string)
 *          AS OF Stroika 2.1b6
 *  && ONLY DIFF IS THIS USES Streams::iostream::SerializeItemToBLOB instead of DefaultSerializer
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *          &&&& CONSIDER DEPRECATING &&&&&
     */
    template <typename DIGESTER, typename T, typename RESULT_TYPE = string>
    [[deprecated ("Since Stroika 2.1b6 use Digester<> directly or Hash - this is equivilent to Hash<T,DIGESTER,RESULT_TYPE,SERIALIZR=Streams::iostream::SerializeItemToBLOB)")]] RESULT_TYPE DigestDataToString (const T& t);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DigestDataToString.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_*/
