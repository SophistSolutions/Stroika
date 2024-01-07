/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ToSeekableInputStream_inl_
#define _Stroika_Foundation_Streams_ToSeekableInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "InputStreamDelegationHelper.h"

namespace Stroika::Foundation::Streams::ToSeekableInputStream {

    /*
     ********************************************************************************
     ****************** Streams::ToSeekableInputStream::New *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    auto New (const Ptr<ELEMENT_TYPE>& in) -> Ptr<ELEMENT_TYPE>
    {
        using Debug::AssertExternallySynchronizedMutex;
        struct seekableWrapper final : InputStreamDelegationHelper<ELEMENT_TYPE> {

            using inherited = InputStreamDelegationHelper<ELEMENT_TYPE>;
            seekableWrapper (const Ptr<ELEMENT_TYPE>& in)
                : inherited{in}
                , fOffset_{in.GetOffset ()}
                , fCacheBaseOffset_{fOffset_}
            {
                Assert (not this->fRealIn.IsSeekable ()); // just to document that's why we're here!
            }
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                SeekOffsetType cacheEnd = fCacheBaseOffset_ + fCachedData_.size ();
                if (fCacheBaseOffset_ <= fOffset_ and fOffset_ < cacheEnd) [[unlikely]] {
                    Ensure (cacheEnd - fOffset_ > 0);
                    return static_cast<size_t> (cacheEnd - fOffset_);
                }
                return this->fRealIn.AvailableToRead ();
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                auto baseRemaining = this->fRealIn.AvailableToRead ();
                if (baseRemaining) {
                    SeekOffsetType cacheEnd = fCacheBaseOffset_ + fCachedData_.size ();
                    Assert (fOffset_ <= cacheEnd);
                    baseRemaining = *baseRemaining + static_cast<size_t> (cacheEnd - fOffset_); // if we have some cached data past current seek offset, add it too
                }
                return baseRemaining;
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, [[maybe_unused]] NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                /*
                 *  See if the request can be serviced from the cached data. If so, do so.
                 */
                SeekOffsetType cacheEnd = fCacheBaseOffset_ + fCachedData_.size ();
                if (fCacheBaseOffset_ <= fOffset_ and fOffset_ < cacheEnd) [[unlikely]] {
                    size_t copyCnt = max<size_t> (static_cast<size_t> (cacheEnd - fOffset_), intoBuffer.size ());
                    auto r = Memory::CopySpanData (span{fCachedData_}.subspan (static_cast<size_t> (fOffset_ - fCacheBaseOffset_), copyCnt), intoBuffer);
                    fOffset_ += copyCnt;
                    return r;
                }
                /*
                 *  If it cannot, accumulate any read data into the cache so it can be re-read.
                 */
                Assert (fOffset_ == inherited::fRealIn.GetOffset ()); // could be bug with this code or somebody else playing fast and loose, but use assert
                auto r = this->fRealIn.Read (intoBuffer, blockFlag);
                // cache it, and update our data structures; note easy, cuz fRealIn must be at matching seek offset
                fCachedData_.push_back (r);
                fOffset_ += r.size ();
                return r;
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fOffset_;
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                static const auto                               kException_ = range_error{"seek"};
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        SeekOffsetType newOffset = static_cast<SeekOffsetType> (offset);
                        Require (newOffset >= fCacheBaseOffset_); // as documented in New() code that creates this, we cannot seek back past where we started from
                        SeekOffsetType cacheEnd = fCacheBaseOffset_ + fCachedData_.size ();
                        while (newOffset > cacheEnd) {
                            // we must read and buffer/accumulate in the cache; note - because of how this code works, the
                            // fRealIn is always seeked to the end cached data.
                            byte someBuf[1024];
                            auto r = this->fRealIn.Read (span{someBuf});
                            fCachedData_.push_back (r); // nb: an exception in this copy would cause fRealIn offset to be out of sync, but not sure what todo about it
                            cacheEnd = fCacheBaseOffset_ + fCachedData_.size ();
                        }
                        Assert (newOffset <= cacheEnd);
                        fOffset_ = newOffset;
                        return newOffset;
                    } break;
                    case Whence::eFromCurrent: {
                        return this->SeekRead (Whence::eFromStart, fOffset_ + offset);
                    } break;
                    case Whence::eFromEnd: {
                        if (auto remainingLength = this->RemainingLength ()) {
                            return this->SeekRead (Whence::eFromStart, fOffset_ + *remainingLength + offset);
                        }
                        else {
                            // implies seeking (fRealIn) to the end, and so reading everything, and then performing the desired seek
                            while (true) {
                                byte someBuf[8 * 1024];
                                auto r = this->fRealIn.Read (span{someBuf});
                                fCachedData_.push_back (r); // nb: an exception in this copy would cause fRealIn offset to be out of sync, but not sure what todo about it
                                if (r.empty ()) {
                                    break;
                                }
                            }
                            SeekOffsetType realEnd = fCacheBaseOffset_ + fCachedData_.size ();
                            Assert (realEnd == this->fRealIn.GetOffset ());
                            return this->SeekRead (Whence::eFromStart, realEnd + offset);
                        }
                    } break;
                    default:
                        RequireNotReached ();
                        return 0;
                }
            }

        private:
            Memory::InlineBuffer<ELEMENT_TYPE> fCachedData_;
            SeekOffsetType                     fOffset_{0}; // this rep's seek offset (as oppsed to that in fRealIn)
            SeekOffsetType                     fCacheBaseOffset_{0};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
        if (in.IsSeekable ()) {
            return in;
        }
        else {
            return Ptr<ELEMENT_TYPE>{make_shared<seekableWrapper> (in)};
        }
        return in;
    }

}
#endif /*_Stroika_Foundation_Streams_ToSeekableInputStream_inl_*/
