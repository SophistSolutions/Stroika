/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_ 1

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
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             */
            template <typename ELEMENT_TYPE>
            class InternallySyncrhonizedInputStream : public InputStream<ELEMENT_TYPE> {
            public:
                /**
                 *  'InternallySyncrhonizedInputStream' is a quasi-namespace: use Ptr or New () members.
                 */
                InternallySyncrhonizedInputStream ()                                         = delete;
                InternallySyncrhonizedInputStream (const InternallySyncrhonizedInputStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *          Streams::InputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedInputStream<Byte>::New (otherInputStreamToBeSharedAcrossThread);
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonized">C++-Standard-Thread-Safety-Letter-Internally-Synchonized/a>
                 */
                static Ptr New (const InputStream<T>::Ptr& stream2Wrap);

            private:
                class Rep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a InternallySyncrhonizedInputStream.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonize">C++-Standard-Thread-Safety-Letter-Internally-Synchonize/a>
             */
            template <typename ELEMENT_TYPE>
            class InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Ptr : public InputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *          Streams::InputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedInputStream<Byte>::New (otherInputStreamToBeSharedAcrossThread);
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonized">C++-Standard-Thread-Safety-Letter-Internally-Synchonized/a>
                 */
                Ptr ()               = delete;
                Ptr (const Ptr& src) = default;
                Ptr (Ptr&& src)      = default;

            private:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator                   = (const InternallySyncrhonizedInputStream& rhs);

            private:
                friend class InternallySyncrhonizedInputStream;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySyncrhonizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_*/
