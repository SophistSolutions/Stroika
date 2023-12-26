/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_inl_
#define _Stroika_Foundation_Streams_TextReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
                return New (forward<ARGS> (args...));
            case Execution::InternallySynchronized::eInternallySynchronized:
                // @todo could explicitly specialize more cases and handle more efficiently, but using the REP loverload of InternallySynchronizedInputStream
                return InternallySynchronizedInputStream::New ({}, New (forward<ARGS> (args...)));
        }
    }

}

#endif /*_Stroika_Foundation_Streams_TextReader_inl_*/
