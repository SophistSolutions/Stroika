/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_h_
#define _Stroika_Foundation_Streams_InputStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"
#include "../Memory/Optional.h"
#include "../Traversal/Iterable.h"

#include "Stream.h"

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
 *              and be safely atomic (atomic stuff only an lssue for 'Synchronized' stream and could probably just
 *              make both calls inside ITS Synchronized lock?).
 *
 *      @todo   Consider making LineEnd format (LF,CR,CRLF, or Auto) an optional param to ReadLine().
 *              Then it would ONLY require Seekable() for CRLF or Auto.
 */

namespace Stroika {
    namespace Foundation {
        namespace Characters {
            class Character;
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Characters {
            class String;
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Memory {
            class BLOB;
        }
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief  InputStream<> is Smart pointer (with abstract Rep) class defining the interface to reading from
             *          a Stream source of data.
             *
             * Design Overview:
             *
             *      o   @See Stream
             *
             *      o   Nearly all read's on an InputStream are BLOCKING. If there is a desire to have a
             *          non-blocking read, then call ReadSome (). All operations block except ReadSome ().
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
             *      o   InputStream and OutputStream may be logically be mixed together to make an
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
             *
             *      o   BLOCKING/NON-BLOCKING Design Choices:
             *          Berkeley sockets have the same API, but a flag that turns the socket from blocking to non-blocking.
             *
             *          This pattern is widely copied, and somewhat of a compelling default choice.
             *
             *          But - it sufers from the grave flaw that when you write generic code or routines, you must write it
             *          to handle both blocking and non-blocking cases, and these are really quite differnt.
             *
             *          Mostly - blocking is MUCH simpler to read/write/debug. But you NEED non-blocking sometimes for efficeincy
             *          reasons (to manage progress through a long operation).
             *
             *          Stroika's approach to this is to essentially everywhere in the InputStream API, have the methods be
             *          blocking (including seek, when supported). But when you must do non-blocking IO, you can call ReadSome ()
             *          which mostly tells you if there is any data available to read (and variants will also read it for you).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Must-Externally-Synchronize-Letter-Thread-Safety">Must-Externally-Synchronize-Letter-Thread-Safety</a>
             */
            template <typename ELEMENT_TYPE>
            class InputStream : public Stream<ELEMENT_TYPE> {
            private:
                using inherited = Stream<ELEMENT_TYPE>;

            protected:
                class _IRep;

            protected:
                using _SharedIRep = shared_ptr<_IRep>;

            public:
                using ElementType = ELEMENT_TYPE;

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
                nonvirtual _SharedIRep _GetRep () const;

            public:
                /**
                 *     Create a Synchronized (thread safe) copy of this stream. Note - this still refers to the same
                 *  underlying stream.
                 */
                nonvirtual InputStream<ELEMENT_TYPE> Synchronized () const;

            public:
                /**
                 * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
                 */
                nonvirtual SeekOffsetType GetOffset () const;

            public:
                /**
                 * GetOffsetToEndOfStream () returns the the distance from the current offset position to the end of the stream.
                 *  This is equivalent to:
                 *      size_t  size =  Seek (Whence::eFromEnd, 0);
                 *      Seek (Whence::eFromStart, savedReadFrom);
                 *(EXCPET MAYBE GUARNATEED ATOMIC????)
                 */
                nonvirtual SeekOffsetType GetOffsetToEndOfStream () const;

            public:
                /**
                 * The new position, measured in bytes, is obtained by adding offset bytes to the position
                 *  specified by whence.
                 *
                 *  Seek () past the end of stream is NOT legal (may reconsider).
                 *
                 *  Seek () returns the new resulting position (measured from the start of the stream - same as GetOffset).
                 */
                nonvirtual SeekOffsetType Seek (SignedSeekOffsetType offset) const;
                nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset) const;

            public:
                /**
                 *  Read/0
                 *      return IsMissing() on EOF.
                 *
                 *  Read/2
                 *      Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *      bufSize (intoEnd-intoStart) must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *      BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *      without prejudice about how much more is available.
                 *
                 *  All overloads:
                 *      It is legal to call Read () if its already returned EOF, but then it MUST return EOF again.
                 *
                 *      \req (intoEnd - intoStart) >= 1
                 *
                 *  @see ReadAll () to read all the data in the file at once.
                 *
                 *  @see ReadSome () for non-blocking
                 */
                nonvirtual Memory::Optional<ElementType> Read () const;
                nonvirtual size_t Read (ElementType* intoStart, ElementType* intoEnd) const;

            public:
                /**
                 *  \brief  similar to Read () - except that it is non-blocking, and will return Memory::nullopt if no data available without blocking.
                 *
                 *  \note   https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
                 *
                 *      ReadSome/0:
                 *          Returns the number of elements definitely available to read without blocking. nullopt return value means
                 *          no data available, and 0 return value means @ EOF.
                 *
                 *          This often will return 1 when more data is available, but due to the semantics of Read() allowing a read of
                 *          less than requested, just knowing there is some data is sufficient.
                 *
                 *      ReadSome/2:
                 *          Never blocks. Read up to the amount specified in the arguments (intoEnd-intoStart), and return Missing/nullopt
                 *          if nothing can be read without blocking, or 0 for EOF, or > 0 for actual elements read.
                 *
                 *      Like Read (), it is legal to call ReadSome () if its already returned EOF, but then it MUST return EOF again.
                 *
                 *  \note   Blocking
                 *          When we say non-blocking, that is in general ambiguous and to the extent to which clear, impossible to guarantee
                 *          (consider a breakpoint or  mutex lock someplace - say on memory allocation, halting problem).
                 *
                 *          What we mean by non-blocking is that this doesn't depend on data being available from an outside upstream
                 *          source, and that the code will run with the data it has at its disposal.
                 *
                 *          If you really need a gaurantee, use a separate thread to do the reading and a BlockingQueue to pass the data
                 *          from that thread to the caller.
                 *
                 *  \note   Returns Memory::nullopt (IsMissing) means no data immediately and definitely available. But sometimes its not possible
                 *          to be sure, so this could return nullopt/IsMissing () - even when a blocking read could have read something.
                 *
                 *  \note   We may need to abandon this experimental API because:
                 *              1>  It makes building Reps more complicated
                 *              2>  Its not that useful without guarantees of not blocking and guurantees that when you read and get back
                 *                  nullopt, there is no point in reading
                 *              3>  it is easily workaroundable using a Blocking Queue and another thread to read actual data
                 *              4>  Original mandate for this Streams class module was simplicity of use, extension etc, and this makes it harder.
                 *
                 *      \req (intoEnd - intoStart) >= 1
                 *
                 *  @see Read ()
                 *  @see ReadAll ()
                 */
                nonvirtual Memory::Optional<size_t> ReadSome () const;
                nonvirtual Memory::Optional<size_t> ReadSome (ElementType* intoStart, ElementType* intoEnd) const;

            public:
                /**
                 *  @todo Consider if we should lose this. Optional appraoch maybe better.
                 *
                 *  Blocking read of a single character. Returns a NUL-character on EOF ('\0')
                 */
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual Characters::Character ReadCharacter () const;

            public:
                /**
                 * shorthand for declaring
                 *      POD_TYPE    tmp;
                 *      size_n = ReadAll ((Byte*)&tmp, (Byte*)(&tmp+1));
                 *      if (n==sizeof(tmp)) {  return tmp; } else throw EOFException (...);
                 *
                 *  \note   If not enough bytes are available to return a POD_TYPE, EOFException will be thrown.
                 *  \note   Only defined on Binary Streams (InputStream<Byte>), but POD_TYPE can be any (is_pod) type.
                 */
                template <typename POD_TYPE, typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                nonvirtual POD_TYPE ReadPOD () const;

            public:
                /**
                 * Readline looks for a trailing bare CR, or bare LF, or CRLF. It returns whatever line-terminator
                 * it encounters as part of the read line.
                 *
                 *  ReadLine() will return an empty string iff EOF.
                 *
                 *      \req IsSeekable ()
                 */
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual Characters::String ReadLine () const;

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
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual Traversal::Iterable<Characters::String> ReadLines () const;

            public:
                /**
                 *  ReadAll/0
                 *  ReadAll/1
                 *      Read from the current seek position, until EOF or upTo elements read (whichever comes first),
                 *      and accumulate all of it into a String or BLOB (depending on stream type).
                 *
                 *      Note - since the stream may not start at the beginning, this isn't necessarily ALL
                 *      that was in the stream -just all that remains.
                 *
                 *      Also - since upTo elements may be read before EOF, the stream may or may not be at the
                 *      EOF state/position after ReadAll ().
                 *
                 *      \req upTo >= 1
                 *
                 *  ReadAll/2
                 *      Like Read, in that it reads all the elements that will fit into the range intoStart...intoEnd.
                 *      However, this guarantees to read all the data that will fit before returning (Read () only
                 *      guarantees to read at least one element).
                 *
                 *      So this can be handy when you KNOW you have a buffer large enough to read all the data in
                 *      the file, you can read without having to check the number of elements read and re-call Read().
                 *
                 *      ReadAll/2 will always return a size_t = intoEnd-intoStart unless it encounters EOF before filling
                 *      the entire buffer.
                 *
                 *      \req intoEnd-intoStart >= 1
                 *
                 *  \note ReadAll () will block if the stream is not KNOWN to be at EOF, and we just ran out of data. Use
                 *        @see ReadSome () to get non-blocking read behavior.
                 *
                 *  @todo DOCUMENT EDGE CONDITIONS - like run out of bytes to read full String - or can we return less than requested number (answer yes - but IFF EOF).
                 *  @see ReadPOD()
                 */
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual Characters::String ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const;
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                nonvirtual Memory::BLOB ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const;
                nonvirtual size_t ReadAll (ElementType* intoStart, ElementType* intoEnd) const;
            };

