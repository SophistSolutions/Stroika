/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_MD5_h_
#define _Stroika_Foundation_Cryptography_MD5_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"


/*
 *  TODO
 *      @todo   FILE SOON TO BE DEPRECTED - USE ADAPTER CODE AND Algorithm/MD5 (see impl)
 *              But need to improve generic accessors / adapters first, before we deprecate here!
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {


            using   Memory::Byte;


            // Returns 16 character hex string
            string  ComputeMD5Digest (const Byte* s, const Byte* e);
            string  ComputeMD5Digest (const vector<Byte>& b);

            template    <typename T>
            string  ComputeMD5Digest_UsingOStream (const T& elt);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MD5.inl"

#endif  /*_Stroika_Foundation_Cryptography_MD5_h_*/
