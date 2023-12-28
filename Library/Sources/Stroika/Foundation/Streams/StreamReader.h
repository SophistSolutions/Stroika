/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_StreamReader_h_
#define _Stroika_Foundation_Streams_StreamReader_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Streams {

    /**
     *  \brief StreamReader is an non-essential Stream utility, adding simplicity of use for a common use case, and a significant performance boost.
     * 
     * \see also https://learn.microsoft.com/en-us/dotnet/api/system.io.streamreader?view=net-8.0 - similar idea, except there you specify conversion from
     *      binary stream.
     *
     *  StreamReader is an unnecessary class for using the Streams library, but it is easy to use, similar to InputStream<T>::Ptr, and significantly more performant
     * 
     *  TODO:
     *      \todo consider if should take templated parameter indicating buffer size
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
         *  \req underlyingReadFromStreamAdopted.Seekable ();
         */
        StreamReader (const typename InputStream::Ptr<ElementType>& underlyingReadFromStreamAdopted);
        StreamReader ()                    = delete;
        StreamReader (const StreamReader&) = delete;

    public:
        /**
         */
        ~StreamReader ();

    public:
        /**
         *  \brief Logically the same as InputStream::Ptr<ELEMENT_TYPE>::Read () but reading cached data
         */
        nonvirtual optional<ElementType> Read ();
        nonvirtual span<ElementType> Read (span<ElementType> intoBuffer);

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
         */
        nonvirtual size_t ReadAll (ElementType* intoStart, ElementType* intoEnd);

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
        nonvirtual bool IsAtEOF ();

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
        // Hack to allow use of inline buffer and uninitialized array even thoug Character class not 'trivial', it probably should be (or I'm not checking the right trait)
        using InlineBufferElementType_ = conditional_t<is_same_v<Characters::Character, ElementType>, char32_t, ElementType>;
        static_assert (sizeof (ElementType) == sizeof (InlineBufferElementType_));

    private:
        struct CacheBlock_ {
            CacheBlock_ () = default;
            size_t                GetSize () const;
            SeekOffsetType        GetStart () const;
            SeekOffsetType        GetEnd () const;
            optional<ElementType> Peek1FromCache (SeekOffsetType actualOffset);
            optional<ElementType> Read1FromCache (SeekOffsetType* actualOffset);
            optional<size_t>      ReadFromCache (SeekOffsetType* actualOffset, ElementType* intoStart, ElementType* intoEnd);
            void FillCacheWith (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd);

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
        nonvirtual optional<ElementType> Peek1FromCache_ ();
        nonvirtual optional<ElementType> Read1FromCache_ ();
        nonvirtual optional<size_t> ReadFromCache_ (ElementType* intoStart, ElementType* intoEnd);
        nonvirtual void FillCacheWith_ (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd);
        nonvirtual void FillCacheWith_ (SeekOffsetType s, const ElementType* intoStart, const ElementType* intoEnd);
        size_t          Read_Slow_Case_ (ElementType* intoStart, ElementType* intoEnd);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StreamReader.inl"

#endif /*_Stroika_Foundation_Streams_StreamReader_h_*/
