/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Streams/OutputStream.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Streams::InternallySynchronizedOutputStream {

    using OutputStream::Ptr;

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
     *          Streams::OutputStream::Ptr<byte> syncStream = Streams::InternallySynchronizedOutputStream<byte>::New (otherOutputStreamToBeSharedAcrossThread);
     *      \endcode
     *
     *  \note BASE_REP overload more efficient, and ::Ptr overload more flexible/simple to use.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename OPTIONS = DefaultOptions, typename... ARGS>
    Ptr<typename BASE_REP_TYPE::ElementType> New (const OPTIONS& o = {}, ARGS&&... args);
    template <typename ELEMENT_TYPE, typename OPTIONS = DefaultOptions>
    Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const OutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_*/
