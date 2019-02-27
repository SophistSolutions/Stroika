/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_h_
#define _Stroika_Foundation_Streams_Stream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Memory/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo https://stroika.atlassian.net/browse/STK-605 - Possibly add IsInternallySynchronized () method to Stream<>::Ptr and _IRep
 */

namespace Stroika::Foundation::Streams {

    /**
     *  When seeking, you can seek (offset) from the start (typical/start) or from the end of the
     *  stream, or from the current position)
     */
    enum class Whence : uint8_t {
        eFromStart,
        eFromCurrent,
        eFromEnd,

        Stroika_Define_Enum_Bounds (eFromStart, eFromEnd)
    };

    /**
     *  SeekOffsetType is unsigned, normally, because for most purposes its zero based.
     *  @see SignedSeekOffsetType.
     *
     *  \note   We use a 64-bit integer here, because streaming >4GB files is common, even in devices so tiny as a cellphone.
     *          and no larger because its inconcievable to store that much data anytime soon.
     */
    using SeekOffsetType = uint64_t;

    /**
     *  SignedSeekOffsetType is signed variant of SeekOffsetType - used to specify a seek
     *  offset which can sometimes negative (backwards).
     *
     *  @see SignedSeekOffsetType.
     */
    using SignedSeekOffsetType = int64_t;

    /**
     *  Used in some subclasses as a flag to indicate stream seekability.
     */
    enum class SeekableFlag {
        eSeekable,
        eNotSeekable,

        Stroika_Define_Enum_Bounds (eSeekable, eNotSeekable)
    };

    /**
     *  \em Design Overview
     *     o   A Streams is a sequence of data elements made available over time. These elements
     *         are typically 'Bytes' - or 'Characters' - but can be any copyable type.
     *
     *     o   Streams are created, and then handled ONLY through smart pointers. Assigning Streams
     *         merely copies the 'smart pointer' to that same underlying stream. This means that offsets
     *         and underlying data, are all shared.
     *
     *     o   Streams have two parallel hierarchies, which mirror one another, of smart pointers and related
     *         'virtual rep' objects which provide the API which implementers override.
     *
     *     o   Seek Offsets are in elements of the kind of stream (e.g in Bytes for a Stream<byte>, and
     *         in Characters for a Stream<Character>).
     *
     *     o   Two important subclasses of Stream<> are InputStreamPtr<> (for reading) and OutputStream<> for
     *         writing. So that each can maintain its own intrinsic current offset (separate seek offset
     *         for reading and writing) in mixed (input/output) streams, the actual offset APIs and
     *         logic are in those subclasses.
     *
     *     o   Stream APIs are intrinsically blocking (except see InputStreamPtr<> and InputStreamPtr<>::ReadNonBlocking()).
     *         This makes working with them much simpler, since code using Streams doesn't need to be written to handle
     *         both blocking and non-blocking behavior, and be surprised when a 'mode' is set on a stream making it non-blocking.
     *
     *     o   Relationship with std iostream:
     *             Stroika streams are in some ways similar to iostreams. They are interoperable with iostreams
     *             through the Streams::iostream::(InputStreamFromStdIStream, InputStreamToStdIStream,
     *             OutputStreamFromStdIStream, OutputStreamToStdIStream) classes.
     *
     *             o   Stroika Streams are much easier to create (just a few intuitive virtual methods
     *                 to override.
     *
     *             o   Stroika streams are unrelated to formatting of text (@todo what other mechanism do we offer?)
     *
     *             o   Stroika Streams are much easier to use and understand, with better internal error checking,
     *                 (like thread safety), and simpler, more consistent naming for offets/seeking, and seekability.
     *
     *             o   Stroika supports non-seekable streams (needed for things like sockets, and certain special files, like
     *                 Linux procfs files).
     *
     *             o   Due to more orthoganal API, easier to provide intuitive simple adapters mapping one kind of stream
     *                 to another (such as binary streams to streams of text, like the .net TextReader).
     *
     *             o   Stroika Streams are divided into 'Smart Pointer' objects (all you interact with) and the underlying Stream data (Rep).
     *                 Copying an iostream is generally not possible with STL, but with Stroika, it copies a reference (smart pointer) to the underlying
     *                 stream.
     */
    template <typename ELEMENT_TYPE>
    class Stream {
    public:
        /**
         *  'Stream' is a quasi-namespace.
         */
        Stream ()              = delete;
        Stream (const Stream&) = delete;

