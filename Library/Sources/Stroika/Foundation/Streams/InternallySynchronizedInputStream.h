/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>

#include "InputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Streams::InternallySynchronizedInputStream {

    using InputStream::Ptr;

    /**
     */
    struct DefaultOptions {
        /**
         * Default to using non-recursive simple mutex (so beware of one rep method calling another - if you do, override this argument to New).
         */
        using MutexType = mutex;
    };

    /**
     *  \par Example Usage
     *      \code
     *          Streams::InputStream::Ptr<byte> syncStream = Streams::InternallySynchronizedInputStream::New<byte> (otherInputStreamToBeSharedAcrossThread);
     *      \endcode
     *
     *  \note BASE_REP overload more efficient, and ::Ptr overload more flexible/simple to use.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename OPTIONS = DefaultOptions, typename... ARGS>
    Ptr<typename BASE_REP_TYPE::ElementType> New (const OPTIONS& o = {}, ARGS&&... args);
    template <typename ELEMENT_TYPE, typename OPTIONS = DefaultOptions>
    Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const InputStream::Ptr<ELEMENT_TYPE>& stream2Wrap);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_*/
