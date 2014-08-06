/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_
#define _Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#include    "../../Streams/iostream/SerializeItemToBLOB.h"

#include    "../Format.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                template    <typename DIGESTER, typename T>
                string  DigestDataToString (const T& t)
                {
                    Format (DIGESTER::ComputeDigest (Streams::iostream::SerializeItemToBLOB (t)));
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Digest_DigestDataToString_inl_*/
