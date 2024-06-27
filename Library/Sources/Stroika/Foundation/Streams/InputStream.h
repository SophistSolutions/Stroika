/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_h_
#define _Stroika_Foundation_Streams_InputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>
#include <optional>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Streams/EWouldBlock.h"
#include "Stroika/Foundation/Streams/Stream.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Consider making LineEnd format (LF,CR,CRLF, or Auto) an optional param to ReadLine().
 *              Then it would ONLY require Seekable() for CRLF or Auto.
 *
 *      @todo   Consider if IsAtEOF() should be added to the virtual rep? Easy, and can provide (current) default implementation. But
 *              putting it there allows it to be much cheaper and its called pretty often (avoid seek logic). Wouldn't change semantics (about possibly blocking).
 */

namespace Stroika::Foundation::Characters {
    class Character;
    class String;
}
namespace Stroika::Foundation::Memory {
    class BLOB;
}

namespace Stroika::Foundation::Streams::InputStream {

    template <typename ELEMENT_TYPE>
    class IRep;

    /**
     *  \em Design Overview
     *
     *      o   @See Streams::Ptr<T>
     *
     *      o   Nearly all read's on an InputStream::Ptr<T> are BLOCKING. If there is a desire to have a
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
     *      o   InputStream::Ptr and OutputStream may be logically be mixed together to make an
     *          input/output stream: @see InputOutputStream
     *
     *      @see Stroika::Foundation::Streams::iostream for adapters to work with std::iostream.
     *
     *      @see MemoryStream for most common stream applications.
     *
     *      @see StreamReader for utility to more easily, and more efficiently, consume the contents
     *      of an input stream.
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
     *          mechanism. But we don't want to always require seeking.
     *
     *          PutBack () is like Seek, but then the question is - do we support just a PutBack of
     *          one character? So only lookahead of one character? That deals with many cases, but not all.
     *          And how does it interact with Seek - if the stream happens to be seekable? And is the
     *          PutBack buffer stored in the letter or envelope class? If in Letter, that is extra work
     *          in every rep (barrier to providing your own stream subtypes). If in the envelope, it doesn't
     *          work intuitively if two variables have separate smart pointers to the same underlying stream.
     *          Reads and writes affect each other EXCPET for the putback buffer!
     *
     *          A special Proxy object is a good choice - but requires the caller todo a lot of extra
     *          work. To some extent that can be automated by somewhat automatically managing
     *          the proxy storage object in the smart pointer, but that's a lot of work and concept
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
     *          But - it suffers from the grave flaw that when you write generic code or routines, you must write it
     *          to handle both blocking and non-blocking cases, and these are really quite different.
     *
     *          Mostly - blocking is MUCH simpler to read/write/debug. But you NEED non-blocking sometimes for efficiency
     *          reasons (to manage progress through a long operation).
     *
     *          Stroika Stream's approach to this is to essentially everywhere in the InputStream::Ptr API, have the methods
     *          take a flag indicating if the caller expects blocking or not (defaulting to blocking).
     *
     *          This adds very little complexity to the implementation classes (reps) and essentially zero to the callers
     *          (since they default to blocking behavior).
     *
     *          And with only modest effort (explicit flag in call) you can get the non-blocking behavior.
     * 
     *          One twist has todo with what todo in the non-blocking case when data isn't available. One
     *          choice would be to return optional (or some other flagging / sentinel approach) and the other is to throw.
     *          Throw behavior is much simpler to write code and reason about. Returning a flag/optional is possibly
     *          more efficient (depends - you have extra flag being copied around, but you avoid the cost of handling exceptions
     *          - which wins the performance balance depends on the costs of a throw, and the frequency of a throw.
     * 
     *          Stroika Streams generally use the throw (EWouldBlock) approach. However, in the 'Rep' classes, they use the
     *          return optional approach, and this is exposed through the ReadNonBlocking Ptr API. Plus they expose an
     *          AvailableToRead () API;
     * 
     *          So generally - the Stroika approach is that for the casual observer (defaults) - things just work with blocking
     *          reads, but if you need to do non-blocking reads, this is pretty straightforward.
     *
     *  \note   For better performance, but some slight usage restrictions, consider using @see StreamReader
     * 
     *  \note ***Cancelation Point***
     *        @todo not 100% sure we want to require this as part of API - but tentatively lets say
     *        All subtypes of InputStream () must have the Read (and related Peek etc) methods implementation be a cancelation point so that those reads can be aborted.
     *        @todo think out Seek () - not sure about this - but probably
     */

