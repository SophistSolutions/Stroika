/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_
#define _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_ 1

#include "../../StroikaPreComp.h"

#include <iostream>

#include "../../Characters/String.h"

/**
 * TODO:
 *
 *      @todo   Completley untested....
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                using Characters::String;

                template <class charT, typename Traits>
                basic_istream<charT, Traits>& operator>> (basic_istream<charT, Traits>& is, String& to);
                template <class charT, typename Traits>
                basic_ostream<charT, Traits>& operator<< (basic_ostream<charT, Traits>& os, const String& from);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CommonInsertersAndExtractors.inl"

#endif /*_Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_*/
