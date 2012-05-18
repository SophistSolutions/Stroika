/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_inl_
#define _Stroika_Foundation_Streams_BinaryOutputStream_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            inline  BinaryOutputStream::BinaryOutputStream ()
            {
            }
            inline  BinaryOutputStream::~BinaryOutputStream ()
            {
            }
            inline  void    BinaryOutputStream::Write (const Byte* start, const Byte* end)
            {
                RequireNotNull (start);
                Require ((end - start) >= 1);
                _Write (start, end);
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
