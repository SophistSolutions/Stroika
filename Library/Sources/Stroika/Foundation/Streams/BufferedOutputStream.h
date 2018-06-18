/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Configuration/Common.h"

#include "InternallySyncrhonizedOutputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   Properly handle seekability based on underlying stream seekability, and/or param/flag on construction (we can always make it seekable by buffering).
 *
 *              BufferedOutputStream NOW must properly support SEEKABLE! if arg
 *              is seekable, we must override seek methods, and forward them, and adjust buffer as appropriate.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probbaly be made more efficent in sycn form - using direct mutex
 */

namespace Stroika::Foundation {
    namespace Streams {

        /**
         *  A BufferedOutputStream wraps an argument stream
         *  (which must have lifetime > this BufferedOutputStream) and will buffer up writes to it.
         *
         *      \note   If you fail to Flush() this object before it is destroyed, exceptions in flushing
         *              the data may be suppressed.
         *              Best to call @Close ()
         *
         *      \note   BufferedOutputStream aggregates its owned substream, so that a Close () on BufferedOutputStream
         *              will Close that substream.
         *
         *      \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
         */
        template <typename ELEMENT_TYPE>
        class BufferedOutputStream : public OutputStream<ELEMENT_TYPE> {
        public:
            BufferedOutputStream ()                            = delete;
            BufferedOutputStream (const BufferedOutputStream&) = delete;

        public:
            class Ptr;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          OutputStream<Byte>::Ptr out = BufferedOutputStream<Memory::Byte>::New (FileOutputStream (fileName, flushFlag));
             *      \endcode
             */
            static Ptr New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut);
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut);

        private:
            class Rep_;

        private:
            using InternalSyncRep_ = InternallySyncrhonizedOutputStream<ELEMENT_TYPE, Streams::BufferedOutputStream<ELEMENT_TYPE>, typename BufferedOutputStream<ELEMENT_TYPE>::Rep_>;
        };

        /**
         *  Ptr is a copyable smart pointer to a BufferedOutputStream.
         */
        template <typename ELEMENT_TYPE>
        class BufferedOutputStream<ELEMENT_TYPE>::Ptr : public OutputStream<ELEMENT_TYPE>::Ptr {
            using inherited = typename OutputStream<ELEMENT_TYPE>::Ptr;

        protected:
            using _SharedIRep = typename BufferedOutputStream<ELEMENT_TYPE>::_SharedIRep;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          BufferedOutputStream<Byte>::Ptr out = BufferedOutputStream<Memory::Byte>::New (FileOutputStream (fileName, flushFlag));
             *          out.SetBufferSize (1000);
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

        public:
            nonvirtual size_t GetBufferSize () const;

        public:
            nonvirtual void SetBufferSize (size_t bufSize);

        public:
            /**
             *  Throws away all data about to be written (buffered). Once this is called,
             *  the effect of future Flush () calls is undefined. This can be used when the stream
             *  wraps an underlying object like a socket, and you dont want to waste effort
             *  talking to it, but its harmless todo so.
             *
             *  One a stream is aborted, its undefined what other operations will do (they wont crash
             *  but they may or may not write).
             */
            nonvirtual void Abort ();

        protected:
            /**
             *  \brief protected access to underlying stream smart pointer
             */
            nonvirtual _SharedIRep _GetSharedRep () const;

        private:
            friend class BufferedOutputStream;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_h_*/
