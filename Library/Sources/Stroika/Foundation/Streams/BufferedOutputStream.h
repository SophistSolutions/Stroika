/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Configuration/Common.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   Think out Seekable. Probably MUST mixin Seekable, and properly handle
 *              (vai throw OperaitonNotSupported) if one constructs a BufferedOutputStream
 *              and tries to SEEK wihout the undelrying stream being seekable.
 *
 *      @todo   BufferedOutputStream NOW must properly support SEEKABLE! if arg
 *              is seekable, we must override seek methods, and forward them, and adjust buffer as appropriate.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  A BufferedOutputStream wraps an argument stream
             *  (which must have lifetime > this BufferedOutputStream) and will buffer up writes to it.
             *
             *      \note   If you fail to Flush() this object before it is destroyed, exceptions in flushing
             *              the data may be suppressed.
             *
             *      \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
             */
            template <typename ELEMENT_TYPE>
            class BufferedOutputStream : public OutputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename OutputStream<ELEMENT_TYPE>::Ptr;

            private:
                class Rep_;

            protected:
                using _SharedIRep = shared_ptr<Rep_>;

            public:
                /**
                 */
                BufferedOutputStream () = delete;
                BufferedOutputStream (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut);
                BufferedOutputStream (BufferedOutputStream&&)      = default;
                BufferedOutputStream (const BufferedOutputStream&) = delete;

            protected:
                BufferedOutputStream (const _SharedIRep& rep);

            public:
                nonvirtual BufferedOutputStream& operator= (const BufferedOutputStream&) = delete;

            public:
                class Ptr;

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
            };

            /**
             *  Ptr is a copyable smart pointer to a MemoryStream.
             */
            template <typename ELEMENT_TYPE>
            class BufferedOutputStream<ELEMENT_TYPE>::Ptr : public BufferedOutputStream<ELEMENT_TYPE> {
            public:
                /**
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;
                Ptr (Ptr&& from)      = default;
                Ptr (const BufferedOutputStream& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator= (Ptr&& rhs) = default;
                nonvirtual Ptr& operator              = (const BufferedOutputStream& rhs);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_h_*/
