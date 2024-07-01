/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <mutex>

#include "Stroika/Foundation/Configuration/Empty.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Execution/NullMutex.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

namespace Stroika::Foundation::Streams::SharedMemoryStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class IRep_ : public InputOutputStream::IRep<ELEMENT_TYPE> {
        public:
            virtual Options              GetOptions () const = 0;
            virtual vector<ELEMENT_TYPE> AsVector () const   = 0;
            virtual string               AsString () const   = 0;
        };
        template <typename ELEMENT_TYPE, typename LOCK_IMPL>
        class SeekableRep_ : public IRep_<ELEMENT_TYPE> {
        public:
            using ElementType = ELEMENT_TYPE;

        private:
            bool fIsOpenForRead_{true};

        private:
            static constexpr bool kLocking_ = same_as<LOCK_IMPL, recursive_mutex>;

        public:
            SeekableRep_ ()
                : fReadCursor_{fData_.begin ()}
                , fWriteCursor_{fData_.begin ()}
            {
            }
            SeekableRep_ (const SeekableRep_&)                       = delete;
            ~SeekableRep_ ()                                         = default;
            nonvirtual SeekableRep_& operator= (const SeekableRep_&) = delete;

            // InputOutputStream::IRep<ELEMENT_TYPE> overrides
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                {
                    [[maybe_unused]] lock_guard critSec{fMutex_};
                    fClosedForWrites_ = true;
                }
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set ();
                }
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
            virtual optional<size_t> AvailableToRead () override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                size_t                      nDefinitelyAvail = distance (fReadCursor_, fData_.cend ());
                if (nDefinitelyAvail > 0) {
                    return nDefinitelyAvail;
                }
                else if (fClosedForWrites_) {
                    return 0;
                }
                else {
                    return nullopt; // if nothing available, but not closed for write, no idea if more to come
                }
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Require (IsOpenRead ());
                return nullopt; // pretty easy but @todo
            }
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-label\"");
            DISABLE_COMPILER_MSC_WARNING_START (4102)
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Require (IsOpenRead ());
                size_t nRequested = intoBuffer.size ();
            tryAgain:
                switch (blockFlag) {
                    case eDontBlock: {
                        if (this->AvailableToRead () == nullopt) {
                            return nullopt;
                        }
                    } break;
                    [[likely]] case eBlockIfNoDataAvailable: {
                        if constexpr (kLocking_) {
                            fMoreDataWaiter_.Wait ();
                        }
                    } break;
                }
                // at this point, data is available
                [[maybe_unused]] lock_guard critSec{fMutex_}; // hold lock for everything EXCEPT wait
                Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                size_t nAvail = distance (fReadCursor_, fData_.cend ());
                if (nAvail == 0 and not fClosedForWrites_) {
                    if constexpr (kLocking_) {
                        fMoreDataWaiter_.Reset (); // ?? @todo consider - is this a race? If we reset at same time(apx) as someone else sets
                        goto tryAgain;             // cannot wait while we hold lock
                    }
                    else {
                        Require (blockFlag == eDontBlock); // else would be a deadlock.
                    }
                }
                size_t nCopied = min (nAvail, nRequested);
                {
                    copy (fReadCursor_, fReadCursor_ + nCopied, intoBuffer.data ());
                    fReadCursor_ = fReadCursor_ + nCopied;
                }
                return intoBuffer.subspan (0, nCopied); // this can be empty on EOF
            }
            DISABLE_COMPILER_MSC_WARNING_END (4102)
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-label\"");
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ());
                Require (IsOpenWrite ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                size_t                      roomLeft     = distance (fWriteCursor_, fData_.end ());
                size_t                      roomRequired = elts.size ();
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                }
                if (roomLeft < roomRequired) {
                    size_t       curReadOffset  = distance (fData_.cbegin (), fReadCursor_);
                    size_t       curWriteOffset = distance (fData_.begin (), fWriteCursor_);
                    const size_t kChunkSize_    = 128; // WAG: @todo tune number...
                    Containers::Support::ReserveTweaks::Reserve4AddN (fData_, roomRequired - roomLeft, kChunkSize_);
                    fData_.resize (curWriteOffset + roomRequired);
                    fReadCursor_  = fData_.begin () + curReadOffset;
                    fWriteCursor_ = fData_.begin () + curWriteOffset;
                    Assert (fWriteCursor_ < fData_.end ());
                }
                copy (elts.data (), elts.data () + roomRequired, fWriteCursor_);
                fWriteCursor_ += roomRequired;
                Assert (fReadCursor_ < fData_.end ()); // < because we wrote at least one byte and that didn't move read cursor
                Assert (fWriteCursor_ <= fData_.end ());
            }
            virtual void Flush () override
            {
                // nothing todo - write 'writes thru'
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return distance (fData_.begin (), fReadCursor_);
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                }
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
                        Streams::SeekOffsetType       curOffset = distance (fData_.cbegin (), fReadCursor_);
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
                return distance (fData_.cbegin (), fReadCursor_);
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Require (IsOpenWrite ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return distance (fData_.begin (), static_cast<typename vector<ElementType>::const_iterator> (fWriteCursor_));
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenWrite ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                }
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
                        Streams::SeekOffsetType       curOffset = distance (fData_.begin (), fWriteCursor_);
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
                return distance (fData_.begin (), fWriteCursor_);
            }
            // Private_::IRep_ overrides
            virtual Options GetOptions () const override
            {
                return Options{.fInternallySynchronized = same_as<LOCK_IMPL, recursive_mutex>
                                                              ? Execution::InternallySynchronized::eInternallySynchronized
                                                              : Execution::InternallySynchronized::eNotKnownInternallySynchronized,
                               .fSeekable               = true};
            }
            virtual vector<ElementType> AsVector () const override
            {
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return fData_;
            }
            virtual string AsString () const override
            {
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return string{reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_))};
            }

        private:
            static inline const auto kSeekException_ = range_error{"seek"};
            mutable LOCK_IMPL        fMutex_;
            [[no_unique_address]] conditional_t<kLocking_, Execution::WaitableEvent, Configuration::Empty> fMoreDataWaiter_{}; // not a race cuz always set/reset when holding fMutex; no need to pre-set cuz auto set when someone adds data (Write)
            vector<ElementType>                          fData_; // Important data comes before cursors cuz of use in CTOR
            typename vector<ElementType>::const_iterator fReadCursor_;
            typename vector<ElementType>::iterator       fWriteCursor_;
            bool                                         fClosedForWrites_{false};
        };
        // @todo re-implement for  saving memory - since not seekable
        template <typename ELEMENT_TYPE, typename LOCK_IMPL>
        class UnseekableRep_ : public IRep_<ELEMENT_TYPE> {
        public:
            using ElementType = ELEMENT_TYPE;

        private:
            bool fIsOpenForRead_{true};

        private:
            static constexpr bool kLocking_ = same_as<LOCK_IMPL, recursive_mutex>;

        public:
            UnseekableRep_ ()
                : fReadCursor_{fData_.begin ()}
                , fWriteCursor_{fData_.begin ()}
            {
            }
            UnseekableRep_ (const UnseekableRep_&)                       = delete;
            ~UnseekableRep_ ()                                           = default;
            nonvirtual UnseekableRep_& operator= (const UnseekableRep_&) = delete;

            // InputOutputStream::IRep<ELEMENT_TYPE> overrides
            virtual bool IsSeekable () const override
            {
                return false;
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                {
                    [[maybe_unused]] lock_guard critSec{fMutex_};
                    fClosedForWrites_ = true;
                }
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set ();
                }
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
            virtual optional<size_t> AvailableToRead () override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                size_t                      nDefinitelyAvail = distance (fReadCursor_, fData_.cend ());
                if (nDefinitelyAvail > 0) {
                    return nDefinitelyAvail;
                }
                else if (fClosedForWrites_) {
                    return 0;
                }
                else {
                    return nullopt; // if nothing available, but not closed for write, no idea if more to come
                }
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Require (IsOpenRead ());
                return nullopt; // pretty easy but @todo
            }
            DISABLE_COMPILER_MSC_WARNING_START (4102)
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-label\"");
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Require (IsOpenRead ());
                size_t nRequested = intoBuffer.size ();
            tryAgain:
                switch (blockFlag) {
                    case eDontBlock: {
                        if (this->AvailableToRead () == nullopt) {
                            return nullopt;
                        }
                    } break;
                    [[likely]] case eBlockIfNoDataAvailable: {
                        if constexpr (kLocking_) {
                            fMoreDataWaiter_.Wait ();
                        }
                    } break;
                }
                // at this point, data is available
                [[maybe_unused]] lock_guard critSec{fMutex_}; // hold lock for everything EXCEPT wait
                Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                size_t nAvail = distance (fReadCursor_, fData_.cend ());
                if (nAvail == 0 and not fClosedForWrites_) {
                    if constexpr (kLocking_) {
                        fMoreDataWaiter_.Reset (); // ?? @todo consider - is this a race? If we reset at same time(apx) as someone else sets
                        goto tryAgain;             // cannot wait while we hold lock
                    }
                    else {
                        Require (blockFlag == eDontBlock); // else would be a deadlock.
                    }
                }
                size_t nCopied = min (nAvail, nRequested);
                {
                    copy (fReadCursor_, fReadCursor_ + nCopied, intoBuffer.data ());
                    fReadCursor_ = fReadCursor_ + nCopied;
                }
                FreeUpSpaceIfNeeded_ ();
                return intoBuffer.subspan (0, nCopied); // this can be empty on EOF
            }
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-label\"");
            DISABLE_COMPILER_MSC_WARNING_END (4102)
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ());
                Require (IsOpenWrite ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                size_t                      roomLeft     = distance (fWriteCursor_, fData_.end ());
                size_t                      roomRequired = elts.size ();
                if constexpr (kLocking_) {
                    fMoreDataWaiter_.Set (); // just means MAY be more data - readers check
                }
                if (roomLeft < roomRequired) {
                    size_t       curReadOffset  = distance (fData_.cbegin (), fReadCursor_);
                    size_t       curWriteOffset = distance (fData_.begin (), fWriteCursor_);
                    const size_t kChunkSize_    = 128; // WAG: @todo tune number...
                    Containers::Support::ReserveTweaks::Reserve4AddN (fData_, roomRequired - roomLeft, kChunkSize_);
                    fData_.resize (curWriteOffset + roomRequired);
                    fReadCursor_  = fData_.begin () + curReadOffset;
                    fWriteCursor_ = fData_.begin () + curWriteOffset;
                    Assert (fWriteCursor_ < fData_.end ());
                }
                copy (elts.data (), elts.data () + roomRequired, fWriteCursor_);
                fWriteCursor_ += roomRequired;
                Assert (fReadCursor_ < fData_.end ()); // < because we wrote at least one byte and that didnt move read cursor
                Assert (fWriteCursor_ <= fData_.end ());
            }
            virtual void Flush () override
            {
                // nothing todo - write 'writes thru'
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return fSpaceClearedFromStreamHead_ + distance (fData_.begin (), fReadCursor_);
            }
            virtual SeekOffsetType SeekRead ([[maybe_unused]] Whence whence, [[maybe_unused]] SignedSeekOffsetType offset) override
            {
                RequireNotReached ();
                return 0;
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Require (IsOpenWrite ());
                [[maybe_unused]] lock_guard critSec{fMutex_};
                return fSpaceClearedFromStreamHead_ +
                       std::distance (fData_.begin (), static_cast<typename vector<ElementType>::const_iterator> (fWriteCursor_));
            }
            virtual SeekOffsetType SeekWrite ([[maybe_unused]] Whence whence, [[maybe_unused]] SignedSeekOffsetType offset) override
            {
                RequireNotReached ();
                return 0;
            }
            // Private_::IRep_ overrides
            virtual Options GetOptions () const override
            {
                return Options{.fInternallySynchronized = same_as<LOCK_IMPL, recursive_mutex>
                                                              ? Execution::InternallySynchronized::eInternallySynchronized
                                                              : Execution::InternallySynchronized::eNotKnownInternallySynchronized,
                               .fSeekable               = false};
            }
            virtual vector<ElementType> AsVector () const override
            {
                RequireNotReached ();
                return {};
            }
            virtual string AsString () const override
            {
                RequireNotReached ();
                return {};
            }

        private:
            /*
             * Since the read stream is not seekable, anything before its read offset can be thrown away. Just adjust the reported 'seek offsets' so its not
             * clear to anyone this has happened.
             *
             * Also - given current data structures (could replace with ChunkedArray - maybe better) - costly to throw stuff away. So for now
             * only do if would save significant space.
             */
            nonvirtual void FreeUpSpaceIfNeeded_ ()
            {
                [[maybe_unused]] lock_guard critSec{fMutex_};
                Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                Assert (fReadCursor_ <= fWriteCursor_); // cuz cannot seek, and cannot read past where we've written so far
                constexpr size_t kMinData2Reclaim_ = 16 * 1024;
                size_t           elts2Reclaim      = distance (fData_.cbegin (), fReadCursor_);
                if (elts2Reclaim * sizeof (ELEMENT_TYPE) >= kMinData2Reclaim_ and IsOpenRead () and IsOpenWrite ()) [[unlikely]] {
                    size_t readOffset  = GetReadOffset ();
                    size_t writeOffset = GetWriteOffset ();
                    fData_.erase (fData_.begin (), fData_.begin () + elts2Reclaim);
                    fSpaceClearedFromStreamHead_ += elts2Reclaim;
                    Assert (readOffset == fSpaceClearedFromStreamHead_); // cuz always wrote more than read, and clear all that read
                    fReadCursor_  = fData_.begin () + (readOffset - fSpaceClearedFromStreamHead_);
                    fWriteCursor_ = fData_.begin () + (writeOffset - fSpaceClearedFromStreamHead_);
                    Assert (readOffset == GetReadOffset ());
                    Assert (writeOffset == GetWriteOffset ());
                }
            }

        private:
            [[no_unique_address]] mutable LOCK_IMPL fMutex_;
            size_t                                  fSpaceClearedFromStreamHead_{0};
            [[no_unique_address]] conditional_t<kLocking_, Execution::WaitableEvent, Configuration::Empty> fMoreDataWaiter_{}; // not a race cuz always set/reset when holding fMutex; no need to pre-set cuz auto set when someone adds data (Write)
            vector<ElementType>                          fData_; // Important data comes before cursors cuz of use in CTOR
            typename vector<ElementType>::const_iterator fReadCursor_;
            typename vector<ElementType>::iterator       fWriteCursor_;
            bool                                         fClosedForWrites_{false};
        };
    }

    /*
     ********************************************************************************
     ********************** SharedMemoryStream<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (Options options) -> Ptr<ELEMENT_TYPE>
    {
        // @todo - could do better on NullMutex stuff \see https://stroika.atlassian.net/browse/STK-584
        if (options.fSeekable) {
            return options.fInternallySynchronized == Execution::InternallySynchronized::eInternallySynchronized
                       ? Ptr<ELEMENT_TYPE>{make_shared<Private_::SeekableRep_<ELEMENT_TYPE, recursive_mutex>> ()}
                       : Ptr<ELEMENT_TYPE>{make_shared<Private_::SeekableRep_<ELEMENT_TYPE, Execution::NullMutex>> ()};
        }
        else {
            return options.fInternallySynchronized == Execution::InternallySynchronized::eInternallySynchronized
                       ? Ptr<ELEMENT_TYPE>{make_shared<Private_::UnseekableRep_<ELEMENT_TYPE, recursive_mutex>> ()}
                       : Ptr<ELEMENT_TYPE>{make_shared<Private_::UnseekableRep_<ELEMENT_TYPE, Execution::NullMutex>> ()};
        }
    }
    template <typename ELEMENT_TYPE, typename COPY_FROM>
    inline auto New (const COPY_FROM& copyFrom, Options options) -> Ptr<ELEMENT_TYPE>
        requires (same_as<ELEMENT_TYPE, byte> and Configuration::IAnyOf<COPY_FROM, Memory::BLOB, span<const ELEMENT_TYPE>>)
    {
        auto p = New<ELEMENT_TYPE> (options);
        p.Write (copyFrom);
        return p;
    }

    /*
     ********************************************************************************
     ****************** SharedMemoryStream::Ptr<ELEMENT_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::IRep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef_ () const -> const Private_::IRep_<ELEMENT_TYPE>&
    {
        return *Debug::UncheckedDynamicCast<const Private_::IRep_<ELEMENT_TYPE>*> (&inherited::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline Options Ptr<ELEMENT_TYPE>::GetOptions () const
    {
        return GetRepConstRef_ ().GetOptions ();
    }
    template <typename ELEMENT_TYPE>
    template <typename T>
    inline T Ptr<ELEMENT_TYPE>::As () const
        requires (same_as<T, vector<ELEMENT_TYPE>> or (same_as<ELEMENT_TYPE, byte> and Configuration::IAnyOf<T, Memory::BLOB, string>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<T, Characters::String>))
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
        return New (span<ELEMENT_TYPE>{start, end});
    }

}
