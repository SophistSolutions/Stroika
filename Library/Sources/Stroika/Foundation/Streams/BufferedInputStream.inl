/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputStream.h"
#include "Stroika/Foundation/Streams/StreamReader.h"

namespace Stroika::Foundation::Streams::BufferedInputStream {

    namespace Private_ {

        // this case easy, delegate to StreamReader to do all the work
        template <typename ELEMENT_TYPE>
        class Rep_Seekable_FromSeekable_ : public IRep_<ELEMENT_TYPE> {
        public:
            Rep_Seekable_FromSeekable_ (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
                : fRealIn_{realIn}
                , fReader_{realIn}
            {
                Require (realIn.IsSeekable ());
            }
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseRead () override
            {
                if (fRealIn_ != nullptr) {
                    fRealIn_.Close ();
                }
                Ensure (not IsOpenRead ());
                Assert (fRealIn_ == nullptr);
            }
            virtual bool IsOpenRead () const override
            {
                return fRealIn_ != nullptr;
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                return fReader_.GetOffset ();
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.AvailableToRead (); // since no actual buffering here yet
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.RemainingLength ();
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.Read (intoBuffer, blockFlag);
            }

        private:
            typename InputStream::Ptr<ELEMENT_TYPE>                        fRealIn_;
            StreamReader<ELEMENT_TYPE>                                     fReader_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };

        // read the source into one big buffer. Keep it all around, so seekable
        template <typename ELEMENT_TYPE, size_t INLINE_BUF_SIZE>
        class Rep_Seekable_FromUnSeekable_ : public IRep_<ELEMENT_TYPE> {
        public:
            Rep_Seekable_FromUnSeekable_ (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
                : fRealIn_{realIn}
            {
            }
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseRead () override
            {
                if (fRealIn_ != nullptr) {
                    fRealIn_.Close ();
                }
                Ensure (not IsOpenRead ());
                Assert (fRealIn_ == nullptr);
            }
            virtual bool IsOpenRead () const override
            {
                return fRealIn_ != nullptr;
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                return fSeekOffset_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fSeekOffset_ < fBufferOfAllReadDataSoFar_.size ()) {
                    return fBufferOfAllReadDataSoFar_.size () - fSeekOffset_; // don't include what we might get upstream cuz more costly to compute
                }
                return fRealIn_.AvailableToRead ();
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (auto rl = fRealIn_.RemainingLength ()) {
                    return MapOffsetFromReal2Mine_ (*rl);
                }
                return nullopt;
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                Assert (fSeekOffset_ <= fBufferOfAllReadDataSoFar_.size ());
                if (fSeekOffset_ == fBufferOfAllReadDataSoFar_.size ()) {
                    ELEMENT_TYPE buf[1024];
                    if (auto r = fRealIn_.Read (span{buf}, blockFlag)) {
                        fBufferOfAllReadDataSoFar_.push_back (*r); // continue, and fall through
                    }
                    else {
                        return nullopt; // no data pre-read, and nothing available upstream
                    }
                }
                if (fSeekOffset_ < fBufferOfAllReadDataSoFar_.size ()) {
                    size_t n2Read = min<size_t> (intoBuffer.size (), fBufferOfAllReadDataSoFar_.size () - fSeekOffset_);
                    auto   result = Memory::CopySpanData (span{fBufferOfAllReadDataSoFar_}.subspan (fSeekOffset_, n2Read), intoBuffer);
                    Assert (result.size () == n2Read);
                    fSeekOffset_ += n2Read;
                    return result;
                }
                return nullopt;
            }

        private:
            nonvirtual SeekOffsetType MapOffsetFromReal2Mine_ (SeekOffsetType so) const
            {
                return static_cast<SeekOffsetType> (static_cast<SignedSeekOffsetType> (so) + static_cast<SignedSeekOffsetType> (fRealIn_.GetOffset ()) -
                                                    static_cast<SignedSeekOffsetType> (fSeekOffset_));
            }

        private:
            typename InputStream::Ptr<ELEMENT_TYPE>                        fRealIn_;
            Memory::InlineBuffer<ELEMENT_TYPE, INLINE_BUF_SIZE>            fBufferOfAllReadDataSoFar_;
            SeekOffsetType                                                 fSeekOffset_{0}; // always inside fBufferOfAllReadDataSoFar_
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };

