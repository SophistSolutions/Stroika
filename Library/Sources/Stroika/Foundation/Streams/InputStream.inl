/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika::Foundation {
    namespace Streams {

        /*
         ********************************************************************************
         *********************** InputStream<ELEMENT_TYPE>::_IRep ***********************
         ********************************************************************************
         */
        template <typename ELEMENT_TYPE>
        optional<size_t> InputStream<ELEMENT_TYPE>::_IRep::_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (ElementType* intoStart, ElementType* intoEnd, size_t elementsRemaining)
        {
            Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
            if (intoStart == nullptr) {
                return elementsRemaining;
            }
            else {
                return elementsRemaining == 0 ? 0 : Read (intoStart, intoEnd); // safe to call beacuse this cannot block - there are elements available
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
        inline void InputStream<ELEMENT_TYPE>::Ptr::Close () const
        {
            Require (IsOpen ());
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            _GetRepRWRef ().CloseRead ();
        }
        template <typename ELEMENT_TYPE>
        inline void InputStream<ELEMENT_TYPE>::Ptr::Close (bool reset)
        {
            lock_guard<AssertExternallySynchronizedLock> critSec{*this};
            _GetRepRWRef ().CloseRead ();
            if (reset) {
                this->reset ();
            }
        }
        template <typename ELEMENT_TYPE>
        inline bool InputStream<ELEMENT_TYPE>::Ptr::IsOpen () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            return _GetRepConstRef ().IsOpenRead ();
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
        inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffset () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            return _GetRepConstRef ().GetReadOffset ();
        }
        template <typename ELEMENT_TYPE>
        SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffsetToEndOfStream () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
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
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            Require (this->IsSeekable ());
            return _GetRepRWRef ().SeekRead (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
        }
        template <typename ELEMENT_TYPE>
        inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::Seek (Whence whence, SignedSeekOffsetType offset) const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            Require (this->IsSeekable ());
            return _GetRepRWRef ().SeekRead (whence, offset);
        }
        template <typename ELEMENT_TYPE>
        inline auto InputStream<ELEMENT_TYPE>::Ptr::Read () const -> optional<ElementType>
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
            ElementType b{};
            return (_GetRepRWRef ().Read (&b, &b + 1) == 0) ? optional<ElementType> () : b;
        }
        template <typename ELEMENT_TYPE>
        inline size_t InputStream<ELEMENT_TYPE>::Ptr::Read (ElementType* intoStart, ElementType* intoEnd) const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
            RequireNotNull (intoStart);
            Require ((intoEnd - intoStart) >= 1);
            return _GetRepRWRef ().Read (intoStart, intoEnd);
        }
        template <typename ELEMENT_TYPE>
        auto InputStream<ELEMENT_TYPE>::Ptr::Peek () const -> optional<ElementType>
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (this->IsSeekable ());
            Require (IsOpen ());
            SeekOffsetType saved  = GetOffset ();
            auto           result = this->Read ();
            this->Seek (saved);
            return result;
        }
        template <typename ELEMENT_TYPE>
        size_t InputStream<ELEMENT_TYPE>::Ptr::Peek (ElementType* intoStart, ElementType* intoEnd) const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (this->IsSeekable ());
            Require (IsOpen ());
            SeekOffsetType saved  = GetOffset ();
            auto           result = this->Read (intoStart, intoEnd);
            this->Seek (saved);
            return result;
        }
        template <typename ELEMENT_TYPE>
        inline bool InputStream<ELEMENT_TYPE>::Ptr::IsAtEOF () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            return not Peek ().has_value ();
        }
        template <typename ELEMENT_TYPE>
        inline optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            return _GetRepRWRef ().ReadNonBlocking (nullptr, nullptr);
        }
        template <typename ELEMENT_TYPE>
        inline optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            RequireNotNull (intoStart);
            Require ((intoEnd - intoStart) >= 1);
            Require (IsOpen ());
            return _GetRepRWRef ().ReadNonBlocking (intoStart, intoEnd);
        }
        template <typename ELEMENT_TYPE>
        template <typename POD_TYPE, typename TEST_TYPE, typename ENABLE_IF_TEST>
        POD_TYPE InputStream<ELEMENT_TYPE>::Ptr::ReadRaw () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
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
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpen ());
            static_assert (std::is_pod<POD_TYPE>::value, "");
            size_t n{ReadAll (reinterpret_cast<Memory::Byte*> (start), reinterpret_cast<Memory::Byte*> (end))};
            if (n != sizeof (POD_TYPE) * (end - start)) {
                Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
            }
        }
        template <typename ELEMENT_TYPE>
        size_t InputStream<ELEMENT_TYPE>::Ptr::ReadAll (ElementType* intoStart, ElementType* intoEnd) const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            RequireNotNull (intoStart);
            Require ((intoEnd - intoStart) >= 1);
            Require (IsOpen ());
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

        /*
         ********************************************************************************
         *************************** InputStream<ELEMENT_TYPE> **************************
         ********************************************************************************
         */
        template <typename ELEMENT_TYPE>
        inline auto InputStream<ELEMENT_TYPE>::_mkPtr (const _SharedIRep& s) -> Ptr
        {
            return Ptr{s};
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
