/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_inl_
#define _Stroika_Foundation_Streams_MemoryStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Debug/Cast.h"

namespace Stroika::Foundation::Streams::MemoryStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputOutputStream::IRep<ELEMENT_TYPE> {
        public:
            using ElementType = ELEMENT_TYPE;

        private:
            bool fOpenRead_{true};
            bool fOpenWrite_{true};

        public:
            Rep_ ()
                : fReadCursor_{fData_.begin ()}
                , fWriteCursor_{fData_.begin ()}
            {
            }
            Rep_ (const Rep_&)                       = delete;
            nonvirtual Rep_& operator= (const Rep_&) = delete;

            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fOpenWrite_ = false;
            }
            virtual bool IsOpenWrite () const override
            {
                return fOpenWrite_;
            }
            virtual void CloseRead () override
            {
                Require (IsOpenRead ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fOpenRead_ = false;
            }
            virtual bool IsOpenRead () const override
            {
                return fOpenRead_;
            }
            virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                Require (IsOpenRead ());
                size_t                                                 nRequested = intoEnd - intoStart;
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                size_t nAvail  = fData_.end () - fReadCursor_;
                size_t nCopied = min (nAvail, nRequested);
                {
                    copy (fReadCursor_, fReadCursor_ + nCopied, intoStart);
                    fReadCursor_ = fReadCursor_ + nCopied;
                }
                return nCopied; // this can be zero iff EOF
            }
            virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return this->_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fData_.end () - fReadCursor_);
            }
            virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
            {
                Require (start != nullptr or start == end);
                Require (end != nullptr or start == end);
                Require (IsOpenWrite ());
                // @todo - rewrite so does in one copy - no idea why this code does multiple copies! IF it makes sense DOCUMENT why...--LGP 2023-12-18
                if (start != end) {
                    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                    size_t                                                 roomLeft     = fData_.end () - fWriteCursor_;
                    size_t                                                 roomRequired = end - start;
                    if (roomLeft < roomRequired) {
                        size_t       curReadOffset  = fReadCursor_ - fData_.begin ();
                        size_t       curWriteOffset = fWriteCursor_ - fData_.begin ();
                        const size_t kChunkSize_    = 128; // WAG: @todo tune number...
                        Containers::Support::ReserveTweaks::Reserve4AddN (fData_, roomRequired - roomLeft, kChunkSize_);
                        fData_.resize (curWriteOffset + roomRequired); // fixup cursors after any possible realloc of fData_
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
                Require (IsOpenWrite ());
                // nothing todo - write 'writes thru'
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fReadCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                        if (uOffset > fData_.size ()) [[unlikely]] {
                            Execution::Throw (EOFException::kThe);
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
                            Execution::Throw (EOFException::kThe);
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
                            Execution::Throw (EOFException::kThe);
                        }
                        fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                    } break;
                }
                Ensure ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                return fReadCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                return fWriteCursor_ - fData_.begin ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        if (static_cast<SeekOffsetType> (offset) > fData_.size ()) [[unlikely]] {
                            Execution::Throw (EOFException::kThe);
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
                            Execution::Throw (EOFException::kThe);
                        }
                        fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = fData_.size () + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kSeekException_);
                        }
                        if (static_cast<size_t> (newOffset) > fData_.size ()) [[unlikely]] {
                            Execution::Throw (EOFException::kThe);
                        }
                        fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                    } break;
                }
                Ensure ((fData_.begin () <= fWriteCursor_) and (fWriteCursor_ <= fData_.end ()));
                return fWriteCursor_ - fData_.begin ();
            }
            vector<ElementType> AsVector () const
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fData_;
            }
            string AsString () const
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return string{reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_))};
            }

        private:
            // @todo - COULD redo using
            //      constexpr size_t  USE_BUFFER_BYTES = 1024 - sizeof(recursive_mutex) - sizeof(byte*) - sizeof (BinaryInputStream::_IRep) - sizeof (Seekable::_IRep);
            //      Memory::InlineBuffer<byte,USE_BUFFER_BYTES>  fData_;
            // Or Stroika chunked array code

        private:
            static inline const auto kSeekException_ = range_error{"seek"};
            vector<ElementType>      fData_; // subtle, but important data declared before cursors for initialization CTOR sake
            typename vector<ElementType>::iterator                         fReadCursor_;
            typename vector<ElementType>::iterator                         fWriteCursor_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     **************************** MemoryStream<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New () -> Ptr<ELEMENT_TYPE>
    {
        using MemoryStream::Private_::Rep_;
        return make_shared<Rep_<ELEMENT_TYPE>> ();
    }
    template <typename ELEMENT_TYPE>
    inline auto New (span<const ELEMENT_TYPE> copyFrom) -> Ptr<ELEMENT_TYPE>
    {
        using MemoryStream::Private_::Rep_;
        Ptr r = make_shared<Rep_<ELEMENT_TYPE>> ();
        r.Write (copyFrom);
        return r;
    }
    template <typename ELEMENT_TYPE>
    inline auto New (const Memory::BLOB& copyFrom) -> Ptr<ELEMENT_TYPE>
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        using MemoryStream::Private_::Rep_;
        Ptr r = make_shared<Rep_<ELEMENT_TYPE>> ();
        r.Write (copyFrom);
        return r;
    }

    /*
     ********************************************************************************
     *********************** MemoryStream::Ptr<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::Rep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef_ () const -> const Private_::Rep_<ELEMENT_TYPE>&
    {
        using Rep_ = typename MemoryStream::Private_::Rep_<ELEMENT_TYPE>;
        return *Debug::UncheckedDynamicCast<const Rep_*> (&inherited::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepRWRef_ () const -> Private_::Rep_<ELEMENT_TYPE>&
    {
        using Rep_ = typename MemoryStream::Private_::Rep_<ELEMENT_TYPE>;
        return *Debug::UncheckedDynamicCast<Rep_*> (&inherited::GetRepRWRef ());
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

    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d5 use span interface")]] static Ptr<ELEMENT_TYPE> New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
    {
        return make_shared<Private_::Rep_<ELEMENT_TYPE>> (start, end);
    }

}

#endif /*_Stroika_Foundation_Streams_MemoryStream_inl_*/
