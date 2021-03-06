/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
namespace Stroika::Foundation::Streams {

    /**
     */
    template <typename ELEMENT_TYPE>
    class LoggingInputOutputStream : public InputOutputStream<ELEMENT_TYPE> {
    protected:
        /**
         *  'LoggingInputOutputStream' is a quasi-namespace:  use Ptr or New () members.
         */
        LoggingInputOutputStream ()                                = delete;
        LoggingInputOutputStream (const LoggingInputOutputStream&) = delete;

    public:
        using ElementType = typename Stream<ELEMENT_TYPE>::ElementType;

    public:
        /**
         */
        using typename InputOutputStream<ELEMENT_TYPE>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *      \endcode
         */
        static Ptr New (const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);
        static Ptr New (Execution::InternallySynchronized internallySynchronized, const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);

    private:
        class Rep_;

    private:
        using InternalSyncRep_ = InternallySynchronizedInputOutputStream<ELEMENT_TYPE, Streams::LoggingInputOutputStream<ELEMENT_TYPE>, typename LoggingInputOutputStream<ELEMENT_TYPE>::Rep_>;
    };

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "LoggingInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_LoggingInputOutputStream_h_*/
