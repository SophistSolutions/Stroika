/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_inl_
#define _Stroika_Foundation_Streams_BinaryInputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            inline  BinaryInputStream::BinaryInputStream () {
            }
            inline  BinaryInputStream::~BinaryInputStream () {
            }
            inline  size_t  BinaryInputStream::Read (Byte* intoStart, Byte* intoEnd) {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                return _Read (intoStart, intoEnd);
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_inl_*/
