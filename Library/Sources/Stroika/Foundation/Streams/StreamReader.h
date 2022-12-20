/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
 *
 *  TODO:
 */

namespace Stroika::Foundation::Streams {

    /**
     *  \brief StreamReader is an unnecessary class for using the Streams library, but it is easy to use, similar to InputStream<T>::Ptr, and significantly more performant
     */
    template <typename ELEMENT_TYPE>
    struct StreamReader {
    public:
        /**
         */
        using ElementType = ELEMENT_TYPE;

    public:
        /**
         *  \note Do NOT use the InputStream<ElementType>::Ptr passed in at the same time as its being used by the
         *        StreamReader, or grave disorder may result. StreamReader assumes its the only one seeking and reading
         *        through the input stream.
         * 
         *  \note At destruction, StreamReader automatically calls SyncrhonizeToUnderlyingStream
         */
        StreamReader (const typename InputStream<ElementType>::Ptr& underlyingReadFromStreamAdopted);
        StreamReader ()                    = delete;
        StreamReader (const StreamReader&) = delete;

    public:
        /**
         */
        ~StreamReader ();

    public:
        /**
         *  \brief Logically the same as InputStream<ELEMENT_TYPE>::Ptr::Read () but reading cached data
         */
        nonvirtual optional<ElementType> Read ();
        nonvirtual size_t                Read (ElementType* intoStart, ElementType* intoEnd);

    public:
        /**
         *  \brief Logically the same as InputStream<ELEMENT_TYPE>::Ptr::Peek () but reading cached data
         */
        nonvirtual optional<ElementType> Peek ();

    public:
        /**
         *  \brief Logically the same as InputStream<ELEMENT_TYPE>::Ptr::GetOffset () - but without being 'synchronized' it maybe a different value than the underlying stream
         */
        nonvirtual SeekOffsetType GetOffset () const;

    public:
        /**
         *  \brief Logically the same as InputStream<ELEMENT_TYPE>::Ptr::Seek () - but without being 'synchronized' it maybe a different value than the underlying stream
         */
        nonvirtual SeekOffsetType Seek (SeekOffsetType offset);
        nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset);

    public:
        /**
         *  \brief Logically the same as InputStream<ELEMENT_TYPE>::Ptr::ReadAll ()
         */
        nonvirtual size_t ReadAll (ElementType* intoStart, ElementType* intoEnd);

    public:
        /**
         *  If you must use the underlying stream along-side StreamReader, you can use
         *  SyncrhonizeToUnderlyingStream and SyncrhonizeFromUnderlyingStream to allow each class to update
         *  each other.
         */
        nonvirtual void SyncrhonizeToUnderlyingStream ();

    public:
        /**
         *  If you must use the underlying stream along-side StreamReader, you can use
         *  SyncrhonizeToUnderlyingStream and SyncrhonizeFromUnderlyingStream to allow each class to update
         *  each other.
         */
        nonvirtual void SyncrhonizeFromUnderlyingStream ();

    public:
        nonvirtual bool IsAtEOF ();

    private:
        // may want to tune these; but I did a little tuning on Windows --LGP 2022-12-17
        static constexpr size_t kDefaultReadBufferSize_ = 8 * 1024;
        static constexpr size_t kMaxBufferedChunkSize_  = 64 * 1024;
        static constexpr size_t kCountPingPingBufs_     = 2;

    private:
        // Hack to allow use of inline buffer and uninitialized array even thoug Character class not 'trivial', it probably should be (or I'm not checking the right trait)
        using InlineBufferElementType_ = conditional_t<is_same_v<Characters::Character, ElementType>, wchar_t, ElementType>;

    private:
        struct CacheBlock_ {
            CacheBlock_ () = default;
            size_t                GetSize () const;
            SeekOffsetType        GetStart () const;
            SeekOffsetType        GetEnd () const;
            optional<ElementType> Peek1FromCache (const SeekOffsetType* actualOffset);
            optional<ElementType> Read1FromCache (SeekOffsetType* actualOffset);
            optional<size_t>      ReadFromCache (SeekOffsetType* actualOffset, ElementType* intoStart, ElementType* intoEnd);
            void                  FillCacheWith (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd);

        private:
            // Cache uses wchar_t instead of Character so can use resize_uninitialized () - requires is_trivially_constructible
            SeekOffsetType                                 fCacheWindowBufStart_{0}; // buffer a 'window' around the current data. Not infinite the whole file, but can be pretty big
            Memory::InlineBuffer<InlineBufferElementType_> fCacheWindowBuf_;
        };

    private:
        typename InputStream<ElementType>::Ptr fStrm_;
        SeekOffsetType                         fOffset_{0};
        SeekOffsetType                         fFarthestReadInUnderlyingStream_{0};
        CacheBlock_                            fCacheBlocks_[kCountPingPingBufs_];
        size_t                                 fCacheBlockLastFilled_{0};

    private:
        nonvirtual optional<ElementType> Peek1FromCache_ ();
        nonvirtual optional<ElementType> Read1FromCache_ ();
        nonvirtual optional<size_t> ReadFromCache_ (ElementType* intoStart, ElementType* intoEnd);
        nonvirtual void             FillCacheWith_ (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd);
        nonvirtual void             FillCacheWith_ (SeekOffsetType s, const ElementType* intoStart, const ElementType* intoEnd);
        size_t                      Read_Slow_Case_ (ElementType* intoStart, ElementType* intoEnd);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StreamReader.inl"

#endif /*_Stroika_Foundation_Streams_StreamReader_h_*/