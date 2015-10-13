/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_h_
#define _Stroika_Foundation_Streams_Stream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"
#include    "../Memory/Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Consdier the design of 'force always blocking' for Streams API. Is that too harsh?
 *              Maybe have some non-blocking API variants?
 *
 *      @todo   Considered doing something like .Net CanSeek (), CanRead(), etc, but I decided it
 *              was simpler and more elegant to just keep things separate and use mixin classes
 *              like 'interfaces' - and use dynamic_cast<> to see what functionality is available
 *              on a stream. At least thats so for the implementations. The wrapper class - Seekable -
 *              does have an IsSeekable() method which just tests if we have a Seekable interface.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  When seeking, you can see an offset from the start (typical/start) or from the end of the
             *  stream, or from the current position)
             */
            enum    class   Whence : uint8_t {
                eFromStart,
                eFromCurrent,
                eFromEnd,

                Stroika_Define_Enum_Bounds(eFromStart, eFromEnd)
            };


            /**
             *  SeekOffsetType is unsigned, normally, because for most purposes its zero based.
             *  @see SignedSeekOffsetType.
             *
             *  \note   We use a 64-bit integer here, because streaming >4GB files is common, even in devices so tiny as a cellphone.
             *          and no larger because its inconcievable to store that much data anytime soon.
             */
            using       SeekOffsetType          =   uint64_t;


            /**
             *  SignedSeekOffsetType is signed variant of SeekOffsetType - used to specify a seek
             *  offset which can sometimes negative (backwards).
             *
             *  @see SignedSeekOffsetType.
             */
            using       SignedSeekOffsetType    =   int64_t;


            /**
              * Design Overview:
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
              *     o   Seek Offsets are in elements of the kind of stream (e.g in Bytes for a Stream<Byte>, and
              *         in Characters for a Stream<Character>).
              *
              *     o   Two important subclasses of Stream<> are InputStream<> (for reading) and OutputStream<> for
              *         writing. So that each can maintain its own intrinsic current offset (separate seek offset
              *         for reading and writing) in mixed (input/output) streams, the actual offset APIs and
              *         logic are in those subclasses.
              *
              *     o   Stream APIs are intrinsically blocking. This makes working with them much simpler, since code
              *         using Streams doesnt need to be written to handle both blocking and non-blocking behavior, and be
              *         surprised when a 'mode' is set on a stream making it non-blocking.
              *
              *     o   Relationship with std iostream:
              *             Stroika streams are in many ways similar to iostreams. They are interoperable with iostreams
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
              *             o   Stroika supports non-seekable streams (needed for things like sockets, and certain specail files, like
              *                 Linux procfs files).
              *
              *             o   Due to more orthoganal API, easier to provide intuitive simple adapters mapping one kind of stream
              *                 to another (such as binary streams to streams of text, like the .net TextReader).
              *
              *             o   (@todo - fill in more differences if there are any more?)
              */
            template    <typename   ELEMENT_TYPE>
            class   Stream {
            protected:
            public:
                class   _IRep;

            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                Stream () = default;
                Stream (nullptr_t);

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit Stream (const _SharedIRep& rep);

            public:
                /**
                 *  empty() doesn't check the data in the stream, but instead checks if this Stream
                 *  smart pointer references any actual stream.
                 *
                 *  @see clear()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  clear () doesn't clear the data in the stream, or close the stream, but unreferences the Stream
                 *  smart pointer. Only if this Stream smartpointer is the last reference to the underlying stream
                 *  data does this clear () close the underlying stream.
                 *
                 *  @see empty()
                 */
                nonvirtual  void    clear ();

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 * \brief   Returns true iff this object was constructed with a seekable input stream rep.
                 *
                 *  Returns true iff this object was constructed with a seekable input stream rep. Note -
                 *  seekability cannot change over the lifetime of an object.
                 */
                nonvirtual  bool    IsSeekable () const;

            private:
                _SharedIRep fRep_;

            private:
                bool        fSeekable_;
            };


            /**
             *
             */
            template    <typename ELEMENT_TYPE>
            class   Stream<ELEMENT_TYPE>::_IRep {
            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                _IRep () = default;
                _IRep (const _IRep&) = delete;

            public:
                virtual ~_IRep () = default;

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;

            public:
                /**
                 *  cannot change value - called once and value cached
                 */
                virtual bool    IsSeekable () const = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Stream.inl"

#endif  /*_Stroika_Foundation_Streams_Stream_h_*/
