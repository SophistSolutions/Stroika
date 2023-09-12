/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_inl_
#define _Stroika_Foundation_Characters_StringBuilder_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <memory>

#include "../Debug/Assertions.h"
#include "../Execution/Common.h"
#include "../Memory/Optional.h"

#include "CString/Utilities.h"
#include "UTFConvert.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::StringBuilder (const String& initialValue)
    {
        Append (initialValue);
    }
    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline StringBuilder<OPTIONS>::StringBuilder (span<const CHAR_T> initialValue)
    {
        Append (initialValue);
    }
    template <typename OPTIONS>
    inline auto StringBuilder<OPTIONS>::operator= (const String& rhs) -> StringBuilder&
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        clear ();
        Append (rhs);
        return *this;
    }

    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (span<const CHAR_T> s)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        size_t                                                 spanSize = s.size ();
        if (spanSize != 0) {
            if constexpr (is_same_v<CHAR_T, ASCII>) {
                Character::CheckASCII (s);
            }
            if constexpr (sizeof (CHAR_T) == sizeof (BufferElementType)) {
                size_t i = fData_.size ();
                fData_.GrowToSize_uninitialized (i + spanSize);
                if constexpr (is_same_v<CHAR_T, BufferElementType>) {
                    Memory::CopySpanData_StaticCast (s, span<CHAR_T>{fData_}.subspan (i));
                }
                else {
                    Memory::CopySpanData_StaticCast (Memory::SpanReInterpretCast<const BufferElementType> (s),
                                                     span<BufferElementType>{fData_}.subspan (i));
                }
            }
            else {
                // @todo  OPTIMIZATION OPPORTUNITY - if given an ascii span, can just do those chars one at a time...
                // Walk through span, and if can definitely short-cut, do so, and when we first find we cannot, convert the balance of the span
                // this slower way.
                // If char is ASCII, it can always be copied as-is
                // if BufferElementType != char8_t, then we can always just blindly copy up to 0xff (Latin1).
                // If BufferElementType (size) == char16_t, ... well - more complicated here - these cases due to surroages.
                auto charITodoHardWay = s.begin ();
                for (; charITodoHardWay != s.end (); ++charITodoHardWay) {
                    // for now - KISS - and just check ASCII case
                    if (not Character::IsASCII (*charITodoHardWay)) [[unlikely]] {
                        break;
                    }
                }
                if (s.begin () != charITodoHardWay) [[likely]] {
                    if constexpr (same_as<CHAR_T, Character>) {
                        for (auto c : s.subspan (0, charITodoHardWay - s.begin ())) {
                            this->fData_.push_back (static_cast<ASCII> (c.GetCharacterCode ()));
                        }
                    }
                    else {
                        this->fData_.push_back_coerced (s.subspan (0, charITodoHardWay - s.begin ()));
                    }
                }
                if (charITodoHardWay != s.end ()) [[unlikely]] {
                    auto                                   hardWaySpan = span{charITodoHardWay, s.end ()};
                    Memory::StackBuffer<BufferElementType> buf{Memory::eUninitialized,
                                                               UTFConvert::ComputeTargetBufferSize<BufferElementType> (hardWaySpan)};
                    Append (UTFConvert::kThe.ConvertSpan (hardWaySpan, span{buf}));
                }
            }
        }
    }
    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (span<CHAR_T> s)
    {
        Append (Memory::ConstSpan (s));
    }
    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const CHAR_T* s)
    {
        Append (span{s, CString::Length (s)});
    }
    template <typename OPTIONS>
    template <IStdBasicStringCompatibleCharacter CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const basic_string<CHAR_T>& s)
        requires (IUNICODECanUnambiguouslyConvertFrom<CHAR_T>)
    {
        Append (span{s});
    }
    template <typename OPTIONS>
    template <IStdBasicStringCompatibleCharacter CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const basic_string_view<CHAR_T>& s)
        requires (IUNICODECanUnambiguouslyConvertFrom<CHAR_T>)
    {
        Append (span{s});
    }
    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::Append (const String& s)
    {
        Memory::StackBuffer<BufferElementType> ignored; // could use char8_t maybe here - optimizing for ASCII case or BufferElementType
        span                                   p = s.GetData (&ignored);
        if (not p.empty ()) {
            Append (p);
        }
    }
    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline
#if defined(_MSC_VER)
        // Makes significant differnce in JSON parser runtime with vs2k 17.4.3
        __forceinline
#endif
        void
        StringBuilder<OPTIONS>::Append (CHAR_T c)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (is_same_v<BufferElementType, char32_t>) {
            if constexpr (is_same_v<CHAR_T, Character>) {
                fData_.push_back (c.GetCharacterCode ());
            }
            else {
                fData_.push_back (c);
            }
            return; // handled
        }
        else if constexpr (is_same_v<BufferElementType, char8_t>) {
            if constexpr (is_same_v<CHAR_T, Character>) {
                if (c.IsASCII ()) [[likely]] {
                    fData_.push_back (c.GetAsciiCode ());
                    return; // handled
                }
            }
            else {
                if (isascii (c)) [[likely]] {
                    fData_.push_back (static_cast<ASCII> (c));
                    return; // handled
                }
            }
        }
        // fall thru - handle
        this->Append (span{&c, 1});
    }
