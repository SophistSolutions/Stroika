/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_h_
#define _Stroika_Foundation_Streams_BufferedInputStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

#include "InputStream.h"
#include "InternallySyncrhonizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   BufferedInputStream::IRep_ IMPLEMENTATION NEEDED. It does no buffering!
 *
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't support seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *
 *      @todo   Add tuning parameters, such as buffer size
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  @brief  BufferedInputStream is an InputStream<ELEMENT_TYPE>::Ptr which provides buffered access.
             *          This is useful if calls to the underling stream source can be expensive. This class
             *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
             */
            template <typename ELEMENT_TYPE>
            class BufferedInputStream : public InputStream<ELEMENT_TYPE> {
            public:
                BufferedInputStream ()                           = delete;
                BufferedInputStream (const BufferedInputStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *          InputStream<Byte>::Ptr in = BufferedInputStream<Byte>::New (fromStream);
                 *      \endcode
                 *
                 *  \par Example Usage
                 *      \code
                 *          CallExpectingBinaryInputStreamPtr (BufferedInputStream<Byte>::New (fromStream))
                 *      \endcode
                 */
                static Ptr New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn);
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const typename InputStream<ELEMENT_TYPE>::Ptr& realIn);

            private:
                class Rep_;

            private:
                using InternalSyncRep_ = InternallySyncrhonizedInputStream<ELEMENT_TYPE, Streams::BufferedInputStream, typename BufferedInputStream<ELEMENT_TYPE>::Rep_>;
            };

            /**
             *  Ptr is a copyable smart pointer to a BufferedInputStream.
             */
            template <typename ELEMENT_TYPE>
            class BufferedInputStream<ELEMENT_TYPE>::Ptr : public InputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
                *  \par Example Usage
                *      \code
                *          InputStream<Byte>::Ptr in = BufferedInputStream<Byte>::New (fromStream);
                *      \endcode
                *
                *  \par Example Usage
                *      \code
                *          CallExpectingBinaryInputStreamPtr (BufferedInputStream<Byte>::New (fromStream))
                *      \endcode
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;

            protected:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;

            private:
                friend class BufferedInputStream;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_h_*/
