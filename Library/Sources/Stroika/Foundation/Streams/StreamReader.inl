/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Traversal/Range.h"

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
    inline auto StreamReader<ELEMENT_TYPE>::CacheBlock_::Peek1FromCache (SeekOffsetType actualOffset) const -> optional<ElementType>
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
    optional<size_t> StreamReader<ELEMENT_TYPE>::CacheBlock_::ReadFromCache (SeekOffsetType* actualOffset, span<ElementType> into)
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
                size_t nToRead = into.size ();
                if (nToRead != 1) {
                    size_t nInBufAvail = static_cast<size_t> (cacheWindow.GetUpperBound () - *actualOffset);
                    nToRead            = min (nToRead, nInBufAvail);
                }
                Assert (nToRead > 0); // because contained _fOffset
                size_t curSeekPosOffsetIntoCache = static_cast<size_t> (*actualOffset - cacheWindow.GetLowerBound ());
                Assert (0 <= curSeekPosOffsetIntoCache and curSeekPosOffsetIntoCache < fCacheWindowBuf_.size ());
                std::copy (fCacheWindowBuf_.data () + curSeekPosOffsetIntoCache,
                           fCacheWindowBuf_.data () + curSeekPosOffsetIntoCache + nToRead, into.data ());
                *actualOffset += nToRead;
                return nToRead;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    void StreamReader<ELEMENT_TYPE>::CacheBlock_::FillCacheWith (SeekOffsetType s, span<InlineBufferElementType_> into)
    {
        // adjust so smarter to not make cache too big...
        size_t         oldCacheSize = fCacheWindowBuf_.GetSize ();
        SeekOffsetType currentEnd   = fCacheWindowBufStart_ + oldCacheSize;
        size_t         nToWrite     = into.size ();
        Require (nToWrite > 0);
        if (currentEnd == s) {
            // extend the cache

            // resize_uninitialized showed up a lot in windows profile running 'large-xxx' so figured
            // if we are going to actually allocate memory anyhow, then do it once, by grabbing largest chunk we
            // (are ever likely to) ask for.
            if (oldCacheSize + nToWrite > fCacheWindowBuf_.kMinCapacity) {
                fCacheWindowBuf_.reserve (kMaxBufferedChunkSize_);
            }
            fCacheWindowBuf_.resize_uninitialized (oldCacheSize + nToWrite);
            std::copy (into.begin (), into.end (), fCacheWindowBuf_.begin () + oldCacheSize);
        }
        else {
            fCacheWindowBuf_.resize_uninitialized (nToWrite); // CAN shrink
            fCacheWindowBufStart_ = s;
            std::copy (into.begin (), into.end (), fCacheWindowBuf_.begin ());
        }
    }

    /*
     ********************************************************************************
     *************************** Streams::StreamReader ******************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    StreamReader<ELEMENT_TYPE>::StreamReader (const typename Streams::InputStream::Ptr<ElementType>& underlyingReadFromStreamAdopted)
        : fStrm_{underlyingReadFromStreamAdopted}
        , fOffset_{underlyingReadFromStreamAdopted.GetOffset ()}
    {
        Require (underlyingReadFromStreamAdopted.IsSeekable ());
    }
    template <typename ELEMENT_TYPE>
    StreamReader<ELEMENT_TYPE>::~StreamReader ()
    {
        // @todo perhaps CTOR flag to control if we do this or not
        IgnoreExceptionsForCall (this->SynchronizeToUnderlyingStream ())
    }
    template <typename ELEMENT_TYPE>
    inline optional<span<ELEMENT_TYPE>> StreamReader<ELEMENT_TYPE>::Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag)
    {
        Require (not intoBuffer.empty ());
        // if already cached, return from cache. Note - even if only one element is in the Cache, that's enough to return
        // and not say 'eof'
        if (optional<size_t> o = ReadFromCache_ (intoBuffer)) {
            return intoBuffer.subspan (0, *o);
        }
        if (auto osz = Read_Slow_Case_ (intoBuffer, blockFlag)) {
            return intoBuffer.subspan (0, *osz);
        }
        Assert (blockFlag == NoDataAvailableHandling::eDontBlock); // Read_Slow_Case_ only returns nullopt in this case - if no data available
        return nullopt;                                            //
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::ReadBlocking () -> optional<ElementType>
    {
        ElementType       e{};
        span<ElementType> r = ReadBlocking (span{&e, 1});
        return r.empty () ? optional<ElementType>{} : e;
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::ReadBlocking (span<ElementType> intoBuffer) -> span<ElementType>
    {
        return Memory::ValueOf (Read (intoBuffer, NoDataAvailableHandling::eBlockIfNoDataAvailable));
    }
    template <typename ELEMENT_TYPE>
    auto StreamReader<ELEMENT_TYPE>::ReadBlocking (Memory::InlineBuffer<ElementType>* intoBuffer, ElementType upToSentinel) -> span<ElementType>
    {
        Require (intoBuffer->size () == 0);
        while (auto oe = ReadBlocking ()) {
            intoBuffer->push_back (*oe); // include the sentinel
            if (*oe == upToSentinel) {
                return span{intoBuffer->data (), intoBuffer->size () - 1}; // dont include the sentinel
            }
        }
        return span{intoBuffer->data (), intoBuffer->size ()};
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::ReadNonBlocking (span<ElementType> intoBuffer) -> optional<span<ElementType>>
    {
        // Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        //        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        Require (not intoBuffer.empty ());
        return Read (intoBuffer, NoDataAvailableHandling::eDontBlock);
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::ReadOrThrow (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) -> span<ElementType>
    {
        if (auto o = Read (intoBuffer, blockFlag)) [[likely]] {
            return *o;
        }
        Execution::Throw (EWouldBlock::kThe);
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Peek () -> optional<ElementType>
    {
        if (auto p = Peek1FromCache_ ()) [[likely]] { // usually will get hit - else default to standard algorithm
            return p;
        }
        SeekOffsetType saved  = fOffset_;
        auto           result = this->ReadBlocking ();
        fOffset_              = saved;
        return result;
    }
    template <typename ELEMENT_TYPE>
    inline auto StreamReader<ELEMENT_TYPE>::Peek (span<ElementType> intoBuffer) -> span<ElementType>
    {
        // @todo maybe able to better optimize this with peeks, and avoid seek
        SeekOffsetType saved  = fOffset_;
        auto           result = this->Read (intoBuffer);
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
            case eFromCurrent:
                fOffset_ += offset;
                break;
            case eFromStart:
                fOffset_ = offset;
                break;
            case eFromEnd:
                fStrm_.Seek (eFromEnd, offset);
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
            size_t eltsReadThisTime = ReadBlocking (span{readCursor, intoEnd}).size ();
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
    inline optional<size_t> StreamReader<ELEMENT_TYPE>::AvailableToRead () const
    {
        if (fFarthestReadInUnderlyingStream_ > fOffset_) {
            return static_cast<size_t> (fFarthestReadInUnderlyingStream_ - fOffset_);
        }
        return fStrm_.AvailableToRead ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<SeekOffsetType> StreamReader<ELEMENT_TYPE>::RemainingLength () const
    {
        if (auto underlyingRemaining = fStrm_.RemainingLength ()) {
            Assert (fOffset_ <= fFarthestReadInUnderlyingStream_);
            return *underlyingRemaining + (fFarthestReadInUnderlyingStream_ - fOffset_);
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    inline void StreamReader<ELEMENT_TYPE>::SynchronizeToUnderlyingStream ()
    {
        fStrm_.Seek (GetOffset ());
    }
    template <typename ELEMENT_TYPE>
    inline void StreamReader<ELEMENT_TYPE>::SynchronizeFromUnderlyingStream ()
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
    inline auto StreamReader<ELEMENT_TYPE>::Peek1FromCache_ () const -> optional<ElementType>
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
    optional<size_t> StreamReader<ELEMENT_TYPE>::ReadFromCache_ (span<ElementType> into)
    {
        // first try last filled - generally will be the right one
        for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
            if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, into)) {
                return r;
            }
        }
        for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
            if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, into)) {
                return r;
            }
        }
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    void StreamReader<ELEMENT_TYPE>::FillCacheWith_ (SeekOffsetType s, span<InlineBufferElementType_> into)
    {
        // pingpong buffers
        // try not to overfill any one cache block, but if the amount being read will fit, append to the current cache block
        // dont start a new one
        size_t thisFillSize = into.size ();
        if (fCacheBlocks_[fCacheBlockLastFilled_].GetEnd () != this->fOffset_ or
            fCacheBlocks_[fCacheBlockLastFilled_].GetSize () + thisFillSize > kMaxBufferedChunkSize_) {
            ++fCacheBlockLastFilled_;
            if (fCacheBlockLastFilled_ >= Memory::NEltsOf (fCacheBlocks_)) {
                fCacheBlockLastFilled_ = 0;
            }
        }
        fCacheBlocks_[fCacheBlockLastFilled_].FillCacheWith (s, into);
    }
    template <typename ELEMENT_TYPE>
    optional<size_t> StreamReader<ELEMENT_TYPE>::Read_Slow_Case_ (span<ElementType> into, NoDataAvailableHandling blockFlag)
    {
        ElementType buf[kDefaultReadBufferSize_];
        fStrm_.Seek (fOffset_); // check if get_offset not same in case not seekable) - or handle not seekable case
        if (optional<span<ElementType>> o = fStrm_.Read (buf, blockFlag)) {
            size_t nRecordsRead = o->size ();
            if (nRecordsRead == 0) {
                // not much point in caching - at eof
                return 0;
            }
            fFarthestReadInUnderlyingStream_ = max (fFarthestReadInUnderlyingStream_, fStrm_.GetOffset ());
            FillCacheWith_ (fOffset_, Memory::SpanBytesCast<span<InlineBufferElementType_>> (span{buf, nRecordsRead}));
            return Memory::ValueOf (ReadFromCache_ (into)); // we just cached bytes a the right offset so this must succeed
        }
        // if upstream read returned nullopt, implies would-block
        Assert (blockFlag == NoDataAvailableHandling::eDontBlock);
        return nullopt;
    }

}
