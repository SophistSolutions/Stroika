/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_ 1

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

namespace Stroika::Foundation::Streams::InternallySynchronizedOutputStream {

    /**
     *  \note   Uses mutex, not recursive_mutex internally, so be carefully about mutual recursion calls in UNSYNC_REP.
     * 
     *  \par Example Usage
     *      \code
     *          Streams::OutputStream<byte>::Ptr syncStream = Streams::InternallySynchronizedOutputStream<byte>::New (otherOutputStreamToBeSharedAcrossThread);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename... ARGS>
    typename OutputStream<typename BASE_REP_TYPE::ElementType>::Ptr New (ARGS&&... args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_*/
