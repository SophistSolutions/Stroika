/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_ 1

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Traversal/Iterator.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ************** Streams::ExternallyOwnedMemoryInputStream::Rep_ *****************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    class ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep {
    public:
        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
            : fStart_{start}
            , fEnd_{end}
            , fCursor_{start}
        {
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        bool fIsOpenForRead_{true};

    protected:
        virtual bool IsSeekable () const override { return true; }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            fIsOpenForRead_ = false;
        }
        virtual bool   IsOpenRead () const override { return fIsOpenForRead_; }
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

    /*
     ********************************************************************************
     ********** Streams::ExternallyOwnedMemoryInputStream<ELEMENT_TYPE> *************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) -> Ptr
    {
        return inherited::_mkPtr (make_shared<Rep_> (start, end));
    }
    template <typename ELEMENT_TYPE>
    template <random_access_iterator ELEMENT_ITERATOR>
    inline auto ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::New (ELEMENT_ITERATOR start, ELEMENT_ITERATOR end) -> Ptr
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>)
    {
        // note, because its not LEGAL to dereference end (and visual studio checks) - even though we dont REALLY dereference and use value -
        // just compute start and add end-start
        if constexpr (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>) {
            return New (reinterpret_cast<const byte*> (Traversal::Iterator2Pointer (start)),
                        reinterpret_cast<const byte*> (Traversal::Iterator2Pointer (start) + (end - start)));
        }
        else {
            return New (static_cast<const ELEMENT_TYPE*> (Traversal::Iterator2Pointer (start)),
                        static_cast<const ELEMENT_TYPE*> (Traversal::Iterator2Pointer (start) + (end - start)));
        }
    }
    template <typename ELEMENT_TYPE>
    inline auto ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::New (Execution::InternallySynchronized internallySynchronized,
                                                                     const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) -> Ptr
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternalSyncRep_::New (start, end);
            case Execution::eNotKnownInternallySynchronized:
                return New (start, end);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }
    template <typename ELEMENT_TYPE>
    template <random_access_iterator ELEMENT_ITERATOR>
    inline auto ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::New (Execution::InternallySynchronized internallySynchronized,
                                                                     ELEMENT_ITERATOR start, ELEMENT_ITERATOR end) -> Ptr
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>)
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternalSyncRep_::New (start, end);
            case Execution::eNotKnownInternallySynchronized:
                return New (start, end);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }
    template <typename ELEMENT_TYPE>
    auto ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::New (const uint8_t* start, const uint8_t* end) -> Ptr
        requires is_same_v<ELEMENT_TYPE, byte>
    {
        return New (reinterpret_cast<const byte*> (start), reinterpret_cast<const std::byte*> (end));
    }

}

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_*/
