/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Jenkins_h_
#define _Stroika_Foundation_Cryptography_Hash_Jenkins_h_  1

#include    "../../StroikaPreComp.h"

#include    <cstdint>

#include    "Common.h"



/*
 * STATUS *****************DRAFT ..... ROUHGH OUTLINE
 *
 *		VERY ROUGH
 *
 *		SEE http://en.wikipedia.org/wiki/Jenkins_hash_function#cite_note-4
 *
 *		Several OTHERS there todo likewise - SPOOKY???
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


				// Just a name to select template implementation
				struct Jenkins {
				};


				template	<>
				struct Hasher<uint32_t,Jenkins> {
					static	uint32_t	Hash (const Streams::BinaryInputStream& from);
					static	uint32_t	Hash (const Byte* from, const Byte* to);
					template	<typename	PODTYPE>
						static	uint32_t	Hash (const PODTYPE& pod);
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
#include    "Jenkins.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Jenkins_h_*/
