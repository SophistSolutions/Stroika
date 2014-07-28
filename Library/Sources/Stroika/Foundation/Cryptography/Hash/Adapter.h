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
				 *	This function applies any (argument) Hash function (HASHER_TYPE) to the given data type, and
				 *	returns the argument hash value.
				 *
				 *	Hash algorithms work on BLOBs, and generate return (often longish) integers, often encoded
				 *	as strings and such.
				 *
				 *	This Adpater takes care of the general part of mapping the inputs and outputs to/from
				 *	common forms, and then makes generic the actual hash computing algorithm.
				 *
				 *	EXAMPLE USAGE:
				 *		xx;
				 */
                template    <typename HASHER_TYPE, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename HASHER_TYPE::ReturnType>
                HASH_RETURN_TYPE  Adapter (TYPE_TO_COMPUTE_HASH_OF data2Hash);


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Adapter.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Jenkins_h_*/
