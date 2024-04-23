/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <string>

namespace Stroika::Foundation::Streams::iostream {

    template <typename charT, typename Traits>
    basic_istream<charT, Traits>& operator>> (basic_istream<charT, Traits>& in, String& to)
    {
        basic_string<charT, Traits> tmp;
        in >> tmp;
        to = tmp;
        return in;
    }
    template <typename charT, typename Traits>
    basic_ostream<charT, Traits>& operator<< (basic_ostream<charT, Traits>& out, const String& from)
    {
        basic_string<charT, Traits> tmp = from.As<basic_string<charT, Traits>> ();
        out << tmp;
        return out;
    }

}
