/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_h_
#define _Stroika_Foundation_Streams_InputStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <optional>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"
#include "../Traversal/Iterable.h"

#include "Stream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Consider making GetOffsetToEndOfStream () a virtual part of rep so it can work with the locks
 *              and be safely atomic (atomic stuff only an lssue for 'Synchronized' stream and could probably just
 *              make both calls inside ITS Synchronized lock?).
 *
 *      @todo   Consider making LineEnd format (LF,CR,CRLF, or Auto) an optional param to ReadLine().
 *              Then it would ONLY require Seekable() for CRLF or Auto.
 *
 *      @todo   Consider if IsAtEOF() should be added to the virtual rep? Easy, and can provide (current) default implemntation. But
 *              putting it there allows it to be much cheaper and its called pretty often (avoid seek logic). Wouldnt change semantics (about possibly blocking).
 */

namespace Stroika::Foundation {
    namespace Characters {
        class Character;
        class String;
    }
    namespace Memory {
        class BLOB;
    }
}

namespace Stroika::Foundation {
    namespace Streams {

        /**
         * Design Overview:
         *
         *      o   @See Stream<T>::Ptr
         *
         *      o   Nearly all read's on an InputStream<T>::Ptr are BLOCKING. If there is a desire to have a
         *          non-blocking read, then call ReadNonBlocking (). All operations block except ReadNonBlocking ().
         *
         *      o   EOF is handled by a return value of zero. Once EOF is returned - any subsequent
         *          calls to Read () will return EOF (unless some other mechanism is used to tweak
         *          the state of the object, like a mixed in Seekable class and calling SEEK, or a MemoryStream where
         *          someone writes more data to it).
         *
         *              Seek called on an input stream that has already returned EOF, may cause
         *              subsequent Read() calls to NOT be at EOF.
         *
         *      o   Exceptions indicate something went wrong - like an IO error, or possibly in some cases
         *          a formatting effort (e.g. if the source is encrypted, and the stream is decrypting,
         *          then it might detect a format error and throw).
         *
         *      o   InputStreamPtr and OutputStream may be logically be mixed together to make an
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
         *          Stroika's approach to this is to essentially everywhere in the InputStreamPtr API, have the methods be
         *          blocking (including seek, when supported). But when you must do non-blocking IO, you can call ReadNonBlocking ()
         *          which mostly tells you if there is any data available to read (and variants will also read it for you).
         *
         *  \note ***Cancelation Point***
         *        @todo not 100% sure we want to require this as part of API - but tentatively lets say
         *        All subtypes of InputStream () must have the Read (and related Peek etc) methods implementation be a cancelation point so that those reads can be aborted.
         *        @todo think out Seek () - not sure about this - but probably
         */
        template <typename ELEMENT_TYPE>
        class InputStream : public Stream<ELEMENT_TYPE> {
        protected:
            /**
             *  'InputStream' is a quasi-namespace: use Ptr or New () members.
             */
            InputStream ()                   = delete;
            InputStream (const InputStream&) = delete;

        public:
            using ElementType = typename Stream<ELEMENT_TYPE>::ElementType;

        public:
            class Ptr;

        public:
            class _IRep;

        protected:
            using _SharedIRep = shared_ptr<_IRep>;

        protected:
            /**
             *  Utility to create a Ptr wrapper (to avoid having to subclass the Ptr class and access its protected constructor)
             */
            static Ptr _mkPtr (const _SharedIRep& s);
        };

        /**
         *  \brief  InputStream<>::Ptr is Smart pointer (with abstract Rep) class defining the interface to reading from
         *          a Stream source of data.
         *
         *  @see InputStream<ELEMENT_TYPE>
         *
         *  \note Since InputStream<ELEMENT_TYPE>::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
         *        the underlying thread object.
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
         */
        template <typename ELEMENT_TYPE>
        class InputStream<ELEMENT_TYPE>::Ptr : public Stream<ELEMENT_TYPE>::Ptr {
        private:
            using inherited = typename Stream<ELEMENT_TYPE>::Ptr;

        protected:
            using AssertExternallySynchronizedLock = typename inherited::AssertExternallySynchronizedLock;

        protected:
            using _SharedIRep = shared_ptr<_IRep>;

        public:
            /**
             *  defaults to null (empty ())
             */
            Ptr () = default;
            Ptr (nullptr_t);
            Ptr (const Ptr&) = default;
            Ptr (Ptr&&)      = default;

