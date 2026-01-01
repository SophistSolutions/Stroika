/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_StreamReader_h_
#define _Stroika_Foundation_Streams_StreamReader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Streams {

    /**
     *  \brief StreamReader is an non-essential Stream utility, adding simplicity of use for a common use case, and a significant performance boost.
     * 
     * \see also https://learn.microsoft.com/en-us/dotnet/api/system.io.streamreader?view=net-8.0 - similar idea, except there you specify conversion from
     *      binary stream.
     *
     *  StreamReader is an unnecessary class for using the Streams library, but it is easy to use, has a similar API to InputStream<T>::Ptr, and significantly more performant.
     * 
     *  \note Stream reader is always fully seekable, and requires its argument (construction time) to be fully seekable as well.
     * 
     *  \note Similar to BufferedInputStream - but that provides an actual Stream object, and is slightly less performant for direct use.
     * 
     *  TODO:
     *      \todo consider if should take templated parameter indicating buffer size (really template parameter StreamReaderDefaultOptions - with various settings)
     */
    template <typename ELEMENT_TYPE>
    struct StreamReader {
    public:
        /**
         */
        using ElementType = ELEMENT_TYPE;

    public:
        /**
         *  \note Do NOT use the InputStream::Ptr<ElementType> passed in at the same time as its being used by the
         *        StreamReader, or grave disorder may result. StreamReader assumes its the only one seeking and reading
         *        through the input stream. See SynchronizeToUnderlyingStream ().
         * 
         *  \note At destruction, StreamReader automatically calls SynchronizeToUnderlyingStream
         * 
         *  \pre underlyingReadFromStreamAdopted.Seekable ();       // so it can read ahead and synchronize back in dtor
         * 
         *  \note a future version of StreamReader might allow underlyingReadFromStreamAdopted.Seekable () and just pre-read to allow seeking, but then
         *        'synchronize' feature wont work
         */
        StreamReader (const typename InputStream::Ptr<ElementType>& underlyingReadFromStreamAdopted);
        StreamReader ()                    = delete;
        StreamReader (const StreamReader&) = delete;

    public:
        /**
         *  \note this does IgnoreExceptionsForCall (this->SynchronizeToUnderlyingStream ())
         */
        ~StreamReader ();

    public:
        /**
         * \brief Read into data referenced by span argument - and using argument blocking strategy (default blocking)
         * 
         *  returns nullopt ONLY if blockFlag = eNonBlocking AND there is NO data available without blocking.
         * 
         *  a return of NOT missing, but an empty span implies EOF.
         * 
         * BLOCKING until data is available, but can return with fewer elements than argument span-size
         * without prejudice about how much more is available.
         *
         *  \note It is legal to call Read () if its already returned EOF, but then it MUST return EOF again.
         *
         *  \pre not intoBuffer.empty ()
         *
         *  @see ReadAll () to read all the data from the stream at once.
         *
         *  @see ReadBlocking ()    - often simplest to use API
         *  @see ReadOrThrow ()
         *  @see ReadNonBlocking ()
         * 
         *   \see also InputStream::Ptr::Read () - identical API (except non-const)
         */
        nonvirtual optional<span<ElementType>> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag);

    public:
        /**
         *  \brief ReadBlocking () reads either a single element, or fills in argument intoBuffer - but never blocks (nor throws EWouldBlock)
         * 
         *  ReadBlocking ():
         *      Reads a single element (blocking as long as needed) - or nullopt when at EOF.
         * 
         *  ReadBlocking(span<ElementType> intoBuffer) 
         *      fills in subspan with at least one element (or zero if at EOF)
         * 
         *  \see also InputStream::Ptr::ReadBlocking () - identical API (except non-const)
         */
        nonvirtual optional<ElementType> ReadBlocking ();
        nonvirtual span<ElementType> ReadBlocking (span<ElementType> intoBuffer);
        nonvirtual span<ElementType> ReadBlocking (Memory::InlineBuffer<ElementType>* intoBuffer, ElementType upToSentinel);

    public:
        /**
         * \brief read into intoBuffer - returning nullopt if would block, and else returning subspan of input with read data present
         * 
         *  same as Read (intoBuffer, NoDataAvailableHandling::eDontBlock)
         * 
         *     \see also InputStream::Ptr::ReadBlocking () - identical API(except non-const)
         */
        nonvirtual optional<span<ElementType>> ReadNonBlocking (span<ElementType> intoBuffer);

    public:
        /**
         * \brief Read (either one or into argument span) and taking NoDataAvailableHandling blockFlag), and throw if would block
         *
         * same as Read() APIs, but instead of returning optional, for cases where nullopt would be returned, throw EWouldBlock
         * 
         * \note if blockFlag == eBlockIfNoDataAvailable, this amounts to *Read(...args), since it will never generate an
         *       eWouldBlock exception;
         * 
         *  when to use this variant? If implementing API where you are handed a blockFlag, but want to KISS, and
         *  just throw if EWouldBlock ..
         * 
         *     \see also InputStream::Ptr::ReadOrThrow () - identical API(except non-const)
         */
        nonvirtual span<ElementType> ReadOrThrow (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag);

    public:
        /**
         *  \brief Logically the same as InputStream::Ptr<ELEMENT_TYPE>::Peek () but reading cached data
         */
        nonvirtual optional<ElementType> Peek ();
        nonvirtual span<ElementType> Peek (span<ElementType> intoBuffer);

    public:
        /**
         *  \brief Logically the same as InputStream::Ptr<ELEMENT_TYPE>::GetOffset () - but without being 'synchronized' it maybe a different value than the underlying stream
         */
        nonvirtual SeekOffsetType GetOffset () const;

    public:
        /**
         *  \brief Logically the same as InputStream::Ptr<ELEMENT_TYPE>::Seek () - but without being 'synchronized' it maybe a different value than the underlying stream
         */
        nonvirtual SeekOffsetType Seek (SeekOffsetType offset);
        nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset);

    public:
        /**
         *  \brief Logically the same as InputStream::Ptr<ELEMENT_TYPE>::ReadAll ()
         * 
         *      @todo add other overloads from InputStream::Ptr::ReadAll() - ...
         */
        nonvirtual size_t ReadAll (ElementType* intoStart, ElementType* intoEnd);

    public:
        /**
         *  \brief returns nullopt if nothing known available, zero if known EOF, and any other number of elements (typically 1) if that number know to be available to read
         *
         *  \see also (different) RemainingLength ()
         */
        nonvirtual optional<size_t> AvailableToRead () const;

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
         *  If you must use the underlying stream along-side StreamReader, you can use
         *  SynchronizeToUnderlyingStream and SynchronizeFromUnderlyingStream to allow each class to update
         *  each other.
         */
        nonvirtual void SynchronizeToUnderlyingStream ();

    public:
        /**
         *  If you must use the underlying stream along-side StreamReader, you can use
         *  SynchronizeToUnderlyingStream and SynchronizeFromUnderlyingStream to allow each class to update
         *  each other.
         */
        nonvirtual void SynchronizeFromUnderlyingStream ();

    public:
        /**
         *  \brief check if the stream is currently at EOF
         *
         *  \note - IsAtEOF/0 may do a blocking Read () call.
         *  \note - IsAtEOF (eDontBlock) returns optional<bool> - nullopt if would block, and false if known not at EOF, and true if known EOF; 
         *          this differs from most Stroika streams APIs - in that nullopt here means 'EWouldBlock';
         */
        nonvirtual bool IsAtEOF ();
        nonvirtual optional<bool> IsAtEOF (NoDataAvailableHandling blockFlag);

    public:
        [[deprecated ("Since Stroika v3.0d5 use Read/1-span")]] size_t Read (ElementType* intoStart, ElementType* intoEnd)
        {
            return Read (span{intoStart, intoEnd}).size ();
        }

    private:
        // may want to tune these; but I did a little tuning on Windows --LGP 2022-12-17
        static constexpr size_t kDefaultReadBufferSize_ = 8 * 1024;
        static constexpr size_t kMaxBufferedChunkSize_  = 64 * 1024;
        // Note the reason for kCountPingPingBufs_ == 2: generally we get a hit with one which is best. You
        // typically read through the buffer one element after another. But its not rare to do put-backs of an
        // element or two. And if you do that - at the edges - you can get degenerate behavior - keep filling cache
        // going back one and forward one. The PingPong/2 value here prevents that degenerate case.
        static constexpr size_t kCountPingPingBufs_ = 2;

    private:
        // Hack to allow use of inline buffer and uninitialized array even though Character class not 'trivial', it probably should be (or I'm not checking the right trait)
        using InlineBufferElementType_ = conditional_t<same_as<Characters::Character, ElementType>, char32_t, ElementType>;
        static_assert (sizeof (ElementType) == sizeof (InlineBufferElementType_));

    private:
        struct CacheBlock_ {
            CacheBlock_ () = default;
            size_t                GetSize () const;
            SeekOffsetType        GetStart () const;
            SeekOffsetType        GetEnd () const;
            optional<ElementType> Peek1FromCache (SeekOffsetType actualOffset) const;
            optional<ElementType> Read1FromCache (SeekOffsetType* actualOffset);
            optional<size_t>      ReadFromCache (SeekOffsetType* actualOffset, span<ElementType> into);
            void                  FillCacheWith (SeekOffsetType s, span<InlineBufferElementType_> into);

        private:
            // Cache uses wchar_t instead of Character so can use resize_uninitialized () - requires is_trivially_constructible
            SeekOffsetType fCacheWindowBufStart_{0}; // buffer a 'window' around the current data. Not infinite the whole file, but can be pretty big
            Memory::InlineBuffer<InlineBufferElementType_> fCacheWindowBuf_;
        };

    private:
        typename InputStream::Ptr<ElementType> fStrm_;
        SeekOffsetType                         fOffset_{0};
        SeekOffsetType                         fFarthestReadInUnderlyingStream_{0};
        CacheBlock_                            fCacheBlocks_[kCountPingPingBufs_];
        size_t                                 fCacheBlockLastFilled_{0};

    private:
        nonvirtual optional<ElementType> Peek1FromCache_ () const;
        nonvirtual optional<ElementType> Read1FromCache_ ();
        nonvirtual optional<size_t> ReadFromCache_ (span<ElementType> into);
        nonvirtual void             FillCacheWith_ (SeekOffsetType s, span<InlineBufferElementType_> into);
        nonvirtual optional<size_t> Read_Slow_Case_ (span<ElementType> into, NoDataAvailableHandling blockFlag);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StreamReader.inl"

#endif /*_Stroika_Foundation_Streams_StreamReader_h_*/
