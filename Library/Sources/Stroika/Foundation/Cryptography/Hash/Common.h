/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Common_h_
#define _Stroika_Foundation_Cryptography_Hash_Common_h_  1

#include    "../../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputStream.h"




/*
 * STATUS *****************DRAFT ..... ROUHGH OUTLINE
 *
 * TODO:
 *      o
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                using   Memory::Byte;


                template    <typename RETURN_TYPE, typename ALGORITHM>
                struct Hasher {
                    typedef typename    RETURN_TYPE ReturnType;

                    static  ReturnType  Hash (const Streams::BinaryInputStream& from);
                    static  ReturnType  Hash (const Byte* from, const Byte* to);

#if 0
                    // maybe can get rid of this - but maybe handy to specialize??? For common case of
                    template    <typename   PODTYPE>
                    static  ReturnType  Hash (const PODTYPE& pod);
#endif

                };


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Common_h_*/
