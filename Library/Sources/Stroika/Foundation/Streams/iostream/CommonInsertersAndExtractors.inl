/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_inl_
#define _Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <string>

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                template <class charT, typename Traits>
                std::basic_istream<charT, Traits>& operator>> (std::basic_istream<charT, Traits>& in, String& to)
                {
                    basic_string<charT, Traits> tmp;
                    in >> tmp;
                    to = tmp;
                    return in;
                }
                template <class charT, typename Traits>
                std::basic_ostream<charT, Traits>& operator<< (std::basic_ostream<charT, Traits>& out, const String& from)
                {
                    basic_string<charT, Traits> tmp = from.As<basic_string<charT, Traits>> ();
                    out << tmp;
                    return out;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_iostream_CommonInsertersAndExtractors_inl_*/
