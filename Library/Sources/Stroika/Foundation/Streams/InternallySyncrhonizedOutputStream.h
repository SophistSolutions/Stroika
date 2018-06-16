/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS = OutputStream, typename BASE_REP_TYPE = typename BASE_CLASS<ELEMENT_TYPE>::_IRep>
            class InternallySyncrhonizedOutputStream : public OutputStream<ELEMENT_TYPE> {
            public:
                /**
                 *  'InternallySyncrhonizedOutputStream' is a quasi-namespace: use Ptr or New () members.
                 */
                InternallySyncrhonizedOutputStream ()                                          = delete;
                InternallySyncrhonizedOutputStream (const InternallySyncrhonizedOutputStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *          Streams::OutputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedOutputStream<Byte>::New (otherOutputStreamToBeSharedAcrossThread);
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonized">C++-Standard-Thread-Safety-Letter-Internally-Synchonized</a>
                 */
                template <typename... ARGS>
                static Ptr New (ARGS&&... args);

            private:
                class Rep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a InternallySyncrhonizedOutputStream.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonize">C++-Standard-Thread-Safety-Letter-Internally-Synchonize</a>
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            class InternallySyncrhonizedOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr : public BASE_CLASS<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename BASE_CLASS<ELEMENT_TYPE>::Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *          Streams::OutputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedOutputStream<Byte>::New (otherOutputStreamToBeSharedAcrossThread);
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonized">C++-Standard-Thread-Safety-Letter-Internally-Synchonized</a>
                 */
                Ptr ()               = delete;
                Ptr (const Ptr& src) = default;
                Ptr (Ptr&& src)      = default;

            private:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator                   = (const InternallySyncrhonizedOutputStream& rhs);

            private:
                friend class InternallySyncrhonizedOutputStream;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySyncrhonizedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_*/
