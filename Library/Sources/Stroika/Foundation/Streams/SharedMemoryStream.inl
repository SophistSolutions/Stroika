/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SharedMemoryStream_inl_
#define _Stroika_Foundation_Streams_SharedMemoryStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Execution/WaitableEvent.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             ******************* SharedMemoryStream<ELEMENT_TYPE>::Rep_ *********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class SharedMemoryStream<ELEMENT_TYPE>::Rep_ : public InputOutputStream<ELEMENT_TYPE>::_IRep {
            public:
                using ElementType = ELEMENT_TYPE;

            private:
                bool fIsOpenForRead_{true};

            public:
                Rep_ ()
                    : fData_ ()
                    , fReadCursor_ (fData_.begin ())
                    , fWriteCursor_ (fData_.begin ())
                {
                }
                Rep_ (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
                    : Rep_ ()
                {
                    Write (start, end);
                }
                Rep_ (const Rep_&)       = delete;
                ~Rep_ ()                 = default;
                nonvirtual Rep_& operator= (const Rep_&) = delete;

                virtual bool IsSeekable () const override
                {
                    return true;
                }
                virtual void CloseWrite () override
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Require (IsOpenWrite ());
                    fClosedForWrites_ = true;
                    fMoreDataWaiter_.Set ();
                }
                virtual bool IsOpenWrite () const override
                {
                    return not fClosedForWrites_;
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
                    size_t nRequested = intoEnd - intoStart;
                    //bool   mustWaitForData{false};    // dont think we need that cuz fMoreDataWaiter_ being set does same thing

                tryAgain:
                    // if (mustWaitForData) {
                    fMoreDataWaiter_.Wait ();
                    //}

                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                    size_t nAvail = fData_.end () - fReadCursor_;
                    if (nAvail == 0 and not fClosedForWrites_) {
                        ///mustWaitForData = true;
                        fMoreDataWaiter_.Reset ();
                        goto tryAgain;
                        // cannot wait while we hold lock
                    }
                    size_t nCopied = min (nAvail, nRequested);
                    {
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
                        Memory::Private::VC_BWA_std_copy (fReadCursor_, fReadCursor_ + nCopied, intoStart);
#else
                        std::copy (fReadCursor_, fReadCursor_ + nCopied, intoStart);
#endif
                        fReadCursor_ = fReadCursor_ + nCopied;
                    }
                    return nCopied; // this can be zero on EOF
                }
                virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                    Require (IsOpenRead ());
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    size_t                      nDefinitelyAvail = fData_.end () - fReadCursor_;
                    if (nDefinitelyAvail > 0) {
                        return this->_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, nDefinitelyAvail);
                    }
                    else if (fClosedForWrites_) {
                        return this->_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, 0);
                    }
                    else {
                        return {}; // if nothing available, but not closed for write, no idea if more to come
                    }
                }
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    Require (start != nullptr or start == end);
                    Require (end != nullptr or start == end);
                    Require (IsOpenWrite ());
                    if (start != end) {
                        lock_guard<recursive_mutex> critSec{fMutex_};
                        size_t                      roomLeft     = fData_.end () - fWriteCursor_;
                        size_t                      roomRequired = end - start;
                        fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                        if (roomLeft < roomRequired) {
                            size_t       curReadOffset  = fReadCursor_ - fData_.begin ();
                            size_t       curWriteOffset = fWriteCursor_ - fData_.begin ();
                            const size_t kChunkSize_    = 128; // WAG: @todo tune number...
                            Containers::ReserveSpeedTweekAddN (fData_, roomRequired - roomLeft, kChunkSize_);
                            fData_.resize (curWriteOffset + roomRequired);
                            fReadCursor_  = fData_.begin () + curReadOffset;
                            fWriteCursor_ = fData_.begin () + curWriteOffset;
                            Assert (fWriteCursor_ < fData_.end ());
                        }
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
                        Memory::Private::VC_BWA_std_copy (start, start + roomRequired, fWriteCursor_);
#else
                        std::copy (start, start + roomRequired, fWriteCursor_);
#endif
                        fWriteCursor_ += roomRequired;
                        Assert (fReadCursor_ < fData_.end ()); // < because we wrote at least one byte and that didnt move read cursor
                        Assert (fWriteCursor_ <= fData_.end ());
                    }
                }
                virtual void Flush () override
                {
                    // nothing todo - write 'writes thru'
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Require (IsOpenRead ());
                    return fReadCursor_ - fData_.begin ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Require (IsOpenRead ());
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                    switch (whence) {
                        case Whence::eFromStart: {
                            if (offset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                            if (uOffset > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fReadCursor_ = fData_.begin () + static_cast<size_t> (uOffset);
                        } break;
                        case Whence::eFromCurrent: {
                            Streams::SeekOffsetType       curOffset = fReadCursor_ - fData_.begin ();
                            Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                            if (uNewOffset > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                        } break;
                        case Whence::eFromEnd: {
                            Streams::SignedSeekOffsetType newOffset = fData_.size () + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                            if (uNewOffset > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                        } break;
                    }
                    Ensure ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                    return fReadCursor_ - fData_.begin ();
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Require (IsOpenWrite ());
                    return fWriteCursor_ - fData_.begin ();
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    Require (IsOpenWrite ());
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                    switch (whence) {
                        case Whence::eFromStart: {
                            if (offset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (static_cast<SeekOffsetType> (offset) > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fWriteCursor_ = fData_.begin () + static_cast<size_t> (offset);
                        } break;
                        case Whence::eFromCurrent: {
                            Streams::SeekOffsetType       curOffset = fWriteCursor_ - fData_.begin ();
                            Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (static_cast<size_t> (newOffset) > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                        } break;
                        case Whence::eFromEnd: {
                            Streams::SignedSeekOffsetType newOffset = fData_.size () + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (static_cast<size_t> (newOffset) > fData_.size ()) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                        } break;
                    }
                    Ensure ((fData_.begin () <= fWriteCursor_) and (fWriteCursor_ <= fData_.end ()));
                    return fWriteCursor_ - fData_.begin ();
                }
                vector<ElementType> AsVector () const
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    return fData_;
                }
                string AsString () const
                {
                    lock_guard<recursive_mutex> critSec{fMutex_};
                    return string (reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_)));
                }

            private:
                mutable recursive_mutex                fMutex_;
                Execution::WaitableEvent               fMoreDataWaiter_{Execution::WaitableEvent::eManualReset}; // not a race cuz always set/reset when holding fMutex; no need to pre-set cuz auto set when someone adds data (Write)
                vector<ElementType>                    fData_;
                typename vector<ElementType>::iterator fReadCursor_;
                typename vector<ElementType>::iterator fWriteCursor_;
                bool                                   fClosedForWrites_{false};
            };

            /*
             ********************************************************************************
             ********************** SharedMemoryStream<ELEMENT_TYPE> ************************
             ********************************************************************************
             */
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wattributes\"")
            template <typename ELEMENT_TYPE>
            inline auto SharedMemoryStream<ELEMENT_TYPE>::New ([[maybe_unused]] Execution::InternallySyncrhonized internallySyncrhonized) -> Ptr
            {
                // always return internally synchronized rep
                return make_shared<Rep_> ();
            }
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wattributes\"")
            template <typename ELEMENT_TYPE>
            inline auto SharedMemoryStream<ELEMENT_TYPE>::New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) -> Ptr
            {
                return make_shared<Rep_> (start, end);
            }
            template <typename ELEMENT_TYPE>
            inline auto SharedMemoryStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) -> Ptr
            {
                return New (start, end);
            }
            template <typename ELEMENT_TYPE>
            template <typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline auto SharedMemoryStream<ELEMENT_TYPE>::New (const Memory::BLOB& blob) -> Ptr
            {
                return New (blob.begin (), blob.end ());
            }
            template <typename ELEMENT_TYPE>
            template <typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline auto SharedMemoryStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const Memory::BLOB& blob) -> Ptr
            {
                return New (blob.begin (), blob.end ());
            }

            /*
             ********************************************************************************
             ****************** SharedMemoryStream<ELEMENT_TYPE>::Ptr ***********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline SharedMemoryStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <>
            template <>
            inline vector<Memory::Byte> SharedMemoryStream<Memory::Byte>::Ptr::As () const
            {
                RequireNotNull (_GetSharedRep ().get ());
                AssertMember (_GetSharedRep ().get (), Rep_);
                const Rep_& rep = *dynamic_cast<const Rep_*> (_GetSharedRep ().get ());
                return rep.AsVector ();
            }
            template <>
            template <>
            inline vector<Characters::Character> SharedMemoryStream<Characters::Character>::Ptr::As () const
            {
                RequireNotNull (_GetSharedRep ().get ());
                AssertMember (_GetSharedRep ().get (), Rep_);
                const Rep_& rep = *dynamic_cast<const Rep_*> (_GetSharedRep ().get ());
                return rep.AsVector ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_SharedMemoryStream_inl_*/