    /**
     *  \brief  InputStream<>::Ptr is Smart pointer (with abstract Rep) class defining the interface to reading from
     *          a Stream source of data.
     *
     *  @see InputStream<ELEMENT_TYPE>
     *
     *  \note Since InputStream::Ptr<ELEMENT_TYPE> is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
     *        the underlying thread object.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public Streams::Ptr<ELEMENT_TYPE> {
    private:
        using inherited = typename Streams::Ptr<ELEMENT_TYPE>;

    public:
        using ElementType = ELEMENT_TYPE;

    public:
        /**
         *  defaults to null (empty ())
         *
         * shared_ptr<IRep> rep is the underlying shared input Stream object.
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr&)     = default;
        Ptr (Ptr&&) noexcept = default;
        Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep);
        template <typename ASSTREAMABLE>
        Ptr (ASSTREAMABLE&& src)
            requires
        // NOTE qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy workaround NOT required DIRECTLY HERE but due to similar hack in Memory::BLOB::As()...
#if qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
            same_as<byte, ELEMENT_TYPE> and
#endif
            requires (ASSTREAMABLE) { src.template As<Ptr<ELEMENT_TYPE>> (); }

#if qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
            : inherited{src.template As<Ptr<ELEMENT_TYPE>> ()} {}
#endif
            ;

    public:
        /**
         */
        nonvirtual Ptr& operator= (Ptr&&) noexcept = default;
        nonvirtual Ptr& operator= (const Ptr&)     = default;

    public:
        /**
         *  Put the input stream in a state where it cannot be read from anymore.
         *  If argument 'reset' is true, this also clears the smart pointer (calls Stream<>::reset()).
         *
         *  It is generally unneeded to ever call Close () - as streams are closed automatically when the final
         *  reference to them is released (smart ptr).
         *
         *  But - this can be handy to signal to the reader from the stream (how this works depends on subtype) - that there
         *  will be no more reading done.
         *
         *  \note Most calls on an InputStream after it is closed are illegal, and result in Require () errors. It is up
         *        to the caller/user of the shared output streams to assure they don't use them after being closed. Note - if that
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
         *  \brief returns nullopt if nothing known available, zero if known EOF, and any other number of elements (typically 1) if that number know to be available to read
         *
         *  \see also (different) RemainingLength ()
         */
        nonvirtual optional<size_t> AvailableToRead () const;

    public:
        /**
         * experimental API (added in 3.0d7)
         * return nullopt if nothing available
         * else do Read() of all elements available and return that buffer
         */
        nonvirtual optional<Memory::InlineBuffer<ELEMENT_TYPE>> ReadAllAvailable () const;

    public:
        /**
         *  \brief returns nullopt if not known (typical, and the default) - but sometimes it is known, and quite helpful)
         * 
         *  \note - Similar to AvailableToRead, but different. For example, on a socket-stream, you can tell how many bytes
         *        are available to read (buffered by kernel). But no guess about the remaining length of the stream (how many bytes
         *        will appear before end).
         * 
         *        But for a disk file, you MIGHT (not always - like unix special files) know the length of the file. This is for that case.
         */
        nonvirtual optional<SeekOffsetType> RemainingLength () const;

