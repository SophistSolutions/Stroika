/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_ 1

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
 *
 *
 */

namespace Stroika::Foundation::Streams::InternallySynchronizedInputStream {

    /**
     *
     *  \note   Uses mutex, not recursive_mutex internally, so be carefully about mutual recursion calls in UNSYNC_REP.
     * 
     *  \par Example Usage
     *      \code
     *          Streams::InputStream<byte>::Ptr syncStream = Streams::InternallySynchronizedInputStream<byte>::New (otherInputStreamToBeSharedAcrossThread);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename... ARGS>
    typename InputStream<typename BASE_REP_TYPE::ElementType>::Ptr New (ARGS&&... args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_*/