        protected:
            /**
             * _SharedIRep rep is the underlying shared output Stream object.
             *
             *  \req rep != nullptr (use nullptr_t constructor)
             */
            explicit Ptr (const _SharedIRep& rep);

        public:
            /**
             */
            nonvirtual Ptr& operator= (const Ptr&) = default;
            nonvirtual Ptr& operator= (Ptr&&) = default;

        public:
            /**
             *  Put the input stream in a state where it cannot be read from anymore.
             *  If argument 'reset' is true, this also clears the smart pointer (calls Stream<>::reset()).
             *
             *  It is generally unneeded to ever call Close () - as streams are closed automatically when the final
             *  reference to them is released (smartptr).
             *
             *  But - this can be handy to signal to the reader from the stream (how this works depends on subtype) - that there
             *  will be no more reading done.
             *
             *  \note Most calls on an InputStream after it is closed are illegal, and result in Require () errors. It is up
             *        to the caller/user of the shared output streams to assure they dont use them after being closed. Note - if that
             *        sounds hard its not: it naturally falls out of normal usage.
             *
             *        The rationale is just that I can think of no useful case where a caller might want to allow writing after close, and
             *        have that translated into an exception, and this choice is more performant (and could be reversed more easily
             *        than the opposite policy if I change my mind).
             *
             *  \note Close () - and IsOpen () are intentionally duplicated in InputStream () and OutputStream () classes. This is so
             *        you can close down the OutputStream side of an InputOutputStream, and leave open the InputStream side - so it sees EOF.
             *
             *  \note When a subtype stream (like BufferedInputStream) aggregates another stream, it is left to that subclass
             *        whether or not closing the top level stream also Closes the sub-stream. Typically, if the class designer intends
             *        you to think of the ownership as 'aggregation' - close of this stream will close any aggregated streams, but
             *        if this class is designed to 'reference' other streams, it will not close them.
             *
             *        See the stream documentation for that stream class to see how it handles Close.
             *
             *  \req IsOpen ()
             */
            nonvirtual void Close () const;
            nonvirtual void Close (bool reset);

        public:
            /**
             *  Return true, unless a call to Close () has been done on the underlying stream (not just Ptr).
             *
             *  \note InputStream and OutputStream (when mixed in InputOutputStream) have separate IsOpen/IsClosed flags, so you
             *        can call Close on the write side of the stream and still read from the InputStream side.
             *
             *  @see Close ()
             */
            nonvirtual bool IsOpen () const;

        public:
            /**
             * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
             *
             *  \req IsOpen ()
             *
             *  \note does NOT require IsSeekable () so always must be supported by rep
             */
            nonvirtual SeekOffsetType GetOffset () const;

        public:
            /**
             * GetOffsetToEndOfStream () returns the the distance from the current offset position to the end of the stream.
             *  This is equivalent to:
             *      SeekOffsetType savedReadFrom = GetOffset ();
             *      size_t  size =  Seek (Whence::eFromEnd, 0);
             *      Seek (savedReadFrom);
             *      return size - savedReadFrom;
             *(EXCPET MAYBE GUARANTEED ATOMIC????)
             *  \note   @todo Not sure how useful this is - I can find no calls to this in code base
             *          Maybe for input stream to see how big a buffer to allocate to read? But even then
             *          probably not a great strategy -- LGP 2017-06-16
             *
             *  \req IsSeekable ()
             */
            nonvirtual SeekOffsetType GetOffsetToEndOfStream () const;

        public:
            /**
             * The new position, measured in Stream element increments, is obtained by adding offset value to the position
             *  specified by whence.
             *
             *  Seek () past the end of stream or before the start is legal (not assertion error) - but may result in an exception (possibly not until next read).
             *
             *  Seek () returns the new resulting position (measured from the start of the stream - same as GetOffset).
             *
             *  \req IsSeekable ()
             */
            nonvirtual SeekOffsetType Seek (SeekOffsetType offset) const;
            nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset) const;