    public:
        /**
         *  Read/0
         *      return nullopt on EOF, and otherwise return a single element. Read/0 will block if no data available.
         *
         *  Read/1
         *      Pointer must refer to valid non-empty span.
         *      Returns empty iff EOF, and otherwise subspan of original span (starting at 0) actually read.
         *      BLOCKING until data is available, but can return with fewer elements than argument span-size
         *      without prejudice about how much more is available.
         *
         *      So if you call first with ReadNonBlocking() to assure there is some data available (at least one element) you can
         *      call Read (BUFSTART,BUFFEND) with as big a buffer as you want, and it won't block.
         *
         *  All overloads:
         *      It is legal to call Read () if its already returned EOF, but then it MUST return EOF again.
         *
         *  @see ReadAll () to read all the data from the stream at once.
         *
         *  @see ReadNonBlocking ()
         */
        nonvirtual optional<ElementType> Read (NoDataAvailableHandling blockFlag = NoDataAvailableHandling::eDEFAULT) const;
        nonvirtual span<ElementType> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag = NoDataAvailableHandling::eDEFAULT) const;

    public:
        /**
         * almost identical to Read (intoBuffer, NoDataAvailableHandling::eThrow except doesn't throw in that case but returns nullopt
         */
        nonvirtual optional<span<ElementType>> ReadNonBlocking (span<ElementType> intoBuffer) const;

    public:
        /**
         *  \brief  Same as Read, but \req IsSeekable, and seeks back to original position
         *
         *  Peek/0
         *      return nullopt on EOF, and otherwise return a single element.
         *
         *  Peek/1
         *      Pointer must refer to valid non-empty span.
         *      Returns empty iff EOF, and otherwise subspan of original span (starting at 0) actually read.
         *      BLOCK until some data is available, but can return with fewer bytes than bufSize
         *      without prejudice about how much more is available.
         *
         *  \note    Peek will block if no data available, by default, unless eDontBlock argument is specified, in which case it will throw EWouldBlock rather than block
         * 
         *  \note Peek () will always throw EWouldBlock for all cases where eDontBlock specified and the call might block).
         * 
         *  \note for this API, nullopt means KNOWN NO DATA, not EWouldBlock
         *
         *      \req not intoBuffer.empty ()
         *      \req IsSeekable ()
         */
        nonvirtual optional<ElementType> Peek (NoDataAvailableHandling blockFlag = NoDataAvailableHandling::eDEFAULT) const;
        nonvirtual span<ElementType> Peek (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag = NoDataAvailableHandling::eDEFAULT) const;

    public:
        /**
         *  \brief check if the stream is currently at EOF
         *
         *  \note - IsAtEOF/0 does a blocking Read () call.
         *  \note - IsAtEOF (eDontBlock) returns optional<bool> - nullopt if would block, and false if known not at EOF, and true if known EOF; 
         *          this differs from most Stroika streams APIs - in that nullopt here means 'EWouldBlock';
         *
         *  \req IsSeekable ()
         */
        nonvirtual bool IsAtEOF () const;
        nonvirtual optional<bool> IsAtEOF (NoDataAvailableHandling blockFlag) const;

    public:
        /**
         *  @todo Consider if we should lose this. Optional approach maybe better.
         *
         *  Read of a single character. Returns a NUL-character on EOF ('\0');
         * 
         *  Blocking (by default), but if you pass eDontBlock, will throw EWouldBlock if would have blocked
         */
        nonvirtual Characters::Character ReadCharacter (NoDataAvailableHandling blockFlag = NoDataAvailableHandling::eDEFAULT) const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  \brief Read a single (or span of) POD_TYPE objects, like with Read () - except always blocking, and treating stream of bytes as composing a single POD_TYPE object
         * 
         *  \note ReadRaw(span > 1 element) requires IsSeekable()
         *
         *  \note   If not enough data available to return a single POD_TYPE, EOFException will be thrown.
         *  \note   Only defined on Binary Streams (InputStream::Ptr<byte>), but POD_TYPE can be any (is_pod) type.
         *  \note   ReadRaw will read a whole number of records requested (> 0, seeking to adjust if necessary. but it may
         *          return any number (>=1 but <= size of input span).
         *          It may result in Seek being called to return a smaller number of records than requested.
         * 
         *  This API is always blocking (see ReadAll () for reasons why - need way to undo - seek or buffer - but may extend this API in the future).
         */
        template <typename POD_TYPE>
        nonvirtual POD_TYPE ReadRaw () const
            requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>);
        template <typename POD_TYPE>
        nonvirtual span<POD_TYPE> ReadRaw (span<POD_TYPE> intoBuffer) const
            requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>);

    public:
        /**
         * Readline looks for a trailing bare CR, or bare LF, or CRLF. It returns whatever line-terminator
         * it encounters as part of the read line.
         *
         *  ReadLine() will return an empty string iff EOF.
         *
         *      \req IsSeekable () to implement read-ahead required for CRLF mapping support 
         *
         *  This API is always blocking (perhaps someday overloads will allow non-blocking but very low priority)
         */
        nonvirtual Characters::String ReadLine () const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  Returns Iterable<String> object, so you can
         *  write code:
         *          for (String line : stream.ReadLines ()) {
         *          }
         *
         *  Like ReadLine(), the returned lines include trailing newlines/etc.
         * 
         *  However, UNLIKE ReadLine(), this function does NOT require the input stream be seekable!
         *
         *  This API is always blocking (perhaps someday overloads will allow non-blocking but very low priority)
         */
        nonvirtual Traversal::Iterable<Characters::String> ReadLines () const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  ReadAll/size_t upTo
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
         *  ReadAll/span<ElementType> intoBuffer
         *      Like Read, in that it reads all the elements that will fit into the range intoBuffer.
         *      However, this guarantees to read all the data that will fit before returning (Read () only
         *      guarantees to read at least one element).
         *
         *      So this can be handy when you KNOW you have a buffer large enough to read all the data in
         *      the file, you can read without having to check the number of elements read and re-call Read().
         *
         *      ReadAll will always return a subspan of intoBuffer (or empty).
         *
         *      \req intoEnd-intoStart >= 1
         *
         *  \note ReadAll () will block if the stream is not KNOWN to be at EOF, and we just ran out of data. Use
         *        @see ReadNonBlocking () or Read (eDontBlock) to get non-blocking read behavior.
         * 
         *  \note ReadAll is ONLY available in a blocking form, because to handle to handle the non-blocking case
         *        we might need to either seek back, or have internal buffering to manage a partial read (could posssibly
         *        extend API like this in the future but no need).
         *
         *  @see ReadRaw()
         *  @see Streams::CopyAll()
         * 
         *  \note ReadAll -> BLOB <template> in cpp file and templated just due to deadly include embrace.
         *        cannot get working with require() since to be in CPP file, need to use template
         *        specialization, and cannot specify requires with template specialization (or I cannot figure out how)
         */
        nonvirtual Characters::String ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);
        nonvirtual Memory::BLOB ReadAll (size_t upTo = numeric_limits<size_t>::max ()) const
