/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

namespace Stroika::Foundation::Streams {

    /**
     *  A SplitterOutputStream wraps 2 output Streams,and duplicates all writes across the two.
     *
     *  This can can be used easily to produce logging for the communications along a stream.
     *
     *      \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    class SplitterOutputStream : public OutputStream<ELEMENT_TYPE> {
    public:
        SplitterOutputStream ()                            = delete;
        SplitterOutputStream (const SplitterOutputStream&) = delete;

    public:
        /**
         *  There are no special methods to SplitterOutputStreamPtr, so we just re-use the baseclass Ptr smart pointer.
         */
        using typename OutputStream<ELEMENT_TYPE>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *      \endcode
         */
        static Ptr New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);
        static Ptr New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1,
                        const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);

    private:
        class Rep_;

    private:
        using InternalSyncRep_ =
            InternallySynchronizedOutputStream<ELEMENT_TYPE, Streams::SplitterOutputStream<ELEMENT_TYPE>, typename SplitterOutputStream<ELEMENT_TYPE>::Rep_>;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SplitterOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_h_*/
