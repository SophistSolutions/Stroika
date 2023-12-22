/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SplitterOutputStream_h_
#define _Stroika_Foundation_Streams_SplitterOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "InternallySynchronizedOutputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Streams::SplitterOutputStream {

    /**
     *  A SplitterOutputStream wraps 2 output Streams,and duplicates all writes across the two.
     *
     *  This can can be used easily to produce logging for the communications along a stream.
     *
     *      \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
        /**
         *  There are no special methods to SplitterOutputStreamPtr, so we just re-use the baseclass Ptr smart pointer.
         */
    template <typename ELEMENT_TYPE>
        using Ptr = typename OutputStream<ELEMENT_TYPE>::Ptr;

        /**
         *  \par Example Usage
         *      \code
         *      \endcode
         */
        template <typename ELEMENT_TYPE>
        Ptr < ELEMENT_TYPE> New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);
        template <typename ELEMENT_TYPE>
            Ptr < ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1,
                        const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);

        template <typename ELEMENT_TYPE>
        class Rep_;

        // @todo must reimplmenet intenrallysync
        #if 0
        template <typename ELEMENT_TYPE>
        using InternalSyncRep_ =
            InternallySynchronizedOutputStream<ELEMENT_TYPE, Streams::SplitterOutputStream<ELEMENT_TYPE>, typename SplitterOutputStream<ELEMENT_TYPE>::Rep_>;
        #endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SplitterOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_h_*/
