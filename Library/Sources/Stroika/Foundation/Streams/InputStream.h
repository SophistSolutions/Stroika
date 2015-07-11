/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_h_
#define _Stroika_Foundation_Streams_InputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"

#include    "Stream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Sterl requested the 'optional SeekOffsetType' param overload to Read methods(). THis is logical,
 *              but I'm not sure useful. Discuss with him the use cases. We should lose this as its not
 *              actually implemented and we'd need to likewise extend the Write() API to make this consistent
 *              in all likelihood.
 *
 *              But hard to say, as I'm not sure I understood the motivations. If there are motivations, it doesn't complicate
 *              the API and implementation much.
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Consider making GetOffsetToEndOfStream () a virtual part of rep so it can work with the locks
 *              and be safely atomic (atomic stuff only an lssue for 'Synchonized' stream and could probably just
 *              make both calls inside ITS synchonized lock?).
 *
 *      @todo   Consider making LineEnd format (LF,CR,CRLF, or Auto) an optional param to ReadLine().
 *              Then it would ONLY require Seekable() for CRLF or Auto.
 */



namespace Stroika { namespace Foundation { namespace Characters { class Character; } } }
namespace Stroika { namespace Foundation { namespace Characters { class String; } } }
namespace Stroika { namespace Foundation { namespace Memory { class BLOB; } } }



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  InputStream<> is Smart pointer (with abstract Rep) class defining the interface to reading from
             *          a Stream source of data.
             *
             * Design Overview:
             *
             *      o   @See Stream
             *
             *      o   All read's on an InputStream are BLOCKING. If there is a desire to have a
             *          non-blocking read, then create a new mixin interface and through that interface
             *          you can do non-blocking reads, but this Read() method must always block.
             *
             *      o   EOF is handled by a return value of zero. Once EOF is returned - any subsequent
             *          calls to Read () will return EOF (unless some other mechanism is used to tweak
             *          the state of the object, like a mixed in Seekable class and calling SEEK).
             *
             *              Seek called on an input stream that has already returned EOF, may cause
             *              subsequent Read() calls to NOT be at EOF.
             *
             *      o   Exceptions indicate something went wrong - like an IO error, or possibly in some cases
             *          a formatting effort (e.g. if the source is encrypted, and the stream is decrypting,
             *          then it might detect a format error and throw).
             *
             *      o   InputStream and OutputStream may be logically be mixed togehter to make an
             *          input/output stream: @see InputOutputStream<>
             *
             *      @see Stroika::Foundation::Streams::iostream for adapters to work with std::iostream.
             *
             *      @see MemoryStream for most common stream applications.
             *
             *      @see ExternallyOwnedMemoryInputStream for a more efficient, but slightly less safe
             *          mapping to streams.
             *
             *      o   ReadString/Seekable/PutBack Design Choices:
             *          o   Some common read methods with TextStreams (parsing) - involve some amount of lookahead.
             *              Lookahead COULD be implemented a number of ways:
             *              o   Seek backwards after fetching
             *              o   Special 'put back' variable/API - which allows you to put back either one,
             *                  or a number of characters back into the input Q
             *              o   A special proxy object which stores the extra data, and maintains the context
             *                  of the state of pre-reading.
             *
             *          Each of these approaches has some advantages and disadvantages. Just using Seek() is
             *          the simplest approach. IF all your streams support seeking, there is no reason for another
             *          mechanism. But we dont want to alwys require seeking.
             *
             *          PutBack () is like Seek, but then the question is - do we support just a PutBack of
             *          one character? So only lookahead of one character? That deals with many cases, but not all.
             *          And how does it interact with Seek - if the stream happens to be seekable? And is the
             *          PutBack buffer stored in the letter or envelope class? If in Letter, thats extra work
             *          in every rep (barrier to providing your own stream subtypes). If in the envelope, it doesn't
             *          work intuitively if two variables have separate smart pointers to the same underlying stream.
             *          Reads and writes affect each other EXCPET for the putback buffer!
             *
             *          A special Proxy object is a good choice - but requires the caller todo a lot of extra
             *          work. To some extent that can be automated by somewhat automatically maanging
             *          the proxy storage object in the smartpointer, but thats alot of work and concept
             *          for very little gain.
             *
             *          In the end - at least for now - I've decided on KISS - ReadLine() simply requires it
             *          is Seekable. And there are plenty of wrapper stream classes you can use with any stream
             *          to make them Seekable.
             */
            template    <typename   ELEMENT_TYPE>
            class   InputStream : public Stream<ELEMENT_TYPE> {
            private:
                using   inherited = Stream<ELEMENT_TYPE>;

            protected:
                class   _IRep;

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                InputStream () = default;
                InputStream (nullptr_t);

            protected:
                /**
                 * _SharedIRep is an underlying InputStream object implementation.
                 */
                explicit InputStream (const _SharedIRep& rep);

            public:
                /**
                 *      @todo - IF THIS MUST BE PUBLIC, then rename to not have _GetRep....
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *     Create a synchonized (thread safe) copy of this stream. Note - this still refers to the same
                 *  underlying stream.
                 */
                nonvirtual  InputStream<ELEMENT_TYPE>   Synchronized () const;

            public:
                /**
                 * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
                 */
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                /**
                 * GetOffsetToEndOfStream () returns the the distance from the current offset position to the end of the stream.
                 *  This is equivalent to:
                 *      size_t  size =  Seek (Whence::eFromEnd, 0);
                 *      Seek (Whence::eFromStart, savedReadFrom);
                 *(EXCPET MAYBE GUARNATEED ATOMIC????)
                 */
                nonvirtual  SeekOffsetType  GetOffsetToEndOfStream () const;

            public:
                /**
                 * The new position, measured in bytes, is obtained by adding offset bytes to the position
                 *  specified by whence.
                 *
                 *  Seek () past the end of stream is NOT legal (may reconsider).
                 *
                 *  Seek () returns the new resulting position (measured from the start of the stream - same as GetOffset).
                 */
                nonvirtual  SeekOffsetType  Seek (SignedSeekOffsetType offset) const;
                nonvirtual  SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) const;

            public:
                /**
                 *  Read/0
                 *      return IsMissing() on EOF.
                 *
                 *  Read/2
                 *      Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *      bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *      BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *      without prejudice about how much more is available.
                 *
                 *  Read/3
                 *      TRIAL BALLOON IDEA FROM STERL - NYI.
                 *
                 *      \req IsSeekable()
                 *
                 *      Would be LIKE Read/2 () - except NOT update any intrinsic seek offset. Input offset
                 *      would be where we read from, and would be updated to reflect where we read to. Change in
                 *      offset would be the same as the returned value.
                 */
                nonvirtual  Memory::Optional<ElementType>  Read () const;
                nonvirtual  size_t  Read (ElementType* intoStart, ElementType* intoEnd) const;
                nonvirtual  size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) const;

            public:
                /**
                 *  @todo Consider if we should lose this. Optional appraoch maybe better.
                 *
                 *  Blocking read of a single character. Returns a NUL-character on EOF ('\0')
                 */
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  Characters::Character   ReadCharacter () const;

            public:
                /**
                 * Readline looks for a trailing bare CR, or bare LF, or CRLF. It returns whatever line-terminator
                 * it encounters as part of the read line.
                 *
                 *  ReadLine() will return an empty string iff EOF.
                 *
                 *      \req IsSeekable ()
                 */
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  Characters::String ReadLine () const;

            public:
                /**
                 *  Returns Iterable<String> object, so you can
                 *  write code:
                 *          for (String line : stream.ReadLines ()) {
                 *          }
                 *
                 *  Like ReadLine(), the returned lines include trailing newlines/etc.
                 *
                 *      \req IsSeekable ()
                 */
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  Traversal::Iterable<Characters::String> ReadLines () const;

            public:
                /**
                 *  Read from the current seek position, until EOF, and accumulate all of it into a String.
                 *  Note - since the stream may not start at the beginning, this isn't necessarily ALL
                 *  that was in the stream -just all that remains.
                 *
                 *  Read from the current stream position until EOF, and accumulate all of it into a BLOB.
                 */
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  Characters::String ReadAll () const;
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Memory::Byte>::value>::type>
                nonvirtual  Memory::BLOB ReadAll () const;
            };


            template    <>
            template    <>
            Characters::Character   InputStream<Characters::Character>::ReadCharacter () const;
            template    <>
            template    <>
            Characters::String InputStream<Characters::Character>::ReadLine () const;
            template    <>
            template    <>
            Traversal::Iterable<Characters::String> InputStream<Characters::Character>::ReadLines () const;
            template    <>
            template    <>
            Characters::String InputStream<Characters::Character>::ReadAll () const;
            template    <>
            template    <>
            Memory::BLOB InputStream<Memory::Byte>::ReadAll () const;


            /**
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   InputStream<ELEMENT_TYPE>::_IRep : public Stream<ELEMENT_TYPE>::_IRep  {
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
                virtual SeekOffsetType      GetReadOffset () const                                     =   0;

            public:
                virtual SeekOffsetType      SeekRead (Whence whence, SignedSeekOffsetType offset)      =   0;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 *
                 *  If offset is nullptr, then read uses builtin seek offset, and updates it (normal case).
                 *
                 *  If offset is non-null, treat as seek to that offset, and update that paraemter offset, but not real underlying read.
                 *  this requires seekability (assert otherwise).
                 */
                virtual size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd)          =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InputStream.inl"

#endif  /*_Stroika_Foundation_Streams_InputStream_h_*/