#if !qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy
    template <typename OPTIONS>
    template <typename APPEND_ARG_T>
    inline auto StringBuilder<OPTIONS>::operator+= (APPEND_ARG_T&& a)
        -> StringBuilder& requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); }) {
            Append (forward<APPEND_ARG_T> (a));
            return *this;
        }

    template <typename OPTIONS>
    template <typename APPEND_ARG_T>
    inline auto StringBuilder<OPTIONS>::operator<< (APPEND_ARG_T&& a)
        -> StringBuilder& requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); }) {
            Append (forward<APPEND_ARG_T> (a));
            return *this;
        }
#endif
    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::push_back (Character c)
    {
        Append (c);
    }
    template <typename OPTIONS>
    inline size_t StringBuilder<OPTIONS>::size () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (sizeof (BufferElementType) == 4) {
            return fData_.size ();
        }
        else {
            return Memory::ValueOf (UTFConvert::kThe.ComputeCharacterLength<BufferElementType> (fData_));
        }
    }
    template <typename OPTIONS>
    inline bool StringBuilder<OPTIONS>::empty () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return fData_.size () == 0;
    }
    template <typename OPTIONS>
    inline Character StringBuilder<OPTIONS>::GetAt (size_t index) const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (same_as<BufferElementType, char32_t>) {
            Require (index < fData_.size ());
            return fData_[index];
        }
        else {
            // inefficient, but rarely used API
            Memory::StackBuffer<char32_t> probablyIgnoredBuf;
            span<const char32_t>          sp = this->GetData (&probablyIgnoredBuf);
            return sp[index];
        }
    }
    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::SetAt (Character item, size_t index) noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (is_same_v<BufferElementType, char32_t>) {
            Require (index < fData_.size ());
            fData_[index] = item.GetCharacterCode ();
        }
        else {
            // inefficient, but rarely used API
            Memory::StackBuffer<char32_t> probablyIgnoredBuf;
            span<const char32_t>          sp = this->GetData (&probablyIgnoredBuf);
            Require (index < sp.size ());
            char32_t* p = const_cast<char32_t*> (sp.data ()) + index;
            *p          = item.GetCharacterCode ();
            this->fData_.clear ();
            this->Append (sp);
        }
    }
    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::clear () noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.resize (0);
    }
    template <typename OPTIONS>
    inline String StringBuilder<OPTIONS>::str () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return String{span{fData_}};
        //return String{Memory::ConstSpan (span{fData_.data (), fData_.size ()})};
    }
    template <typename OPTIONS>
    template <typename RESULT_T>
    inline RESULT_T StringBuilder<OPTIONS>::As () const
        requires (is_same_v<RESULT_T, String> or is_same_v<RESULT_T, wstring>)
    {
        if constexpr (is_same_v<RESULT_T, String>) {
            return str ();
        }
        if constexpr (is_same_v<RESULT_T, wstring>) {
            return str ().template As<wstring> ();
        }
    }
    template <typename OPTIONS>
    template <typename RESULT_T>
    inline void StringBuilder<OPTIONS>::As (RESULT_T* into) const
        requires (is_same_v<RESULT_T, String> or is_same_v<RESULT_T, wstring>)
    {
        RequireNotNull (into);
        // @todo could do more efficiently
        *into = str ();
        if constexpr (is_same_v<RESULT_T, String>) {
            *into = str ();
        }
        if constexpr (is_same_v<RESULT_T, wstring>) {
            *into = str ().template As<wstring> ();
        }
    }
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::operator String () const
    {
        return As<String> ();
    }
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::operator wstring () const
    {
        return As<wstring> ();
    }
    template <typename OPTIONS>
    inline size_t StringBuilder<OPTIONS>::length () const noexcept
    {
        return size ();
    }
    template <typename OPTIONS>
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline span<const CHAR_T> StringBuilder<OPTIONS>::GetData (Memory::StackBuffer<CHAR_T>* probablyIgnoredBuf) const
        requires (not is_const_v<CHAR_T>)
    {
        RequireNotNull (probablyIgnoredBuf); // required param even if not used
        if constexpr (sizeof (CHAR_T) == sizeof (BufferElementType)) {
            return span{reinterpret_cast<const CHAR_T*> (fData_.data ()), fData_.size ()};
        }
        else {
            probablyIgnoredBuf->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_T> (span{fData_}));
            return UTFConvert::kThe.ConvertSpan (span{fData_}, span{*probablyIgnoredBuf});
        }
    }

}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