        public:
            /**
             *  Read/0
             *      return nullopt on EOF, and otherwise return a single element. Read/0 will block if no data available.
             *
             *  Read/2
             *      Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
             *      bufSize (intoEnd-intoStart) must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
             *      BLOCKING until data is available, but can return with fewer bytes than bufSize
             *      without prejudice about how much more is available.
             *
             *      So if you call first with ReadNonBlocking() to assure there is some data available (at least one element) you can
             *      call Read (BUFSTART,BUFFEND) with as big a buffer as you want, and it won't block.
             *
             *  All overloads:
             *      It is legal to call Read () if its already returned EOF, but then it MUST return EOF again.
             *
             *      \req (intoEnd - intoStart) >= 1
             *
             *  @see ReadAll () to read all the data in the file at once.
             *
             *  @see ReadNonBlocking ()
             */
            nonvirtual optional<ElementType> Read () const;
            nonvirtual size_t Read (ElementType* intoStart, ElementType* intoEnd) const;

        public:
            /**
             *  \brief  Same as Read, but \req IsSeekable, and seeks back to original position
             *
             *  Peek/0
             *      return nullopt on EOF, and otherwise return a single element.
             *
             *  Peek/2
             *      Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
             *      bufSize (intoEnd-intoStart) must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
             *      BLOCKING until data is available, but can return with fewer bytes than bufSize
             *      without prejudice about how much more is available.
             *
             *  \note    Peek will block if no data available.
             *
             *      \req (intoEnd - intoStart) >= 1
             *      \req IsSeekable ()
             *
             */
            nonvirtual optional<ElementType> Peek () const;
            nonvirtual size_t Peek (ElementType* intoStart, ElementType* intoEnd) const;

        public:
            /**
             *  \brief check if the stream is currently at EOF (blocking call)
             *
             *  \note - this does a Read () call - which can block, to check for EOF. Use ReadNonBlocking () to avoid blocking.
             *
             *  \req IsSeekable ()
             */
            nonvirtual bool IsAtEOF () const;

        public:
            /**
             *  \brief  similar to Read () - except that it is non-blocking, and will return Memory::nullopt if no data available without blocking.
             *
             *  \note   https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
             *
             *      ReadNonBlocking/0:
             *          Returns the number of elements definitely available to read without blocking. nullopt return value means
             *          no data available, and 0 return value means @ EOF.
             *
             *          This often will return 1 when more data is available, but due to the semantics of Read() allowing a read of
             *          less than requested, just knowing there is some data is sufficient.
             *  
             *          Note this cannot be reliably used to check for EOF, because a return value of Missing () - maybe EOF, and may not be.
             *
             *          Note this does not adjust the seek pointer, because it doesn't read anything
             *
             *      ReadNonBlocking/2:
             *          Never blocks. Read up to the amount specified in the arguments (intoEnd-intoStart), and return Missing/nullopt
             *          if nothing can be read without blocking, or 0 for EOF, or > 0 for actual elements read.
             *
             *      Like Read (), it is legal to call ReadNonBlocking () if its already returned EOF, but then it MUST return EOF again.
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
             *  \note   Returns Memory::nullopt means no data immediately and definitely available. But sometimes its not possible
             *          to be sure, so this could return nullopt - even when a blocking read could have read something.
             *
             *  \note   We may need to abandon this experimental API because:
             *              1>  It makes building Reps more complicated
             *              2>  Its not that useful without guarantees of not blocking and gaurantees that when you read and get back
             *                  nullopt, there is no point in reading
             *              3>  it is easily workaroundable using a Blocking Queue and another thread to read actual data
             *              4>  Original mandate for this Streams class module was simplicity of use, extension etc, and this makes it harder.
             *
             *      \req (intoEnd - intoStart) >= 1
             *
             *  @see Read ()
             *  @see ReadAll ()
             */
            nonvirtual optional<size_t> ReadNonBlocking () const;
            nonvirtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const;

        public:
            /**
             *  @todo Consider if we should lose this. Optional appraoch maybe better.
             *
             *  Blocking read of a single character. Returns a NUL-character on EOF ('\0')
             */
            template <typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>* = nullptr>
            nonvirtual Characters::Character ReadCharacter () const;

        public:
            /**
             * shorthand for declaring
             *      POD_TYPE    tmp;
             *      size_n = ReadAll ((Byte*)&tmp, (Byte*)(&tmp+1));
             *      if (n==sizeof(tmp)) {  return tmp; } else throw EOFException (...);
             *
             *  \note   If not enough bytes are available to return a POD_TYPE, EOFException will be thrown.
             *  \note   Only defined on Binary Streams (InputStream<Byte>::Ptr), but POD_TYPE can be any (is_pod) type.
             *  \note   ReadRaw will read exactly the number of records requested, or throw an EOF exception.
             */
            template <typename POD_TYPE, typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Memory::Byte>>* = nullptr>
            nonvirtual POD_TYPE ReadRaw () const;
            template <typename POD_TYPE, typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Memory::Byte>>* = nullptr>
            nonvirtual void ReadRaw (POD_TYPE* start, POD_TYPE* end) const;