    public:
        using ElementType = ELEMENT_TYPE;

    public:
        class Ptr;

    public:
        class _IRep;

    protected:
        using _SharedIRep = shared_ptr<_IRep>;
    };

    /**
     *  \brief A Stream<ELEMENT_TYPE>::Ptr is a smart-pointer to a stream of elements of type T.
     *
     *  Stream<T>::Ptr is seldom used directly. Much more commonly, you will want InputStream<T>::Ptr, or OutputStream<T>::Ptr.
     *
     *  \note Since Stream<ELEMENT_TYPE>::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
     *        the underlying thread object.
     *
     *  @see Stream<ELEMENT_TYPE>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class Stream<ELEMENT_TYPE>::Ptr : protected Debug::AssertExternallySynchronizedLock {
    public:
        /**
         *  defaults to nullptr
         */
        Ptr ()           = default;
        Ptr (const Ptr&) = default;
        Ptr (Ptr&&)      = default;
        Ptr (nullptr_t);

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr&) = default;
        nonvirtual Ptr& operator= (Ptr&&) = default;

    protected:
        /**
         * _SharedIRep rep is the underlying shared output Stream object.
         *
         *  \req rep != nullptr (use other constructor)
         */
        explicit Ptr (const _SharedIRep& rep);

    public:
        /**
         *  reset () doesn't clear the data in the stream, or close the stream, but unreferences the Stream
         *  smart pointer. Only if this Stream smartpointer is the last reference to the underlying stream
         *  data does this reset () close the underlying stream.
         */
        nonvirtual void reset () noexcept;

    public:
        /**
         *  \brief return true iff stream ptr is nullptr
         *
         *  @see reset()
         */
        nonvirtual bool operator== (nullptr_t) const;

    public:
        /**
         *  \brief return true iff stream ptr is not nullptr
         *
         *  @see reset()
         */
        nonvirtual bool operator!= (nullptr_t) const;

    public:
        /**
         *  \brief return true iff *this != nullptr
         */
        nonvirtual explicit operator bool () const;

    protected:
        /**
         *  \brief protected access to underlying stream smart pointer
         */
        nonvirtual _SharedIRep _GetSharedRep () const;

    protected:
        /**
         * \req *this != nullptr
         */
        nonvirtual const _IRep& _GetRepConstRef () const;

    protected:
        /**
         * \req *this != nullptr
         */
        nonvirtual _IRep& _GetRepRWRef () const;

    public:
        /**
         * \brief   Returns true iff this object was constructed with a seekable input stream rep.
         *
         *  Returns true iff this object was constructed with a seekable input stream rep. Note -
         *  seekability cannot change over the lifetime of an object.
         *
         *  @see GetSeekability
         */
        nonvirtual bool IsSeekable () const;

    public:
        /**
         * \brief   Returns SeekableFlag of the input stream rep.
         *
         *  Note - seekability cannot change over the lifetime of an object.
         *
         *  @see IsSeekable
         */
        nonvirtual SeekableFlag GetSeekability () const;

    private:
        _SharedIRep fRep_;

    private:
        bool fSeekable_{false};
    };

    template <typename ELEMENT_TYPE>
    bool operator== (nullptr_t, const typename Stream<ELEMENT_TYPE>::Ptr& s);
    template <typename ELEMENT_TYPE>
    bool operator!= (nullptr_t, const typename Stream<ELEMENT_TYPE>::Ptr& s);

    /**
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype/a>
     */
    template <typename ELEMENT_TYPE>
    class Stream<ELEMENT_TYPE>::_IRep {
    public:
        using ElementType = ELEMENT_TYPE;

    public:
        _IRep ()             = default;
        _IRep (const _IRep&) = delete;

    public:
        virtual ~_IRep () = default;

    public:
        nonvirtual _IRep& operator= (const _IRep&) = delete;

    public:
        /**
         *  cannot change value - called once and value cached
         */
        virtual bool IsSeekable () const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Stream.inl"

#endif /*_Stroika_Foundation_Streams_Stream_h_*/
