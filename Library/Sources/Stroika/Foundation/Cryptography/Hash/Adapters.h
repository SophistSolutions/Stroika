/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Adapters_h_
#define _Stroika_Foundation_Cryptography_Hash_Adapters_h_  1

#include    "../../StroikaPreComp.h"

#include    <cstdint>

#include    "../../Characters/String.h"
#include    "../../Memory/BLOB.h"

#include    "Common.h"



/*
 * STATUS *****************DRAFT ..... ROUHGH OUTLINE
 *
 *      VERY ROUGH
 *
 *
 *
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



                /**
                // NOT SURE HOWT TODO
                // BUT IDEA IS PROBBALY USE  ATEMPLATE WHICH OWRKS ON ANY TTYPE.
                // DO SPECIAILIZER THAT ASKS "ispodtype' or "has-interstitial-space' - and then do default series of bytes hash on it
                // and do special ones for important types like date, String, BLOB, and perhaps a few others.
                //
                // Must somehow arrange so code that #includes other module like String and so on is just in the .CPP file - not
                // i the header file -s o we do force too many uneeded includes
                //
                //  o   Adapters take other types – like string, or stream, etc, and maybe template things todo
                //  other types, and let use plug in an int-based algorithm, and ??

                // VAGUELY loke this - but also need has ptr arg somehow ...
                 */
                template    <typename HASHER_TYPE, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename HASHER_TYPE::ReturnType>
                HASH_RETURN_TYPE  HashAdapter (TYPE_TO_COMPUTE_HASH_OF data2Hash);


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Adapters.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Jenkins_h_*/
