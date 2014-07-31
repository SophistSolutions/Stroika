/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Format_h_
#define _Stroika_Foundation_Cryptography_Format_h_  1

#include    "../StroikaPreComp.h"

#include    <array>
#include    <cstdint>

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Streams/BinaryInputStream.h"



/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                /*
				 *	Generally, this Format function is used to take a BLOB and render it as a hex string.
				 *
                 *  DIGEST_RESULT_TYPE can be any of the uintN types (e.g. uint16_t, etc), or any of the defined
                 *  std::array<> types defined in Digest/ResultTypes.h.
                 *
                 *  AS_RESULT_TYPE can be:
                 *      o   Characters::String
                 *      o   std::string
                 */
                template    <typename DIGEST_RESULT_TYPE, typename AS_RESULT_TYPE = string>
                AS_RESULT_TYPE  Format (const DIGEST_RESULT_TYPE& digestResult);


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Format.inl"

#endif  /*_Stroika_Foundation_Cryptography_Format_h_*/
