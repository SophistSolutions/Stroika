/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_StreamReader_inl_
#define _Stroika_Foundation_Streams_StreamReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Traversal/Range.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ******************** Streams::StreamReader::CacheBlock_ ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline size_t StreamReader<ELEMENT_TYPE>::CacheBlock_::GetSize () const
    {
        return fCacheWindowBuf_.GetSize ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType StreamReader<ELEMENT_TYPE>::CacheBlock_::GetStart () const
    {
        return fCacheWindowBufStart_;
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType StreamReader<ELEMENT_TYPE>::CacheBlock_::GetEnd () const
    {
        return fCacheWindowBufStart_ + fCacheWindowBuf_.GetSize ();
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::CacheBlock_::Peek1FromCache (SeekOffsetType actualOffset) -> optional<ElementType>
    {
        size_t cacheWindowSize = fCacheWindowBuf_.size ();
        if (fCacheWindowBufStart_ <= actualOffset and actualOffset < fCacheWindowBufStart_ + cacheWindowSize) [[likely]] {
            return fCacheWindowBuf_[static_cast<size_t> (actualOffset - fCacheWindowBufStart_)];
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::CacheBlock_::Read1FromCache (SeekOffsetType* actualOffset) -> optional<ElementType>
    {
        RequireNotNull (actualOffset);
        auto result = Peek1FromCache (*actualOffset);
        if (result) [[likely]] {
            ++(*actualOffset);
        }
        return result;
    }
    template <typename ELEMENT_TYPE>
    optional<size_t> StreamReader<ELEMENT_TYPE>::CacheBlock_::ReadFromCache (SeekOffsetType* actualOffset, ElementType* intoStart, ElementType* intoEnd)
    {
        using namespace Traversal;
        size_t cacheWindowSize = fCacheWindowBuf_.size ();
        if (cacheWindowSize != 0) [[likely]] {
            Range<SignedSeekOffsetType> cacheWindow{
                static_cast<SignedSeekOffsetType> (fCacheWindowBufStart_),
                static_cast<SignedSeekOffsetType> (fCacheWindowBufStart_ + cacheWindowSize),
                Openness::eClosed,
                Openness::eOpen,
            };
            if (cacheWindow.Contains (*actualOffset)) [[likely]] {
                // then we can return at least some data from the cache - do that now
                size_t nToRead = intoEnd - intoStart;
                if (nToRead != 1) {
                    size_t nInBufAvail = static_cast<size_t> (cacheWindow.GetUpperBound () - *actualOffset);
                    nToRead            = min (nToRead, nInBufAvail);
                }
                Assert (nToRead > 0); // because contained _fOffset
                size_t curSeekPosOffsetIntoCache = static_cast<size_t> (*actualOffset - cacheWindow.GetLowerBound ());
                Assert (0 <= curSeekPosOffsetIntoCache and curSeekPosOffsetIntoCache < fCacheWindowBuf_.size ());
                std::copy (fCacheWindowBuf_.data () + curSeekPosOffsetIntoCache, fCacheWindowBuf_.data () + curSeekPosOffsetIntoCache + nToRead, intoStart);
                *actualOffset += nToRead;
                return nToRead;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    void StreamReader<ELEMENT_TYPE>::CacheBlock_::FillCacheWith (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd)
    {
        // adjust so smarter to not make cache too big...
        size_t         oldCacheSize = fCacheWindowBuf_.GetSize ();
        SeekOffsetType currentEnd   = fCacheWindowBufStart_ + oldCacheSize;
        size_t         nToWrite     = intoEnd - intoStart;
        Require (nToWrite > 0);
        if (currentEnd == s) {
            // extend the cache

            // resize_uninitialized showed up alot in windows profile running 'large-xxx' so figured
            // if we are going to actually allocate memory anyhow, then do it once, by grabbing largest chunk we
            // (are ever likely to) ask for.
            if (oldCacheSize + nToWrite > fCacheWindowBuf_.kMinCapacity) {
                fCacheWindowBuf_.reserve (kMaxBufferedChunkSize_);
            }
            fCacheWindowBuf_.resize_uninitialized (oldCacheSize + nToWrite);
            std::copy (intoStart, intoEnd, fCacheWindowBuf_.begin () + oldCacheSize);
        }
        else {
            fCacheWindowBuf_.resize_uninitialized (nToWrite); // CAN shrink
            fCacheWindowBufStart_ = s;
            std::copy (intoStart, intoEnd, fCacheWindowBuf_.begin ());
        }
    }

    /*
     ********************************************************************************
     *************************** Streams::StreamReader ******************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    StreamReader<ELEMENT_TYPE>::StreamReader (const typename Streams::InputStream<ElementType>::Ptr& underlyingReadFromStreamAdopted)
        : fStrm_{underlyingReadFromStreamAdopted}
        , fOffset_{underlyingReadFromStreamAdopted.GetOffset ()}
    {
    }
    template <typename ELEMENT_TYPE>
    StreamReader<ELEMENT_TYPE>::~StreamReader ()
    {
        // @todo perhaps CTOR flag to control if we do this or not
        IgnoreExceptionsForCall (this->SyncrhonizeToUnderlyingStream ())
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Read () -> optional<ElementType>
    {
        if (auto p = Read1FromCache_ ()) [[likely]] { // usually will get hit - else default to standard algorithm
            return p;
        }
        ElementType b; // intentionally not initialized, since will be filled in by Read_Slow_Case_ or unused
        return (Read_Slow_Case_ (&b, &b + 1) == 0) ? optional<ElementType>{} : b;
    }
    template <typename ELEMENT_TYPE>
    inline size_t StreamReader<ELEMENT_TYPE>::Read (ElementType* intoStart, ElementType* intoEnd)
    {
        // if already cached, return from cache. Note - even if only one element is in the Cache, thats enough to return
        // and not say 'eof'
        if (optional<size_t> o = ReadFromCache_ (intoStart, intoEnd)) {
            return *o;
        }
        return Read_Slow_Case_ (intoStart, intoEnd);
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Peek () -> optional<ElementType>
    {
        if (auto p = Peek1FromCache_ ()) [[likely]] { // usually will get hit - else default to standard algorithm
            return p;
        }
        SeekOffsetType saved  = fOffset_;
        auto           result = this->Read ();
        fOffset_              = saved;
        return result;
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType StreamReader<ELEMENT_TYPE>::GetOffset () const
    {
        return fOffset_;
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType StreamReader<ELEMENT_TYPE>::Seek (SeekOffsetType offset)
    {
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        return Seek (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType StreamReader<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset)
    {
        switch (whence) {
            case Whence::eFromCurrent:
                fOffset_ += offset;
                break;
            case Whence::eFromStart:
                fOffset_ = offset;
                break;
            case Whence::eFromEnd:
                fStrm_.Seek (Whence::eFromEnd, offset);
                fOffset_ = fStrm_.GetOffset ();
                break;
        }
        return fOffset_;
    }
    template <typename ELEMENT_TYPE>
    size_t StreamReader<ELEMENT_TYPE>::ReadAll (ElementType* intoStart, ElementType* intoEnd)
    {
        size_t elementsRead{};
        for (ElementType* readCursor = intoStart; readCursor < intoEnd;) {
            size_t eltsReadThisTime = Read (readCursor, intoEnd);
            Assert (eltsReadThisTime <= static_cast<size_t> (intoEnd - readCursor));
            if (eltsReadThisTime == 0) {
                // irrevocable EOF
                break;
            }
            elementsRead += eltsReadThisTime;
            readCursor += eltsReadThisTime;
        }
        return elementsRead;
    }
    template <typename ELEMENT_TYPE>
    inline void StreamReader<ELEMENT_TYPE>::SyncrhonizeToUnderlyingStream ()
    {
        fStrm_.Seek (GetOffset ());
    }
    template <typename ELEMENT_TYPE>
    inline void StreamReader<ELEMENT_TYPE>::SyncrhonizeFromUnderlyingStream ()
    {
        fOffset_ = fStrm_.GetOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline bool StreamReader<ELEMENT_TYPE>::IsAtEOF ()
    {
        if (fOffset_ < fFarthestReadInUnderlyingStream_) [[likely]] {
            return false; // not logically needed, but optimization
        }
        return not Peek ().has_value ();
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Peek1FromCache_ () -> optional<ElementType>
    {
        // first try last filled - generally will be the right one
        for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
            if (auto r = fCacheBlocks_[i].Peek1FromCache (this->fOffset_)) [[likely]] {
                return r;
            }
        }
        for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
            if (auto r = fCacheBlocks_[i].Peek1FromCache (this->fOffset_)) {
                return r;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Read1FromCache_ () -> optional<ElementType>
    {
        // first try last filled - generally will be the right one
        for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
            if (auto r = fCacheBlocks_[i].Read1FromCache (&this->fOffset_)) [[likely]] {
                return r;
            }
        }
        for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
            if (auto r = fCacheBlocks_[i].Read1FromCache (&this->fOffset_)) {
                return r;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    optional<size_t> StreamReader<ELEMENT_TYPE>::ReadFromCache_ (ElementType* intoStart, ElementType* intoEnd)
    {
        // first try last filled - generally will be the right one
        for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
            if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, intoStart, intoEnd)) {
                return r;
            }
        }
        for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
            if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, intoStart, intoEnd)) {
                return r;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    void StreamReader<ELEMENT_TYPE>::FillCacheWith_ (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd)
    {
        // pingpong buffers
        // try not to overfill any one cache block, but if the amount being read will fit, append to the current cache block
        // dont start a new one
        size_t thisFillSize = intoEnd - intoStart;
        if (fCacheBlocks_[fCacheBlockLastFilled_].GetEnd () != this->fOffset_ or fCacheBlocks_[fCacheBlockLastFilled_].GetSize () + thisFillSize > kMaxBufferedChunkSize_) {
            ++fCacheBlockLastFilled_;
            if (fCacheBlockLastFilled_ >= Memory::NEltsOf (fCacheBlocks_)) {
                fCacheBlockLastFilled_ = 0;
            }
        }
        fCacheBlocks_[fCacheBlockLastFilled_].FillCacheWith (s, intoStart, intoEnd);
    }
    template <typename ELEMENT_TYPE>
    inline void StreamReader<ELEMENT_TYPE>::FillCacheWith_ (SeekOffsetType s, const ElementType* intoStart, const ElementType* intoEnd)
    {
        FillCacheWith_ (s, reinterpret_cast<const InlineBufferElementType_*> (intoStart), reinterpret_cast<const InlineBufferElementType_*> (intoEnd));
    }
    template <typename ELEMENT_TYPE>
    size_t StreamReader<ELEMENT_TYPE>::Read_Slow_Case_ (ElementType* intoStart, ElementType* intoEnd)
    {
        ElementType buf[kDefaultReadBufferSize_];
        fStrm_.Seek (fOffset_); // check if getoffset not same in case not seekable) - or handle not seekable case
        size_t nRecordsRead = fStrm_.Read (begin (buf), end (buf));
        if (nRecordsRead == 0) {
            // not much point in caching - at eof
            return 0;
        }
        fFarthestReadInUnderlyingStream_ = max (fFarthestReadInUnderlyingStream_, fStrm_.GetOffset ());
        FillCacheWith_ (fOffset_, begin (buf), begin (buf) + nRecordsRead);
        return Memory::ValueOf (ReadFromCache_ (intoStart, intoEnd)); // we just cached bytes a the right offset so this must succeed
    }

}

#endif /*_Stroika_Foundation_Streams_StreamReader_inl_*/
