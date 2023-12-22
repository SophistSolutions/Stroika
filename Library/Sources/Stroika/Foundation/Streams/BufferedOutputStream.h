/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Configuration/Common.h"

#include "InternallySynchronizedOutputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Properly handle seekability based on underlying stream seekability, and/or param/flag on construction (we can always make it seekable by buffering).
 *
 *              BufferedOutputStream NOW must properly support SEEKABLE! if arg
 *              is seekable, we must override seek methods, and forward them, and adjust buffer as appropriate.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probbaly be made more efficent in sycn form - using direct mutex
 */

namespace Stroika::Foundation::Streams::BufferedOutputStream {

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
     *      \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    class Ptr;

    /**
         *  \par Example Usage
         *      \code
         *          OutputStream<byte>::Ptr out = BufferedOutputStream::New<byte> (FileOutputStream (fileName, flushFlag));
         *      \endcode
         */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut);

    template <typename ELEMENT_TYPE>
    class Rep_;

#if 0
    template <typename ELEMENT_TYPE>
        using InternalSyncRep_ =
            InternallySynchronizedOutputStream<ELEMENT_TYPE, Streams::BufferedOutputStream<ELEMENT_TYPE>, typename BufferedOutputStream<ELEMENT_TYPE>::Rep_>;
#endif

    /**
     *  Ptr is a copyable smart pointer to a BufferedOutputStream.
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public OutputStream<ELEMENT_TYPE>::Ptr {
        using inherited = typename OutputStream<ELEMENT_TYPE>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          BufferedOutputStream::Ptr<byte> out = BufferedOutputStream::New<byte> (FileOutputStream (fileName, flushFlag));
         *          out.SetBufferSize (1000);
         *      \endcode
         */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;
        Ptr (Ptr&& from)      = default;
        Ptr (const shared_ptr<Rep_<ELEMENT_TYPE>>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;
        nonvirtual Ptr& operator= (Ptr&& rhs)      = default;

    public:
        nonvirtual size_t GetBufferSize () const;

    public:
        nonvirtual void SetBufferSize (size_t bufSize);

    public:
        /**
         *  Throws away all data about to be written (buffered). Once this is called,
         *  the effect of future Flush () calls is undefined. This can be used when the stream
         *  wraps an underlying object like a socket, and you don't want to waste effort
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
        nonvirtual shared_ptr<Rep_<ELEMENT_TYPE>> _GetSharedRep () const;

        //private:
        // friend class BufferedOutputStream;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_h_*/
