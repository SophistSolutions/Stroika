/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_inl_
#define _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_inl_ 1

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/Span.h"
#include "../Traversal/Iterator.h"

#include "InternallySynchronizedInputStream.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Streams::ExternallyOwnedSpanInputStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ ()            = delete;
            Rep_ (const Rep_&) = delete;
            template <size_t EXTENT_T>
            Rep_ (span<const ELEMENT_TYPE, EXTENT_T> s)
                : fStart_{Traversal::Iterator2Pointer (s.begin ())}
                , fEnd_{fStart_ + s.size ()}
                , fCursor_{fStart_}
            {
            }
            template <typename ELEMENT_TYPE2, size_t EXTENT_T>
            Rep_ (span<const ELEMENT_TYPE2, EXTENT_T> s)
                requires (same_as<ELEMENT_TYPE, byte> and (same_as<ELEMENT_TYPE2, char> or same_as<ELEMENT_TYPE2, uint8_t>))
                : fStart_{reinterpret_cast<const byte*> (Traversal::Iterator2Pointer (s.begin ()))}
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
                Require (IsOpenRead ());
                fIsOpenForRead_ = false;
            }
            virtual bool IsOpenRead () const override
            {
                return fIsOpenForRead_;
            }
            virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                Require (IsOpenRead ());
                size_t                                                 nRequested = intoEnd - intoStart;
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
                size_t nAvail  = fEnd_ - fCursor_;
                size_t nCopied = min (nAvail, nRequested);
                copy (fCursor_, fCursor_ + nCopied, intoStart);
                fCursor_ += nCopied;
                return nCopied; // this can be zero on EOF
            }
            virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return this->_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fEnd_ - fCursor_);
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
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        if (offset > (fEnd_ - fStart_)) [[unlikely]] {
                            Execution::Throw (kRangeException_);
                        }
                        fCursor_ = fStart_ + offset;
                    } break;
                    case Whence::eFromCurrent: {
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
                    case Whence::eFromEnd: {
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
            const ELEMENT_TYPE*                                            fStart_;
            const ELEMENT_TYPE*                                            fEnd_;
            const ELEMENT_TYPE*                                            fCursor_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ************ Streams::ExternallyOwnedSpanInputStream<ELEMENT_TYPE> *************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename ELEMENT_TYPE2, size_t EXTENT2>
    inline Ptr<ELEMENT_TYPE> New (span<ELEMENT_TYPE2, EXTENT2> s)
        requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                  (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, char> or same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)))
    {
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep_<ELEMENT_TYPE>> (Memory::ConstSpan (s))};
    }
    template <typename ELEMENT_TYPE, typename ELEMENT_TYPE2, size_t EXTENT2>
    inline Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, span<const ELEMENT_TYPE2, EXTENT2> s)
        requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                  (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, char> or same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)))
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, Memory::ConstSpan (s));
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (s);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_inl_*/
