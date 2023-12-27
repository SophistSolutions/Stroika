/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SharedMemoryStream_inl_
#define _Stroika_Foundation_Streams_SharedMemoryStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/Cast.h"
#include "../Execution/WaitableEvent.h"

namespace Stroika::Foundation::Streams::SharedMemoryStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputOutputStream::IRep<ELEMENT_TYPE> {
        public:
            using ElementType = ELEMENT_TYPE;

        private:
            bool fIsOpenForRead_{true};

        public:
            Rep_ ()
                : fReadCursor_{fData_.begin ()}
                , fWriteCursor_{fData_.begin ()}
            {
            }
            Rep_ (const Rep_&)                       = delete;
            ~Rep_ ()                                 = default;
            nonvirtual Rep_& operator= (const Rep_&) = delete;

            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                {
                    [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                    fClosedForWrites_               = true;
                }
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
            virtual span<ELEMENT_TYPE> Read (span<ELEMENT_TYPE> intoBuffer) override
            {
                Require (not intoBuffer.empty ());
                Require (IsOpenRead ());
                size_t nRequested = intoBuffer.size ();

            tryAgain:
                fMoreDataWaiter_.Wait ();

                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_}; // hold lock for everything EXCEPT wait
                Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                size_t nAvail = fData_.end () - fReadCursor_;
                if (nAvail == 0 and not fClosedForWrites_) {
                    fMoreDataWaiter_.Reset (); // ?? @todo consider - is this a race? If we reset at same time(apx) as someone else sets
                    goto tryAgain;             // cannot wait while we hold lock
                }
                size_t nCopied = min (nAvail, nRequested);
                {
                    copy (fReadCursor_, fReadCursor_ + nCopied, intoBuffer.data ());
                    fReadCursor_ = fReadCursor_ + nCopied;
                }
                return intoBuffer.subspan (0, nCopied); // this can be empty on EOF
            }
            virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                Require (IsOpenRead ());
                [[maybe_unused]] auto&& critSec          = lock_guard{fMutex_};
                size_t                  nDefinitelyAvail = fData_.end () - fReadCursor_;
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
                    [[maybe_unused]] auto&& critSec      = lock_guard{fMutex_};
                    size_t                  roomLeft     = fData_.end () - fWriteCursor_;
                    size_t                  roomRequired = end - start;
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                    if (roomLeft < roomRequired) {
                        size_t       curReadOffset  = fReadCursor_ - fData_.begin ();
                        size_t       curWriteOffset = fWriteCursor_ - fData_.begin ();
                        const size_t kChunkSize_    = 128; // WAG: @todo tune number...
                        Containers::Support::ReserveTweaks::Reserve4AddN (fData_, roomRequired - roomLeft, kChunkSize_);
                        fData_.resize (curWriteOffset + roomRequired);
                        fReadCursor_  = fData_.begin () + curReadOffset;
                        fWriteCursor_ = fData_.begin () + curWriteOffset;
                        Assert (fWriteCursor_ < fData_.end ());
                    }
                    copy (start, start + roomRequired, fWriteCursor_);
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
                Require (IsOpenRead ());
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                return fReadCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                        if (uOffset > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fReadCursor_ = fData_.begin () + static_cast<size_t> (uOffset);
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SeekOffsetType       curOffset = fReadCursor_ - fData_.begin ();
                        Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                        if (uNewOffset > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = fData_.size () + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                        if (uNewOffset > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                    } break;
                }
                Ensure ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                return fReadCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Require (IsOpenWrite ());
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                return fWriteCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenWrite ());
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        if (static_cast<SeekOffsetType> (offset) > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fWriteCursor_ = fData_.begin () + static_cast<size_t> (offset);
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SeekOffsetType       curOffset = fWriteCursor_ - fData_.begin ();
                        Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        if (static_cast<size_t> (newOffset) > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = fData_.size () + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        if (static_cast<size_t> (newOffset) > fData_.size ()) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                    } break;
                }
                Ensure ((fData_.begin () <= fWriteCursor_) and (fWriteCursor_ <= fData_.end ()));
                return fWriteCursor_ - fData_.begin ();
            }
            vector<ElementType> AsVector () const
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                return fData_;
            }
            string AsString () const
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
                return string{reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_))};
            }

        private:
            static inline const auto kSeekException_ = range_error{"seek"};
            mutable recursive_mutex  fMutex_;
            Execution::WaitableEvent fMoreDataWaiter_{}; // not a race cuz always set/reset when holding fMutex; no need to pre-set cuz auto set when someone adds data (Write)
            vector<ElementType>                    fData_; // Important data comes before cursors cuz of use in CTOR
            typename vector<ElementType>::iterator fReadCursor_;
            typename vector<ElementType>::iterator fWriteCursor_;
            bool                                   fClosedForWrites_{false};
        };
    }

    /*
     ********************************************************************************
     ********************** SharedMemoryStream<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New () -> Ptr<ELEMENT_TYPE>
    {
        return make_shared<Private_::Rep_<ELEMENT_TYPE>> ();
    }
    template <typename ELEMENT_TYPE>
    inline auto New (const span<const byte>& copyFrom) -> Ptr<ELEMENT_TYPE>
        requires (same_as<ELEMENT_TYPE, byte>)
    {
        auto p = New<ELEMENT_TYPE> ();
        p.Write (copyFrom);
        return p;
    }
    template <typename ELEMENT_TYPE>
    inline auto New (const Memory::BLOB& copyFrom) -> Ptr<ELEMENT_TYPE>
        requires (same_as<ELEMENT_TYPE, byte>)
    {
        auto p = New<ELEMENT_TYPE> ();
        p.Write (copyFrom);
        return p;
    }

    /*
     ********************************************************************************
     ****************** SharedMemoryStream::Ptr<ELEMENT_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::Rep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef_ () const -> const Private_::Rep_<ELEMENT_TYPE>&
    {
        return *Debug::UncheckedDynamicCast<const Private_::Rep_<ELEMENT_TYPE>*> (&inherited::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepRWRef_ () const -> Private_::Rep_<ELEMENT_TYPE>&
    {
        return *Debug::UncheckedDynamicCast<Private_::Rep_<ELEMENT_TYPE>*> (&inherited::GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    template <typename T>
    inline T Ptr<ELEMENT_TYPE>::As () const
        requires (same_as<T, vector<ELEMENT_TYPE>> or (same_as<ELEMENT_TYPE, byte> and (same_as<T, Memory::BLOB> or same_as<T, string>)) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and (same_as<T, Characters::String>)))
    {
        using Characters::Character;
        using Characters::String;
        using Memory::BLOB;
        if constexpr (same_as<T, vector<ELEMENT_TYPE>>) {
            return GetRepConstRef_ ().AsVector ();
        }
        else if constexpr (same_as<T, Memory::BLOB>) {
            return GetRepConstRef_ ().AsVector ();
        }
        else if constexpr (same_as<T, string>) {
            return GetRepConstRef_ ().AsString ();
        }
        else if constexpr (same_as<T, String>) {
            auto tmp = GetRepConstRef_ ().AsVector ();
            return String{span{tmp}};
        }
    }

    /// deprecated
    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d5 - use span overload")]] inline auto New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
        -> Ptr<ELEMENT_TYPE>
    {
        return make_shared<Private_::Rep_<ELEMENT_TYPE>> (start, end);
    }

}

#endif /*_Stroika_Foundation_Streams_SharedMemoryStream_inl_*/