        public:
            /**
             * Readline looks for a trailing bare CR, or bare LF, or CRLF. It returns whatever line-terminator
             * it encounters as part of the read line.
             *
             *  ReadLine() will return an empty string iff EOF.
             *
             *      \req IsSeekable ()
             */
            template <typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>* = nullptr>
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
            template <typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>* = nullptr>
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
             *        @see ReadNonBlocking () to get non-blocking read behavior.
             *
             *  @todo DOCUMENT EDGE CONDITIONS - like run out of bytes to read full String - or can we return less than requested number (answer yes - but IFF EOF).
             *  @see ReadRaw()
             */
            template <typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>* = nullptr>
            nonvirtual Characters::String ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const;
            template <typename TEST_TYPE = ELEMENT_TYPE, enable_if_t<is_same_v<TEST_TYPE, Memory::Byte>>* = nullptr>
            nonvirtual Memory::BLOB ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const;
            nonvirtual size_t ReadAll (ElementType* intoStart, ElementType* intoEnd) const;

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

        private:
            friend class InputStream<ELEMENT_TYPE>;
        };

        template <>
        template <>
        Characters::Character InputStream<Characters::Character>::Ptr::ReadCharacter () const;
        template <>
        template <>
        Characters::String InputStream<Characters::Character>::Ptr::ReadLine () const;
        template <>
        template <>
        Traversal::Iterable<Characters::String> InputStream<Characters::Character>::Ptr::ReadLines () const;
        template <>
        template <>
        Characters::String InputStream<Characters::Character>::Ptr::ReadAll (size_t upTo) const;
        template <>
        template <>
        Memory::BLOB InputStream<Memory::Byte>::Ptr::ReadAll (size_t upTo) const;

        /**
         *  \note   \em Thread-Safety   <a href="thread_safety.html#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype/a>
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
            /**
             *  May (but typically not) called before destruction. If called, \req no other write or seek etc operations.
             *
             *  \note - 'Require (IsOpen()) automatically checked in Ptr wrappers for things like Read, so subclassers dont need to
             *          do that in implementing reps, but probably good docs/style todo in both places.'
             */
            virtual void CloseRead () = 0;

        public:
            /**
             *  return true iff CloseRead () has not been called (cannot construct closed stream)
             */
            virtual bool IsOpenRead () const = 0;

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
             *  @see InputStream<>::ReadNonBlocking () for the details of the read semantics.
             *
             *  For simple cases, where the amount of data available to read can be computed easily, just implement this as:
             *      \code
             *          return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, NUMBER_OF_ELTS_DEFINITELY_AVAILABLE)
             *      \endcode
             *
             *  For trickier cases, where the upstream source may block, a helpful pattern is:
             *      \code
             *          do code to check if anything upstream is avaialble - like ::select () - or fUpstream.ReadNonBlocking() and if we find no
             *          data, return {} and if we find at least n bytes avail, set NUMBER_OF_ELTS_DEFINITELY_AVAILABLE = n and fallthrough...
             *          return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, NUMBER_OF_ELTS_DEFINITELY_AVAILABLE)
             *      \endcode
             *
             *  A legal (but not very useful) implementation would be:
             *      \code
             *          return {};  // no data KNOWN to be available - you must make blocking call to find out!
             *      \endcode
             *
             *  \req  ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1)
             *
             *  \note similar to basic_istream::readsome - http://en.cppreference.com/w/cpp/io/basic_istream/readsome
             */
            virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) = 0;

        protected:
            /**
             *  Implementers of _IRep where there is no 'non-blocking' mode supported (always the same as blocking) - can
             *  simply call this in the ReadNonBlocking () override.
             *
             *  The only 'hitch' is that the _IRep subtype using this must know the number of elements available, and pass that in.
             *  All that really matters is if this is 0 or 1, but best if you can pass in the actual value.
             */
            nonvirtual optional<size_t> _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (ElementType* intoStart, ElementType* intoEnd, size_t elementsRemaining);
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStream.inl"

#endif /*_Stroika_Foundation_Streams_InputStream_h_*/
