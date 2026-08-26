/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputStream.h"
#include "Stroika/Foundation/Streams/StreamReader.h"

namespace Stroika::Foundation::Streams::BufferedInputStream {

    namespace Private_ {

        [[noreturn]] void ThrowCannotSeekFromEnd_ ();

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
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.AvailableToRead (); // since no actual buffering here yet
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.RemainingLength ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.Seek (whence, offset);
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReader_.Read (intoBuffer, blockFlag);
            }

        private:
            typename InputStream::Ptr<ELEMENT_TYPE>      fRealIn_;
            StreamReader<ELEMENT_TYPE>                   fReader_;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
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
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fSeekOffset_ < fBufferOfAllReadDataSoFar_.size ()) [[likely]] {
                    return fBufferOfAllReadDataSoFar_.size () - static_cast<size_t> (fSeekOffset_); // don't include what we might get upstream cuz more costly to compute
                }
                return fRealIn_.AvailableToRead ();
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (auto rl = fRealIn_.RemainingLength ()) {
                    return MapOffsetFromReal2Mine_ (*rl);
                }
                return nullopt;
            }
            virtual auto SeekRead (Whence whence, SignedSeekOffsetType offset) -> SeekOffsetType override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                // @todo - allow seek forward past fBufferOfAllReadDataSoFar_?
                switch (whence) {
                    case Whence::eFromCurrent:
                        fSeekOffset_ += offset;
                        break;
                    case Whence::eFromStart:
                        fSeekOffset_ = offset;
                        break;
                    case Whence::eFromEnd:
                        if (auto remaining = this->RemainingLength ()) {
                            fSeekOffset_ += static_cast<SignedSeekOffsetType> (*remaining) - offset;
                            break;
                        }
                        else {
                            Private_::ThrowCannotSeekFromEnd_ ();
                        }
                    default:
                        RequireNotReached ();
                }
                return fSeekOffset_;
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                Assert (fSeekOffset_ <= fBufferOfAllReadDataSoFar_.size ());
                if (fSeekOffset_ == fBufferOfAllReadDataSoFar_.size ()) [[unlikely]] {
                    ELEMENT_TYPE buf[1024];
                    if (auto r = fRealIn_.Read (span{buf}, blockFlag)) {
                        fBufferOfAllReadDataSoFar_.push_back (*r); // continue, and fall through
                    }
                    else {
                        return nullopt; // no data pre-read, and nothing available upstream
                    }
                }
                if (fSeekOffset_ <= fBufferOfAllReadDataSoFar_.size ()) [[likely]] {
                    size_t n2Read = min<size_t> (intoBuffer.size (), static_cast<size_t> (fBufferOfAllReadDataSoFar_.size () - fSeekOffset_));
                    auto result = Memory::CopySpanData (span{fBufferOfAllReadDataSoFar_}.subspan (static_cast<size_t> (fSeekOffset_), n2Read), intoBuffer);
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
            typename InputStream::Ptr<ELEMENT_TYPE>             fRealIn_;
            Memory::InlineBuffer<ELEMENT_TYPE, INLINE_BUF_SIZE> fBufferOfAllReadDataSoFar_;
            SeekOffsetType                                      fSeekOffset_{0}; // always inside fBufferOfAllReadDataSoFar_
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
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
                // fRealIn_ has typically pre-read PAST what the caller has consumed, so back out
                // whatever is still sitting unread in fIntermediateBuffer_
                Assert (fRealIn_.GetOffset () >= GetNEltsAlreadyBufferedFromUpstream_ ());
                return fRealIn_.GetOffset () - GetNEltsAlreadyBufferedFromUpstream_ ();
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
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
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (auto o = fRealIn_.RemainingLength ()) {
                    return *o + GetNEltsAlreadyBufferedFromUpstream_ ();
                }
                return nullopt; // if nothing upstream available (but not zero/eof)
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                auto n = GetNEltsAlreadyBufferedFromUpstream_ ();
                if (n == 0) [[unlikely]] {
                    // read a big chunk upstream (not into intoBuffer, which may be tiny) - OK to
                    // overwrite what we had cuz no seeking allowed, so we never re-examine it.
                    //  NB: read via a local buffer, NOT span{fIntermediateBuffer_}: an InlineBuffer's
                    //  size () starts out ZERO (INLINE_BUF_SIZE is only its inline CAPACITY), so that
                    //  span would be empty - which is a precondition violation in Read (). Going
                    //  through a local also means fIntermediateBuffer_ is only touched once the read
                    //  has actually succeeded.
                    ELEMENT_TYPE buf[INLINE_BUF_SIZE];
                    if (auto bufR = fRealIn_.Read (span{buf}, blockFlag)) {
                        // we filled buffer (possibly with zero elements)
                        fIntermediateBuffer_.resize_uninitialized (0);
                        fIntermediateBuffer_.push_back (*bufR);
                        fReadOffsetIntoIntermediateBuf_ = 0;
                        n                               = bufR->size ();
                    }
                    else {
                        return nullopt; // no new information, don't change state so can Read again
                    }
                }
                // if we get here, and n = 0, really EOF, cuz return above on NOT-AVAIL case
                //  nb: IsAtEOF () peeks - read then seek back - so it can only be asked of a
                //  seekable stream, and fRealIn_ here often is not one
                Assert (n != 0 or not fRealIn_.IsSeekable () or fRealIn_.IsAtEOF ());
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
            typename InputStream::Ptr<ELEMENT_TYPE>             fRealIn_;
            Memory::InlineBuffer<ELEMENT_TYPE, INLINE_BUF_SIZE> fIntermediateBuffer_;
            size_t                                              fReadOffsetIntoIntermediateBuf_{0};
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ********************* Streams::BufferedInputStream::New ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, optional<SeekableFlag> seekable) -> Ptr<ELEMENT_TYPE>
    {
        using PTR                        = Ptr<ELEMENT_TYPE>;
        SeekableFlag     srcSeekable     = realIn.GetSeekability ();
        SeekableFlag     useSeekable     = seekable.value_or (srcSeekable);
        constexpr size_t INLINE_BUF_SIZE = 4 * 1024;
        if (useSeekable == SeekableFlag::eSeekable) {
            return (srcSeekable == SeekableFlag::eSeekable)
                       ? PTR{Memory::MakeSharedPtr<Private_::Rep_Seekable_FromSeekable_<ELEMENT_TYPE>> (realIn)}
                       : PTR{Memory::MakeSharedPtr<Private_::Rep_Seekable_FromUnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> (realIn)};
        }
        else {
            return PTR{Memory::MakeSharedPtr<Private_::Rep_UnSeekable_<ELEMENT_TYPE, INLINE_BUF_SIZE>> (realIn)};
        }
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn,
                     optional<SeekableFlag> seekable) -> Ptr<ELEMENT_TYPE>
    {
        constexpr size_t INLINE_BUF_SIZE = 4 * 1024;
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized: {
                SeekableFlag srcSeekable = realIn.GetSeekability ();
                SeekableFlag useSeekable = seekable.value_or (srcSeekable);
                if (useSeekable == SeekableFlag::eSeekable) {
                    return (srcSeekable == SeekableFlag::eSeekable)
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

    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d19 use Seekability overload")]] Ptr<ELEMENT_TYPE> New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn,
                                                                                             optional<bool> seekable)
    {
        optional<SeekableFlag> sf;
        if (seekable) {
            sf = *seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
        }
        return New (realIn, sf);
    }
    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d19 use Seekability overload")]] Ptr<ELEMENT_TYPE>
    New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, optional<bool> seekable = {})
    {
        optional<SeekableFlag> sf;
        if (seekable) {
            sf = *seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
        }
        return New (internallySynchronized, realIn, sf);
    }
}
