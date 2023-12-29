/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_OutputStream_h_
#define _Stroika_Foundation_Streams_OutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Characters/Character.h"
#include "../Configuration/Common.h"
#include "../Memory/Common.h"

#include "Stream.h"

/**
 *
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Consider if Seek () past end of stream on writable stream should be
 *              allowed. Often - like in UNIX - this works - and you can then write there,
 *              and this creates a hole in the file read back as zeros.
 *
 *              Its easier to DISALLOW this now, and then lift the restriction, and later allow it,
 *              so since I'm unsure, disallow for now. This can always be simulated with an extra
 *              zero write, and it assuming no seek past EOF makes implementations simpler, and
 *              definition more consistent (read).
 *
 *      @todo   Add abiiliy to SetEOF (); You can SEEK, but if you seek backwards, and start writing - that doesn't change EOF. EOF
 *              remains fixed as max written to. DODUCMNET THIS (for text and binary) - and provide a SetEOF() method
 *              (maybe just for seekable streams)? Maybe add rule that SetEOF () can only go backwards (shorten). Then call
 *              PullBackEOF() or RestrictEOF() or RemovePast(); OR ResetEOFToCurrentPosiiton(). Later maybe best API.
 *
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where
 *              it was a PROPERTY OF THE CLASS (or alternate API) where writes timeout after
 *              a certain point.
 *
 */

namespace Stroika::Foundation::Characters {
    class String;
}
namespace Stroika::Foundation::Memory {
    class BLOB;
}

namespace Stroika::Foundation::Streams::OutputStream {

    template <typename ELEMENT_TYPE>
    class IRep;

    /**
     *  \em Design Overview
     *
     *      o   @See Stream
     *      o   @See OutputStream::Ptr<ELEMENT_TYPE>
     *
     *      o   InputStream::Ptr and OutputStream::Ptr may logically be mixed together to make an
     *          input/output stream: @see InputOutputStream::Ptr<ELEMENT_TYPE>
     *
     *      o   One (potential) slight design flaw with this API, is that its not possible to have legal partial writes.
     *          But not supporting partial writes makes use much simpler (since callers don't need
     *          to worry about that case), and its practically never useful. In principle - this API could be
     *          extended so that an exception (or extra method to ask about last write) could include information
     *          about partial writes, but for now - I don't see any reason.
     *
     *  Note - when you Seek() away from the end of an output stream, and then write, you automatically
     *  extend the stream to the point seeked to, and if you seek back (less) than the end and write, this overwrites
     *  instead of inserting.
     *
     *  Note - Write is sufficient to guarantee the data is written, but it may be buffered until you call
     *  the destructor on the OutputStream (last reference goes away) or until you call Flush ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     *
     *  \brief  OutputStream<>::Ptr is Smart pointer to a stream-based sink of data.
     *
     *  \note Since OutputStream::Ptr<ELEMENT_TYPE> is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
     *        the underlying thread object.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public Streams::Ptr<ELEMENT_TYPE> {
    private:
        using inherited = typename Streams::Ptr<ELEMENT_TYPE>;

    public:
        /**
         *  defaults to null (empty ())
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr&) = default;
        Ptr (Ptr&&)      = default;
        Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep);

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr&) = default;
        nonvirtual Ptr& operator= (Ptr&&)      = default;

    public:
        /**
         * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
         *
         *  \req IsOpen ()
         */
        nonvirtual SeekOffsetType GetOffset () const;

