/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late</a>
 */

namespace Stroika::Foundation::Streams::InternallySynchronizedInputOutputStream {

    using InputOutputStream::Ptr;

    /**
     */
    struct DefaultOptions {
        /**
         * Default to using non-recursive simple mutex (so beware of one rep method calling another - if you do, override this argument to New).
         */
        using MutexType = mutex;
    };

    /**
     *  Helper to wrap an existing stream in synchronized wrapper - like @see Execution::Synchronized - except this wrape
     *  the underlying pointed to stream, not the external smart-pointer wrapper.
     *
     *  \par Example Usage
     *      \code
     *          Streams::InputOutputStream::Ptr<byte> syncStream = Streams::InternallySynchronizedInputOutputStream::New<UNSYNC_REP> (otherInputOutputStreamToBeSharedAcrossThread);
     *      \endcode
     * 
     *  \note BASE_REP overload more efficient, and ::Ptr overload more flexible/simple to use.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename OPTIONS = DefaultOptions, typename... ARGS>
    Ptr<typename BASE_REP_TYPE::ElementType> New (const OPTIONS& o = {}, ARGS&&... args);
    template <typename ELEMENT_TYPE, typename OPTIONS = DefaultOptions>
    Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const InputOutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_*/
