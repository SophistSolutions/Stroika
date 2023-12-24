/*
* Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
*/
#ifndef _Stroika_Foundation_Streams_LoggingInputOutputStream_h_
#define _Stroika_Foundation_Streams_LoggingInputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "InputOutputStream.h"
#include "InternallySynchronizedInputOutputStream.h"

/**
*  \file
*
*  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
*
*/
namespace Stroika::Foundation::Streams::LoggingInputOutputStream {

    /**
     */
    //template <typename ELEMENT_TYPE>
    //    using ElementType = typename Stream<ELEMENT_TYPE>::ElementType;

    template <typename ELEMENT_TYPE>
    /**
     */
    using Ptr = typename InputOutputStream::Ptr<ELEMENT_TYPE>;

    /**
     *  \par Example Usage
     *      \code
     *      \endcode
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                           const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                           const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);

    template <typename ELEMENT_TYPE>
    class Rep_;

#if 0
        // tmp disable til we can fix --LGP2 2023-12-22
        template <typename ELEMENT_TYPE>
        using InternalSyncRep_ =
            InternallySynchronizedInputOutputStream<ELEMENT_TYPE, Streams::LoggingInputOutputStream<ELEMENT_TYPE>, typename LoggingInputOutputStream<ELEMENT_TYPE>::Rep_>;
#endif

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "LoggingInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_LoggingInputOutputStream_h_*/
