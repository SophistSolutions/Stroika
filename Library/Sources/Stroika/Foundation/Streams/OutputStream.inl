/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_OutputStream_inl_
#define _Stroika_Foundation_Streams_OutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Characters/LineEndings.h"
#include "../Characters/String.h"
#include "../Debug/Assertions.h"
#include "../Memory/BLOB.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ************************ OutputStream<ELEMENT_TYPE>::Ptr ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline OutputStream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep)
        : inherited (rep)
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline OutputStream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t)
        : inherited (nullptr)
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto OutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return dynamic_pointer_cast<_IRep> (inherited::_GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto OutputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        // reinterpret_cast faster than dynamic_cast - check equivilent
        Assert (dynamic_cast<const _IRep*> (&inherited::_GetRepConstRef ()) == reinterpret_cast<const _IRep*> (&inherited::_GetRepConstRef ()));
        return *reinterpret_cast<const _IRep*> (&inherited::_GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto OutputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        // reinterpret_cast faster than dynamic_cast - check equivilent
        Assert (dynamic_cast<_IRep*> (&inherited::_GetRepRWRef ()) == reinterpret_cast<_IRep*> (&inherited::_GetRepRWRef ()));
        return *reinterpret_cast<_IRep*> (&inherited::_GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType OutputStream<ELEMENT_TYPE>::Ptr::GetOffset () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        return _GetRepConstRef ().GetWriteOffset ();
    }
    template <typename ELEMENT_TYPE>
    SeekOffsetType OutputStream<ELEMENT_TYPE>::Ptr::GetOffsetToEndOfStream () const
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
    inline SeekOffsetType OutputStream<ELEMENT_TYPE>::Ptr::Seek (SeekOffsetType offset) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        return _GetRepRWRef ().SeekWrite (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType OutputStream<ELEMENT_TYPE>::Ptr::Seek (Whence whence, SignedSeekOffsetType offset) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        return _GetRepRWRef ().SeekWrite (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Write (const ElementType* start, const ElementType* end) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        Require (start <= end);
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
            _GetRepRWRef ().Write (start, end);
        }
    }
    template <typename ELEMENT_TYPE>
    template <typename T, enable_if_t<is_same_v<byte, T>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Write (const uint8_t* start, const uint8_t* end) const
    {
        Write (reinterpret_cast<const std::byte*> (start), reinterpret_cast<const std::byte*> (end));
    }
    template <typename ELEMENT_TYPE>
    template <typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, byte>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Write (const Memory::BLOB& blob) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        Write (blob.begin (), blob.end ());
    }
    template <typename ELEMENT_TYPE>
    template <typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Write (const wchar_t* cStr) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        Write (cStr, cStr + ::wcslen (cStr));
    }
    template <typename ELEMENT_TYPE>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Write (const ElementType& e) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        Write (&e, &e + 1);
    }
    template <typename ELEMENT_TYPE>
    template <typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::WriteLn (const wchar_t* cStr) const
    {
        Write (Characters::String (cStr));
    }
    template <typename ELEMENT_TYPE>
    template <typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, Characters::Character>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::WriteLn (const Characters::String& s) const
    {
        Write (s + Characters::GetEOL<wchar_t> ());
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE, typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, byte>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::WriteRaw (const POD_TYPE& p) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        WriteRaw (&p, &p + 1);
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE, typename TEST_TYPE, enable_if_t<is_same_v<TEST_TYPE, byte>>*>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::WriteRaw (const POD_TYPE* start, const POD_TYPE* end) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        static_assert (is_pod_v<POD_TYPE>);
        Write (reinterpret_cast<const byte*> (start), reinterpret_cast<const byte*> (end));
    }
    template <typename ELEMENT_TYPE>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Close () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        _GetRepRWRef ().CloseWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Close (bool reset)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpen ());
        _GetRepRWRef ().CloseWrite ();
        if (reset) {
            this->reset ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline bool OutputStream<ELEMENT_TYPE>::Ptr::IsOpen () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _GetRepConstRef ().IsOpenWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void OutputStream<ELEMENT_TYPE>::Ptr::Flush () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _GetRepRWRef ().Flush ();
    }
    template <>
    template <>
    inline const OutputStream<Characters::Character>::Ptr& OutputStream<Characters::Character>::Ptr::operator<< (const Characters::String& write2TextStream) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Write (write2TextStream);
        return *this;
    }
    template <>
    template <>
    inline const OutputStream<Characters::Character>::Ptr& OutputStream<Characters::Character>::Ptr::operator<< (const wchar_t* write2TextStream) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Write (write2TextStream);
        return *this;
    }

    /*
     ********************************************************************************
     *************************** OutputStream<ELEMENT_TYPE> *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto OutputStream<ELEMENT_TYPE>::_mkPtr (const _SharedIRep& s) -> Ptr
    {
        return Ptr{s};
    }

}

#endif /*_Stroika_Foundation_Streams_OutputStream_inl_*/
