/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_inl_
#define _Stroika_Foundation_Streams_InputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Debug/Assertions.h"
#include "../Memory/BLOB.h"
#include "EOFException.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             *********************** InputStream<ELEMENT_TYPE>::_IRep ***********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            Memory::Optional<size_t> InputStream<ELEMENT_TYPE>::_IRep::_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (ElementType* intoStart, ElementType* intoEnd, size_t elementsReamining)
            {
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                if (intoStart == nullptr) {
                    return elementsReamining;
                }
                else {
                    return elementsReamining == 0 ? 0 : Read (intoStart, intoEnd); // safe to call beacuse this cannot block - there are elements available
                }
            }

            /*
             ********************************************************************************
             *********************** InputStream<ELEMENT_TYPE>::Ptr *************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InputStream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep)
                : inherited (rep)
            {
                RequireNotNull (rep);
            }
            template <typename ELEMENT_TYPE>
            inline InputStream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t)
                : inherited (nullptr)
            {
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (inherited::_GetSharedRep ());
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
            {
                EnsureMember (&inherited::_GetRepConstRef (), _IRep);
                return *reinterpret_cast<const _IRep*> (&inherited::_GetRepConstRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
            {
                EnsureMember (&inherited::_GetRepRWRef (), _IRep);
                return *reinterpret_cast<_IRep*> (&inherited::_GetRepRWRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline typename InputStream<ELEMENT_TYPE>::Ptr InputStream<ELEMENT_TYPE>::Ptr::Synchronized () const
            {
                struct SyncRep_ : public InputStream<ELEMENT_TYPE>::_IRep {
                public:
                    SyncRep_ (const InputStream<ELEMENT_TYPE>::Ptr& realIn)
                        : InputStream<ELEMENT_TYPE>::Ptr::_IRep ()
                        , fCriticalSection_ ()
                        , fRealIn_ (realIn)
                    {
                    }
                    virtual bool IsSeekable () const override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.IsSeekable ();
                    }
                    virtual SeekOffsetType GetReadOffset () const override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.GetReadOffset ();
                    }
                    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.SeekRead (whence, offset);
                    }
                    virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        using Execution::make_unique_lock;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.Read (intoStart, intoEnd);
                    }
                    virtual Memory::Optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
                    {
                        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                        using Execution::make_unique_lock;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
                    }

                private:
                    mutable mutex                  fCriticalSection_;
                    InputStream<ELEMENT_TYPE>::Ptr fRealIn_;
                };
                return InputStream<ELEMENT_TYPE>::Ptr (make_shared<SyncRep_> (*this));
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffset () const
            {
                return _GetRepConstRef ().GetReadOffset ();
            }
            template <typename ELEMENT_TYPE>
            SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffsetToEndOfStream () const
            {
                SeekOffsetType savedReadFrom = GetOffset ();
                SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
                Seek (Whence::eFromStart, savedReadFrom);
                Assert (size >= savedReadFrom);
                size -= savedReadFrom;
                return size;
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::Seek (SeekOffsetType offset) const
            {
                Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
                return _GetRepRWRef ().SeekRead (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                return _GetRepRWRef ().SeekRead (whence, offset);
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::Ptr::Read () const -> Memory::Optional<ElementType>
            {
                ElementType b{};
                RequireNotNull (_GetSharedRep ());
                return (_GetRepRWRef ().Read (&b, &b + 1) == 0) ? Memory::Optional<ElementType> () : b;
            }
            template <typename ELEMENT_TYPE>
            inline size_t InputStream<ELEMENT_TYPE>::Ptr::Read (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetSharedRep ().get ());
                return _GetRepRWRef ().Read (intoStart, intoEnd);
            }
            template <typename ELEMENT_TYPE>
            auto InputStream<ELEMENT_TYPE>::Ptr::Peek () const -> Memory::Optional<ElementType>
            {
                Require (this->IsSeekable ());
                SeekOffsetType saved  = GetOffset ();
                auto           result = this->Read ();
                this->Seek (saved);
                return result;
            }
            template <typename ELEMENT_TYPE>
            size_t InputStream<ELEMENT_TYPE>::Ptr::Peek (ElementType* intoStart, ElementType* intoEnd) const
            {
                Require (this->IsSeekable ());
                SeekOffsetType saved  = GetOffset ();
                auto           result = this->Read (intoStart, intoEnd);
                this->Seek (saved);
                return result;
            }
            template <typename ELEMENT_TYPE>
            inline bool InputStream<ELEMENT_TYPE>::Ptr::IsAtEOF () const
            {
                return Peek ().IsMissing ();
            }
            template <typename ELEMENT_TYPE>
            inline Memory::Optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking () const
            {
                RequireNotNull (_GetSharedRep ().get ());
                return _GetRepRWRef ().ReadNonBlocking (nullptr, nullptr);
            }
            template <typename ELEMENT_TYPE>
            inline Memory::Optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetSharedRep ().get ());
                return _GetRepRWRef ().ReadNonBlocking (intoStart, intoEnd);
            }
            template <typename ELEMENT_TYPE>
            template <typename POD_TYPE, typename TEST_TYPE, typename ENABLE_IF_TEST>
            POD_TYPE InputStream<ELEMENT_TYPE>::Ptr::ReadRaw () const
            {
                static_assert (std::is_pod<POD_TYPE>::value, "");
                POD_TYPE tmp; // intentionally don't zero-initialize
                size_t   n{ReadAll (reinterpret_cast<Memory::Byte*> (&tmp), reinterpret_cast<Memory::Byte*> (&tmp + 1))};
                if (n == sizeof (tmp)) {
                    return tmp;
                }
                else {
                    Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
                }
            }
            template <typename ELEMENT_TYPE>
            template <typename POD_TYPE, typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline void InputStream<ELEMENT_TYPE>::Ptr::ReadRaw (POD_TYPE* start, POD_TYPE* end) const
            {
                static_assert (std::is_pod<POD_TYPE>::value, "");
                size_t n{ReadAll (reinterpret_cast<Memory::Byte*> (start), reinterpret_cast<Memory::Byte*> (end))};
                if (n != sizeof (POD_TYPE) * (end - start)) {
                    Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
                }
            }
            template <typename ELEMENT_TYPE>
            size_t InputStream<ELEMENT_TYPE>::Ptr::ReadAll (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetSharedRep ().get ());
                size_t elementsRead{};
                for (ElementType* readCursor = intoStart; readCursor < intoEnd;) {
                    size_t eltsReadThisTime = Read (readCursor, intoEnd);
                    Assert (eltsReadThisTime <= static_cast<size_t> (intoEnd - readCursor));
                    if (eltsReadThisTime == 0) {
                        // irrevocable EOF
                        break;
                    }
                    elementsRead += eltsReadThisTime;
                    readCursor += eltsReadThisTime;
                }
                return elementsRead;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
