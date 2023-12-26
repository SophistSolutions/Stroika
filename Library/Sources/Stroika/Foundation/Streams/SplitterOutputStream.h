/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SplitterOutputStream_h_
#define _Stroika_Foundation_Streams_SplitterOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Streams::SplitterOutputStream {

    using OutputStream::Ptr;

    /**
     *  A SplitterOutputStream wraps 2 output Streams,and duplicates all writes across the two.
     *
     *  This can can be used easily to produce logging for the communications along a stream.
     *
     *      \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  \par Example Usage
     *      \code
     *      \endcode
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut1, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut2);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut1,
                           const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut2);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SplitterOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_h_*/
