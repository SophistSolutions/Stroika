/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SplitterOutputStream_h_
#define _Stroika_Foundation_Streams_SplitterOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "InternallySyncrhonizedOutputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  A SplitterOutputStream wraps 2 output Streams,and duplicates all writes across the two.
             *
             *  This can can be used easily to produce logging for the communications along a stream.
             *
             *      \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
             */
            template <typename ELEMENT_TYPE>
            class SplitterOutputStream : public OutputStream<ELEMENT_TYPE> {
            public:
                SplitterOutputStream ()                            = delete;
                SplitterOutputStream (const SplitterOutputStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *      \endcode
                 */
                static Ptr New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2);

            private:
                class Rep_;

            private:
                using InternalSyncRep_ = InternallySyncrhonizedOutputStream<ELEMENT_TYPE, Streams::SplitterOutputStream, typename SplitterOutputStream<ELEMENT_TYPE>::Rep_>;
            };

            /**
             *  Ptr is a copyable smart pointer to a SplitterOutputStream.
             */
            template <typename ELEMENT_TYPE>
            class SplitterOutputStream<ELEMENT_TYPE>::Ptr : public OutputStream<ELEMENT_TYPE>::Ptr {
                using inherited = typename OutputStream<ELEMENT_TYPE>::Ptr;

            protected:
                using _SharedIRep = typename SplitterOutputStream<ELEMENT_TYPE>::_SharedIRep;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *      \endcode
                 */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;
                Ptr (Ptr&& from)      = default;

            protected:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator= (Ptr&& rhs) = default;

            protected:
                /**
                 *  \brief protected access to underlying stream smart pointer
                 */
                nonvirtual _SharedIRep _GetSharedRep () const;

            private:
                friend class SplitterOutputStream;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SplitterOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_h_*/
