/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
#ifndef _Stroika_Foundation_Streams_LoggingInputOutputStream_h_
#define _Stroika_Foundation_Streams_LoggingInputOutputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/InputOutputStream.h"

/**
*  \file
*
*  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
*
*/
namespace Stroika::Foundation::Streams::LoggingInputOutputStream {

    using InputOutputStream::Ptr;

    /**
     *  \par Example Usage
     *      \code
     *      \endcode
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                           const typename OutputStream::Ptr<ELEMENT_TYPE>& logInput, const typename OutputStream::Ptr<ELEMENT_TYPE>& logOutput);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                           const typename OutputStream::Ptr<ELEMENT_TYPE>& logInput, const typename OutputStream::Ptr<ELEMENT_TYPE>& logOutput);

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "LoggingInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_LoggingInputOutputStream_h_*/
