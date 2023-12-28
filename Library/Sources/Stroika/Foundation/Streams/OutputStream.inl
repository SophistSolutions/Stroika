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
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep)
        : inherited{rep}
    {
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetSharedRep () const -> shared_ptr<IRep<ELEMENT_TYPE>>
    {
        return Debug::UncheckedDynamicPointerCast<IRep<ELEMENT_TYPE>> (inherited::GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef () const -> const IRep<ELEMENT_TYPE>&
    {
        return Debug::UncheckedDynamicCast<const IRep<ELEMENT_TYPE>&> (inherited::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepRWRef () const -> IRep<ELEMENT_TYPE>&
    {
        return Debug::UncheckedDynamicCast<IRep<ELEMENT_TYPE>&> (inherited::GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetOffset () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        return GetRepConstRef ().GetWriteOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::Seek (SeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        return GetRepRWRef ().SeekWrite (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        return GetRepRWRef ().SeekWrite (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    template <typename ELEMENT_TYPE2, size_t EXTENT_2>
    inline void Ptr<ELEMENT_TYPE>::Write (span<const ELEMENT_TYPE2, EXTENT_2> elts) const
        requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                  (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and (Characters::IUNICODECanUnambiguouslyConvertFrom<remove_cvref_t<ELEMENT_TYPE2>>)))
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        Require (not elts.empty ());
        if constexpr (same_as<ELEMENT_TYPE, byte>) {
            GetRepRWRef ().Write (Memory::SpanReInterpretCast<const ELEMENT_TYPE> (elts));
        }
        else if constexpr (same_as<ELEMENT_TYPE, Characters::Character>) {
            if constexpr (sizeof (ELEMENT_TYPE2) == sizeof (Characters::Character)) {
                GetRepRWRef ().Write (Memory::SpanReInterpretCast<const ELEMENT_TYPE> (elts));
            }
            else {
                Memory::StackBuffer<Characters::Character> buf{Memory::eUninitialized,
                                                               Characters::UTFConvert::ComputeTargetBufferSize<Characters::Character> (elts)};
                GetRepRWRef ().Write (Characters::UTFConvert::kThe.ConvertSpan (elts, span{buf}));
            }
        }
        else {
            GetRepRWRef ().Write (elts);
        }
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const Memory::BLOB& blob) const
        requires (same_as<ELEMENT_TYPE, byte>)
    {
        this->Write (blob.As<span<const byte>> ());
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Write (const ELEMENT_TYPE& e) const
    {
        this->Write (span{&e, 1});
    }
    template <typename ELEMENT_TYPE>
    void Ptr<ELEMENT_TYPE>::Write (const Characters::String& s) const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
    {
        Memory::StackBuffer<Characters::Character> ignored;
        this->Write (s.GetData<Characters::Character> (&ignored));
    }
    template <typename ELEMENT_TYPE>
    template <Characters::IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void Ptr<ELEMENT_TYPE>::Write (const CHAR_T* cStr) const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
    {
        this->Write (span{cStr, Characters::CString::Length (cStr)});
    }
    template <typename ELEMENT_TYPE>
    template <typename ELT_2_WRITE>
    inline void Ptr<ELEMENT_TYPE>::WriteLn (ELT_2_WRITE&& arg) const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
    {
        this->Write (forward<ELT_2_WRITE> (arg));
        this->Write (Characters::GetEOL<Characters::Character> ());
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteRaw (const POD_TYPE& p) const
        requires (is_same_v<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        this->WriteRaw (span{&p, 1});
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline void Ptr<ELEMENT_TYPE>::WriteRaw (span<const POD_TYPE> elts) const
        requires (is_same_v<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        this->Write (as_bytes (elts));
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        GetRepRWRef ().CloseWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Close (bool reset)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        GetRepRWRef ().CloseWrite ();
        if (reset) {
            this->reset ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpen () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return GetRepConstRef ().IsOpenWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Flush () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        GetRepRWRef ().Flush ();
    }
    template <typename ELEMENT_TYPE>
    template <typename T>
    inline const typename OutputStream::Ptr<ELEMENT_TYPE>& Ptr<ELEMENT_TYPE>::operator<< (const T& write2TextStream) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Write (write2TextStream);
        return *this;
    }

}

#endif /*_Stroika_Foundation_Streams_OutputStream_inl_*/