        // pretty easy/efficient case cuz we can throw away data as we go, and since not seekable, not many cases to analyze
        template <typename ELEMENT_TYPE, size_t INLINE_BUF_SIZE>
        class Rep_UnSeekable_ : public IRep_<ELEMENT_TYPE> {
        public:
            Rep_UnSeekable_ (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
                : fRealIn_{realIn}
            {
            }
            virtual bool IsSeekable () const override
            {
                return false;
            }
            virtual void CloseRead () override
            {
                if (fRealIn_ != nullptr) {
                    fRealIn_.Close ();
                }
                Ensure (not IsOpenRead ());
                Assert (fRealIn_ == nullptr);
            }
            virtual bool IsOpenRead () const override
            {
                return fRealIn_ != nullptr;
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                return fReadOffsetIntoIntermediateBuf_ + fRealIn_.GetOffset ();
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                size_t n = GetNEltsAlreadyBufferedFromUpstream_ ();
                if (auto o = fRealIn_.AvailableToRead ()) {
                    // if we KNOW what's available upstream, add to what we've pre-read
                    return *o + n;
                }
                else if (n != 0) {
                    return n; // if zero buffered, and nothing KNOWN about upstream, return nullopt
                }
                return nullopt; // if nothing upstream available (but not zero/eof)
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (auto o = fRealIn_.RemainingLength ()) {
                    return *o + GetNEltsAlreadyBufferedFromUpstream_ ();
                }
                return nullopt; // if nothing upstream available (but not zero/eof)
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                auto n = GetNEltsAlreadyBufferedFromUpstream_ ();
                if (n == 0) {
                    // read into fIntermediateBuffer_ (not intoBuffer)- OK to overwrite cuz
                    // no seeking allowed, so we will never re-examine that data/buffer
                    auto bufR = fRealIn_.Read (span{fIntermediateBuffer_}, blockFlag);
                    if (bufR) {
                        // we filled buffer (possibly with zero elements)
                        fReadOffsetIntoIntermediateBuf_ = 0;
                        n                               = bufR->size ();
                    }
                    else {
                        return nullopt; // no new information, don't change state so can Read again
                    }
                }
                // if we get here, and n = 0, really EOF, cuz return above on NOT-AVAIL case
                Assert (n != 0 or fRealIn_.IsAtEOF ());
                size_t n2Read = Math::AtMost (n, intoBuffer.size ());
                auto   t = Memory::CopySpanData (span{fIntermediateBuffer_}.subspan (fReadOffsetIntoIntermediateBuf_, n2Read), intoBuffer);
                Assert (t.size () == n2Read);
                fReadOffsetIntoIntermediateBuf_ += n2Read;
                return t;
            }

        private:
            nonvirtual size_t GetNEltsAlreadyBufferedFromUpstream_ () const
            {
                Assert (fReadOffsetIntoIntermediateBuf_ <= fIntermediateBuffer_.size ());
                return fIntermediateBuffer_.size () - fReadOffsetIntoIntermediateBuf_;
            }

        private:
            typename InputStream::Ptr<ELEMENT_TYPE>                        fRealIn_;
            Memory::InlineBuffer<ELEMENT_TYPE, INLINE_BUF_SIZE>            fIntermediateBuffer_;
            size_t                                                         fReadOffsetIntoIntermediateBuf_{0};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ********************* Streams::BufferedInputStream::New ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, optional<bool> seekable) -> Ptr<ELEMENT_TYPE>
    {
        using PTR                        = Ptr<ELEMENT_TYPE>;
        bool             srcSeekable     = realIn.IsSeekable ();
        bool             useSeekable     = seekable.value_or (srcSeekable);
        constexpr size_t INLINE_BUF_SIZE = 4 * 1024;
        if (useSeekable) {
            return srcSeekable ? PTR{make_shared<Private_::Rep_Seekable_FromSeekable_<ELEMENT_TYPE>> (realIn)}
                               : PTR{make_shared<Private_::Rep_Seekable_FromUnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> (realIn)};
        }
        else {
            return PTR{make_shared<Private_::Rep_UnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> (realIn)};
        }
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn,
                     optional<bool> seekable) -> Ptr<ELEMENT_TYPE>
    {
        constexpr size_t INLINE_BUF_SIZE = 4 * 1024;
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized: {
                bool srcSeekable = realIn.IsSeekable ();
                bool useSeekable = seekable.value_or (srcSeekable);
                if (useSeekable) {
                    return srcSeekable
                               ? InternallySynchronizedInputStream::New<Private_::Rep_Seekable_FromSeekable_<ELEMENT_TYPE>> ({}, realIn)
                               : InternallySynchronizedInputStream::New<Private_::Rep_Seekable_FromUnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> ({}, realIn);
                }
                else {
                    return InternallySynchronizedInputStream::New<Private_::Rep_UnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> ({}, realIn);
                }
            }
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realIn, seekable);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

    /*
     ********************************************************************************
     ******************* BufferedInputStream::Ptr<ELEMENT_TYPE> *********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::IRep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }

}
