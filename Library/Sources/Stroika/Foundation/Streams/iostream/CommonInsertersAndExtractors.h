/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_
#define _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_   1

#include    "../../StroikaPreComp.h"

#include    <iostream>

#include    "../../Characters/String.h"


/*
 * TODO:
 *      o
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                using   Characters::String;


                template<class charT, class Traits>
                std::basic_istream<charT, Traits>&  operator>> (std::basic_istream<charT, Traits>& is, String& to);
                template<class charT, class Traits>
                std::basic_ostream<charT, Traits>&  operator<< (std::basic_ostream<charT, Traits>& os,  const String& from);


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_h_*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CommonInsertersAndExtractors.inl"
