/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_OutputStream_inl_
#define _Stroika_Foundation_Streams_OutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Characters/Format.h"
#include "../Characters/LineEndings.h"
#include "../Characters/String.h"
#include "../Debug/Assertions.h"
#include "../Debug/Cast.h"
#include "../Memory/BLOB.h"

namespace Stroika::Foundation::Streams::OutputStream {

    /*
     ********************************************************************************
     ************************ OutputStream::Ptr<ELEMENT_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<_IRep<ELEMENT_TYPE>>& rep)
        : inherited{rep}
    {
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetSharedRep () const -> shared_ptr<_IRep<ELEMENT_TYPE>>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return Debug::UncheckedDynamicPointerCast<_IRep<ELEMENT_TYPE>> (inherited::_GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepConstRef () const -> const _IRep<ELEMENT_TYPE>&
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        // reinterpret_cast faster than dynamic_cast - check equivalent
        Assert (dynamic_cast<const _IRep<ELEMENT_TYPE>*> (&inherited::_GetRepConstRef ()) ==
                reinterpret_cast<const _IRep<ELEMENT_TYPE>*> (&inherited::_GetRepConstRef ()));
        return *reinterpret_cast<const _IRep<ELEMENT_TYPE>*> (&inherited::_GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepRWRef () const -> _IRep<ELEMENT_TYPE>&
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        // reinterpret_cast faster than dynamic_cast - check equivalent
        Assert (dynamic_cast<_IRep<ELEMENT_TYPE>*> (&inherited::_GetRepRWRef ()) ==
                reinterpret_cast<_IRep<ELEMENT_TYPE>*> (&inherited::_GetRepRWRef ()));
        return *reinterpret_cast<_IRep<ELEMENT_TYPE>*> (&inherited::_GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetOffset () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        return _GetRepConstRef ().GetWriteOffset ();
    }
    template <typename ELEMENT_TYPE>
    SeekOffsetType Ptr<ELEMENT_TYPE>::GetOffsetToEndOfStream () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        SeekOffsetType savedReadFrom = GetOffset ();
        SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
        Seek (Whence::eFromStart, savedReadFrom);
        Assert (size >= savedReadFrom);
        size -= savedReadFrom;
        return size;
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::Seek (SeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        return _GetRepRWRef ().SeekWrite (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        return _GetRepRWRef ().SeekWrite (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Require (start <= end);
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
            _GetRepRWRef ().Write (start, end);
        }
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const uint8_t* start, const uint8_t* end) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Write (reinterpret_cast<const byte*> (start), reinterpret_cast<const byte*> (end));
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const Memory::BLOB& blob) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Write (blob.begin (), blob.end ());
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const wchar_t* cStr) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Write (cStr, cStr + ::wcslen (cStr));
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const ELEMENT_TYPE& e) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Write (&e, &e + 1);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (span<const ELEMENT_TYPE> s) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        if (not s.empty ()) {
            Write (s.data (), s.data () + s.size ());
        }
    }
    template <typename ELEMENT_TYPE>
    void Ptr<ELEMENT_TYPE>::Write (const Characters::String& s) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        // @todo performance tweek so uses Peek
        Memory::StackBuffer<wchar_t> ignored;
        span<const wchar_t>          sp = s.GetData<wchar_t> (&ignored);
        Write (sp.data (), sp.data () + sp.size ());
    }
    template <typename ELEMENT_TYPE>
    void Ptr<ELEMENT_TYPE>::Write (const wchar_t* start, const wchar_t* end) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        if constexpr (sizeof (wchar_t) == sizeof (Characters::Character)) {
            Write (reinterpret_cast<const Characters::Character*> (start), reinterpret_cast<const Characters::Character*> (end));
        }
        else {
            Memory::StackBuffer<Characters::Character> buf{
                Memory::eUninitialized, Characters::UTFConvert::ComputeTargetBufferSize<Characters::Character> (span{start, end})};
            span<Characters::Character> writeSpan = Characters::UTFConvert::kThe.ConvertSpan (span{start, end}, span{buf});
            Write (writeSpan.data (), writeSpan.data () + writeSpan.size ());
        }
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteLn (const wchar_t* cStr) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Write (Characters::String{cStr});
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteLn (const Characters::String& s) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Write (s + Characters::GetEOL<wchar_t> ());
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteRaw (const POD_TYPE& p) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        WriteRaw (&p, &p + 1);
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteRaw (const POD_TYPE* start, const POD_TYPE* end) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        static_assert (is_trivially_copyable_v<POD_TYPE> and is_standard_layout_v<POD_TYPE>);
        Write (reinterpret_cast<const byte*> (start), reinterpret_cast<const byte*> (end));
    }
    template <typename ELEMENT_TYPE>
    void Ptr<ELEMENT_TYPE>::PrintF (const wchar_t* format, ...)
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        RequireNotNull (format);
        va_list argsList;
        va_start (argsList, format);
        Write (Characters::FormatV (format, argsList));
        va_end (argsList);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Close () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        _GetRepRWRef ().CloseWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Close (bool reset)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (IsOpen ());
        _GetRepRWRef ().CloseWrite ();
        if (reset) {
            this->reset ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpen () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _GetRepConstRef ().IsOpenWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Flush () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _GetRepRWRef ().Flush ();
    }
    template <typename ELEMENT_TYPE>
    template <typename T>
    inline const typename OutputStream::Ptr<ELEMENT_TYPE>& Ptr<ELEMENT_TYPE>::operator<< (const T& write2TextStream) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Write (write2TextStream);
        return *this;
    }

}

#endif /*_Stroika_Foundation_Streams_OutputStream_inl_*/