#if 0
            requires (same_as<ELEMENT_TYPE, byte>)  // get compile error on vis studio when we specailize - not sure if LGP bug or compiler bug...--LGP 2023-12-28
#endif
            ;
        template <typename ELEMENT_TYPE2, size_t EXTENT2_T>
        nonvirtual span<ElementType> ReadAll (span<ELEMENT_TYPE2, EXTENT2_T> intoBuffer) const
            requires (same_as<ELEMENT_TYPE, ELEMENT_TYPE2>);

    public:
        /**
         *  \brief access to underlying stream smart pointer
         */
        nonvirtual shared_ptr<IRep<ELEMENT_TYPE>> GetSharedRep () const;

    public:
        /**
         * \req *this != nullptr
         */
        nonvirtual const IRep<ELEMENT_TYPE>& GetRepConstRef () const;

    public:
        /**
         * \req *this != nullptr
         */
        nonvirtual IRep<ELEMENT_TYPE>& GetRepRWRef () const;

    public:
        [[deprecated ("Since Stroika v3.0d5 use IsDataAvailableToRead ()")]] optional<size_t> ReadNonBlocking () const;
        [[deprecated ("Since Stroika v3.0d5 use Read (span, NoDataAvailableHandling::eDontBlock )")]] optional<size_t>
        ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const;
        [[deprecated ("Since Strokka v3.0d5 deprecated since not widely used and very specific purpose and directly implementingable given "
                      "apis")]] SeekOffsetType
        GetOffsetToEndOfStream () const
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
            Require (IsOpen ());
            SeekOffsetType savedReadFrom = GetOffset ();
            SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
            Seek (Whence::eFromStart, savedReadFrom);
            Assert (size >= savedReadFrom);
            size -= savedReadFrom;
            return size;
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload")]] size_t Read (ElementType* intoStart, ElementType* intoEnd) const
        {
            return Read (span{intoStart, intoEnd}).size ();
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload")]] size_t Peek (ElementType* intoStart, ElementType* intoEnd) const
        {
            return Peek (span{intoStart, intoEnd}).size ();
        }
        template <typename POD_TYPE>
        [[deprecated ("Since Stroika v3.0d5 use span overload")]] void ReadRaw (POD_TYPE* start, POD_TYPE* end) const
            requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
        {
            ReadRaw (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload")]] size_t ReadAll (ElementType* intoStart, ElementType* intoEnd) const
        {
            return ReadAll (span{intoStart, intoEnd}).size ();
        }
    };

    /**
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype/a>
     *
     */
    template <typename ELEMENT_TYPE>
    class IRep : public Streams::IRep<ELEMENT_TYPE> {
    public:
        using ElementType = ELEMENT_TYPE;

    public:
        IRep ()            = default;
        IRep (const IRep&) = delete;

    public:
        virtual ~IRep () = default;

    public:
        nonvirtual IRep& operator= (const IRep&) = delete;

    public:
        /**
         *  May (but typically not) called before destruction. If called, \req no other read or seek etc operations.
         *
         *  \note - 'Require (IsOpen()) automatically checked in Ptr wrappers for things like Read, so subclassers don't need to
         *          do that in implementing reps, but probably good docs/style todo in both places.'
         * 
         *  \note this could have just be called 'close' but we want to be able to mix InputStream::IRep and OutputStream::IRep without conflict.
         */
        virtual void CloseRead () = 0;

    public:
        /**
         *  return true iff CloseRead () has not been called (cannot construct closed stream)
         * 
         *  \note this could have just be called 'IsOpen' but we want to be able to mix InputStream::IRep and OutputStream::IRep without conflict.
         */
        virtual bool IsOpenRead () const = 0;

    public:
        /**
         *  \note this could have just be called 'GetOffset' but we want to be able to mix InputStream::IRep and OutputStream::IRep without conflict.
         */
        virtual SeekOffsetType GetReadOffset () const = 0;

    public:
        /*
         *  \req IsSeekable ()
         *
         *  \note this could have just be called 'Seek' but we want to be able to mix InputStream::IRep and OutputStream::IRep without conflict.
         * 
         *  \note if not seekable (what method) - default does AssertNotImplemented (); so must override iff IsSeekable
         */
        virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset);

    public:
        /**
         *  \brief returns nullopt if nothing known available, zero if known EOF, and any other number of elements (typically 1) if that number know to be available to read
         * 
         *  Default implementation - if seekable - does a read, and then seeks back, so flexible, but fairly inefficient.
         *  Subclassers MUST re-implement this function if not IsSeekable (); and should re-implement for efficiency sake.
         */
        virtual optional<size_t> AvailableToRead ();

    public:
        /**
         *  \brief returns nullopt if not known (typical, and the default) - but sometimes it is known, and quite helpful)
         */
        virtual optional<SeekOffsetType> RemainingLength ();

    public:
        /**
         *  incoming intoBuffer must be a valid, non-empty span of elements (to be overwritten).
         * 
         *  Returns empty span iff EOF, and otherwise intoBuffer.subspan(0,number of ELEMENT_TYPE elements read).
         *  BLOCKING until data is available, but can return with fewer bytes than bufSize
         *  without prejudice about how much more is available.
         * 
         *  Blocking:
         *      o   If blockFlag == eBlockIfNoDataAvailable, always blocks until data available and returns non-nullopt span
         *      o   if blockFlag == eDontBlock, will return nullopt if would block, and else number of elements read
         *      In EITHER case, NEVER throws EWouldBlock (can throw other stuff). That is done by Ptr wrapper.
         */
        virtual optional<span<ElementType>> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStream.inl"

#endif /*_Stroika_Foundation_Streams_InputStream_h_*/