            template <>
            template <>
            Characters::Character InputStream<Characters::Character>::ReadCharacter () const;
            template <>
            template <>
            Characters::String InputStream<Characters::Character>::ReadLine () const;
            template <>
            template <>
            Traversal::Iterable<Characters::String> InputStream<Characters::Character>::ReadLines () const;
            template <>
            template <>
            Characters::String InputStream<Characters::Character>::ReadAll (size_t upTo) const;
            template <>
            template <>
            Memory::BLOB InputStream<Memory::Byte>::ReadAll (size_t upTo) const;

            /**
             *
             */
            template <typename ELEMENT_TYPE>
            class InputStream<ELEMENT_TYPE>::_IRep : public Stream<ELEMENT_TYPE>::_IRep {
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
                virtual SeekOffsetType GetReadOffset () const = 0;

            public:
                /*
                 *  \req IsSeekable ()
                 */
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) = 0;

            public:
                /**
                 *  Pointer must refer to valid memory at least 1 ELEMENT_TYPE long, and cannot be nullptr.
                 *  bufSize (intoEnd-intoStart) must always be >= 1. Returns 0 iff EOF, and otherwise number of ELEMENT_TYPE elements read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 *
                 *      \req (intoEnd - intoStart) >= 1
                 */
                virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) = 0;

            public:
                /**
                 *  see Read () - except intoStart may == nullptr iff intoEnd == nullptr, and otehrwise they must refer to at least a single element
                 *
                 *  \note   https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
                 *
                 *      \req  ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1)
                 */
                virtual Memory::Optional<size_t> ReadSome (ElementType* intoStart, ElementType* intoEnd) = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStream.inl"

#endif /*_Stroika_Foundation_Streams_InputStream_h_*/
