/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Traversal/Iterator.h"

#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::ExternallyOwnedSpanInputStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ ()            = delete;
            Rep_ (const Rep_&) = delete;
            template <size_t EXTENT_T>
            Rep_ (span<const ELEMENT_TYPE, EXTENT_T> s)
                : fStart_{s.data ()}
                , fEnd_{fStart_ + s.size ()}
                , fCursor_{fStart_}
            {
            }

        public:
            nonvirtual Rep_& operator= (const Rep_&) = delete;

        private:
            bool fIsOpenForRead_{true};

        protected:
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseRead () override
            {
                fIsOpenForRead_ = false;
                Ensure (not IsOpenRead ());
            }
            virtual bool IsOpenRead () const override
            {
                return fIsOpenForRead_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                Ensure (fEnd_ >= fCursor_);
                return static_cast<size_t> (fEnd_ - fCursor_);
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                Ensure (fEnd_ >= fCursor_);
                return static_cast<size_t> (fEnd_ - fCursor_);
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, [[maybe_unused]] NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Require (IsOpenRead ());
                size_t                                                 nRequested = intoBuffer.size ();
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
                size_t nAvail  = fEnd_ - fCursor_;
                size_t nCopied = min (nAvail, nRequested);
                copy (fCursor_, fCursor_ + nCopied, intoBuffer.data ());
                fCursor_ += nCopied;
                return intoBuffer.subspan (0, nCopied); // this can be empty on EOF
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fCursor_ - fStart_;
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                static const auto kRangeException_ = range_error{"seek"};
                switch (whence) {
                    case eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        if (offset > (fEnd_ - fStart_)) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        fCursor_ = fStart_ + offset;
                    } break;
                    case eFromCurrent: {
                        Streams::SeekOffsetType       curOffset = fCursor_ - fStart_;
                        Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        if (newOffset > (fEnd_ - fStart_)) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        fCursor_ = fStart_ + newOffset;
                    } break;
                    case eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = (fEnd_ - fStart_) + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        if (newOffset > (fEnd_ - fStart_)) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        fCursor_ = fStart_ + newOffset;
                    } break;
                }
                Ensure ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
                return fCursor_ - fStart_;
            }

        private:
            const ELEMENT_TYPE*                  fStart_;
            const ELEMENT_TYPE*                  fEnd_;
            const ELEMENT_TYPE*                  fCursor_;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_TRY_ANYHOW Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ************ Streams::ExternallyOwnedSpanInputStream<ELEMENT_TYPE> *************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, Memory::ISpanBytesCastable<span<const ELEMENT_TYPE>> FROM_SPAN>
    Ptr<ELEMENT_TYPE> New (FROM_SPAN s)
    {
        return Ptr<ELEMENT_TYPE>{Memory::MakeSharedPtr<Private_::Rep_<ELEMENT_TYPE>> (Memory::SpanBytesCast<span<const ELEMENT_TYPE>> (s))};
    }
    template <typename ELEMENT_TYPE, Memory::ISpanBytesCastable<span<const ELEMENT_TYPE>> FROM_SPAN>
    inline Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, FROM_SPAN s)
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, Memory::SpanBytesCast<span<const ELEMENT_TYPE>> (s));
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (s);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}