    public:
        /**
         * The new position, measured in bytes, is obtained by adding offset bytes to the position
         *  specified by whence.
         *
         *  Seek () past the end of stream is NOT legal (may reconsider).
         *
         *  Seek () returns the new resulting position (measured from the start of the stream - same as GetOffset).
         *
         *  \req IsOpen ()
         */
        nonvirtual SeekOffsetType Seek (SeekOffsetType offset) const;
        nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset) const;

    public:
        /**
         *  Write the given span of elements to the output stream.
         * 
         *  If ELEMENT_TYPE==byte
         *      then the argument may also be of type uint8_t
         *  if ELEMENT_TYPE=Character
         *      then the argument may also be of type String or something convertible to String (char8_t*, string_view, etc)
         *  
         *  Writes always succeed fully or throw (no partial writes).
         *
         *  \note The meaning of Write () depends on the exact type of Stream you are referencing. The data
         *        may still be buffered. Call @Flush () to get it pushed out.
         *
         *  \req IsOpen ()
         */
        template <typename ELEMENT_TYPE2, size_t EXTENT_2>
        nonvirtual void Write (span<const ELEMENT_TYPE2, EXTENT_2> elts) const
            requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                      (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)) or
                      (same_as<ELEMENT_TYPE, Characters::Character> and
                       (Characters::IUNICODECanUnambiguouslyConvertFrom<remove_cvref_t<ELEMENT_TYPE2>>)));
        nonvirtual void Write (const ELEMENT_TYPE& e) const;
        nonvirtual void Write (const Memory::BLOB& blob) const
            requires (same_as<ELEMENT_TYPE, byte>);
        nonvirtual void Write (const Characters::String& s) const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);
        template <Characters::IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual void Write (const CHAR_T* cStr) const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  For Character output streams only - do a Write () with the given argument, followed by a newline.
         */
        template <typename ELT_2_WRITE>
        nonvirtual void WriteLn (ELT_2_WRITE&& arg) const
            requires (same_as<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *      WriteRaw () only applies to binary (ELEMENT_TYPE=byte) streams. It allows easily writing POD (plain old data) types
         *      to the stream.
         *
         *      WriteRaw (X) is an alias for WriteRaw (span{&x, 1})
         *      WriteRaw (span) writes all the POD records from start to end to the binary stream.
         * 
         *  @todo consider LOSING this in favor or Write (as_bytes (X)) - since the same as that too!!
         * ``        see what as_bytes () does on a string...
         *
         *      \note Used to be called WritePOD (too easy to use mistakenly, and if you really want to do something like this with
         *            non-POD data, not hard, but we don't want to encourage it.
         *
         *  \req is_trivially_copyable_v<POD_TYPE> and is_standard_layout_v<POD_TYPE>
         *  \req ELEMENT_TYPE==byte
         *  \req IsOpen ()
         *
         *  shorthand for declaring
         *      POD_TYPE    tmp;
         *      Write ((byte*)&tmp, (byte*)(&tmp+1));
         */
        template <typename POD_TYPE>
        nonvirtual void WriteRaw (const POD_TYPE& p) const
            requires (is_same_v<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>);
        template <typename POD_TYPE>
        nonvirtual void WriteRaw (span<const POD_TYPE> elts) const
            requires (is_same_v<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>);

    public:
        /**
         *  \req IsOpen ()
         */
        nonvirtual void PrintF (const wchar_t* format, ...)
            requires (is_same_v<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  Put the output stream in a state where it cannot be written to anymore.
         *  If argument 'reset' is true, this also clears the smart pointer (calls Stream<>::reset()).
         *
         *  It is generally unneeded to ever call Close () - as streams are closed automatically when the final
         *  reference to them is released (smartptr).
         *
         *  But - this can be handy - in that it allows for exception handling. Exceptions closing out an output stream - doing
         *  final writes - cannot be reported if done by destroying objects (cannot throw from dtor) - so Close () assures
         *  a clean shutdown with exceptions being propagated during the cleanup.
         *
         *  \note Most calls on an OutputStream after it is closed are illegal, and result in Require () errors. It is up
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
         *  \note When a subtype stream (like BufferedOutputStream) aggregates another stream, it is left to that subclass
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
         *  \brief forces any data contained in this stream to be written.
         *
         *  Forces any data contained in this stream to be written.
         *
         *  For some streams (such as buffered streams, sockets, and OpenSSLCryptStream) - they may not
         *  finish their writes until they are destroyed. The trouble then - is that they cannot
         *  propagate exceptions! Calling Flush() before destroying the output stream allows exceptions
         *  to be propagated properly.
         *
         *  \req IsOpen ()
         */
        nonvirtual void Flush () const;

    public:
        /**
         * EXPERIEMNTAL API
         * done as template so third parties can externally extend, and have overloading work right..
         * @todo need overloads for basic types, std::string, int, float, etc...
         * But don't do except for string for now. Don't make same mistake as iostream - with formatting. Not clear how todo
         * right so don't dig a hole and do it wrong (yet).
         *
         *  \req IsOpen ()
         */
        template <typename T>
        const typename OutputStream::Ptr<ELEMENT_TYPE>& operator<< (const T& write2TextStream) const
            requires (is_same_v<ELEMENT_TYPE, Characters::Character>);

    public:
        /**
         *  \brief protected access to underlying stream smart pointer
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
        [[deprecated ("Since Strokka v3.0d5 deprecated since not widely used and very specific purpose and directly implementingable given "
                      "apis")]] SeekOffsetType
        GetOffsetToEndOfStream () const
        {
            SeekOffsetType savedReadFrom = GetOffset ();
            SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
            Seek (Whence::eFromStart, savedReadFrom);
            size -= savedReadFrom;
            return size;
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload of Write")]] void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) const
        {
            Write (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload")]] void Write (const uint8_t* start, const uint8_t* end) const
            requires (is_same_v<ELEMENT_TYPE, byte>)
        {
            this->Write (span{start, end});
        }
        template <typename POD_TYPE>
        [[deprecated ("Since Stroika v3.0d5 use span overload of WriteRaw")]] void WriteRaw (const POD_TYPE* start, const POD_TYPE* end) const
            requires (is_same_v<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
        {
            WriteRaw (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d5 use span overload of Write")]] void Write (const wchar_t* start, const wchar_t* end) const
            requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
        {
            this->Write (span{start, end});
        }
    };

    /**
     *  \brief Abstract interface for output stream object. Don't call directly (use Ptr usually) - but use directly mostly to implement new output stream types.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype/a>
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
         *  May (but typically not) called before destruction. If called, \req no other write or seek etc operations.
         *
         *  \note - 'Require (IsOpen()) automatically checked in Ptr wrappers for things like Write, so subclassers don't need to
         *          do that in implementing reps, but probably good docs/style todo in both places.'
         *
         *  \note -  CloseWrite must implicitly Flush ()
         */
        virtual void CloseWrite () = 0;

    public:
        /**
         *  return true iff CloseWrite () has not been called (cannot construct closed stream)
         */
        virtual bool IsOpenWrite () const = 0;

    public:
        virtual SeekOffsetType GetWriteOffset () const = 0;

    public:
        virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) = 0;

    public:
        /**
         *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
         *  BufSize must always be >= 1.
         *  Writes always succeed fully or throw (no partial writes so no return value of amount written).
         *
         *  \note The meaning of Write () depends on the exact type of Stream you are referencing. The data
         *        may still be buffered. Call @Flush () to ensure it pushed out.
         * 
         *  \note This can block indefinitely (for example in writing to a UNIX Pipe) - when output buffers fill.
         */
        virtual void Write (span<const ELEMENT_TYPE> elts) = 0;

    public:
        /**
         *
         */
        virtual void Flush () = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "OutputStream.inl"

#endif /*_Stroika_Foundation_Streams_OutputStream_h_*/
