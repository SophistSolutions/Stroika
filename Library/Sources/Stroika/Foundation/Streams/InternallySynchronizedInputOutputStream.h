/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late</a>
 */

namespace Stroika::Foundation::Streams::InternallySynchronizedInputOutputStream {

    /**
     */
    struct DefaultOptions {
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
     *  \note   Uses mutex, not recursive_mutex internally, so be carefully about mutual recursion calls in UNSYNC_REP.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename OPTIONS = DefaultOptions, typename... ARGS>
    typename InputOutputStream::Ptr<typename BASE_REP_TYPE::ElementType> New (OPTIONS o = {}, ARGS&&... args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_*/
