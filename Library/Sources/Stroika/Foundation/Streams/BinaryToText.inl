/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::BinaryToText::Reader {

    /*
     ********************************************************************************
     **************************** BinaryToText::Reader::New *******************************
     ********************************************************************************
     */
    inline auto New (const InputStream::Ptr<Character>& src) -> InputStream::Ptr<Character>
    {
        return src;
    }
    template <typename... ARGS>
    inline InputStream::Ptr<Character> New (Execution::InternallySynchronized internallySynchronized, ARGS... args)
    {
        switch (internallySynchronized) {
            case Execution::InternallySynchronized::eNotKnownInternallySynchronized:
                return New (forward<ARGS...> (args...));
            case Execution::eInternallySynchronized:
                // @todo could explicitly specialize more cases and handle more efficiently, but using the REP overload of InternallySynchronizedInputStream
                return InternallySynchronizedInputStream::New ({}, New (forward<ARGS...> (args...)));
        }
    }

}
