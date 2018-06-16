/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Streams_LoggingInputOutputStream_h_
#define _Stroika_Foundation_Streams_LoggingInputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "InputOutputStream.h"
#include "InternallySyncrhonizedInputOutputStream.h"

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

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
                using Ptr = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *      \endcode
                 */
                static Ptr New (const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput);

            private:
                class Rep_;

            private:
                struct Ptr_ : InputOutputStream<ELEMENT_TYPE>::Ptr {
                    template <typename ARG>
                    Ptr_ (ARG arg)
                        : Ptr (arg)
                    {
                    }
                };

            private:
                using InternalSyncRep_ = InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, Streams::LoggingInputOutputStream, typename LoggingInputOutputStream<ELEMENT_TYPE>::Rep_>;
            };
        }
    }
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "LoggingInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_LoggingInputOutputStream_h_*/
