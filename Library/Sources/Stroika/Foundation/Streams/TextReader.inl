/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::TextReader {

    /*
     ********************************************************************************
     ***************************** TextReader::New **********************************
     ********************************************************************************
     */
    inline auto New (const InputStream::Ptr<Character>& src) -> Ptr
    {
        return src;
    }
    template <typename... ARGS>
    inline Ptr New (Execution::InternallySynchronized internallySynchronized, ARGS... args)
    {
        switch (internallySynchronized) {
            case Execution::InternallySynchronized::eNotKnownInternallySynchronized:
                return New (forward<ARGS...> (args...));
            case Execution::eInternallySynchronized:
                // @todo could explicitly specialize more cases and handle more efficiently, but using the REP loverload of InternallySynchronizedInputStream
                return InternallySynchronizedInputStream::New ({}, New (forward<ARGS...> (args...)));
        }
    }

}
