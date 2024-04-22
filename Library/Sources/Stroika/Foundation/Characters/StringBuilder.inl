/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <memory>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/UTFConvert.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** StringBuilder<OPTIONS> ***************************
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
            if constexpr (same_as<CHAR_T, BufferElementType> or (same_as<BufferElementType, char8_t> and same_as<CHAR_T, ASCII>) or
                          (sizeof (CHAR_T) == sizeof (BufferElementType) and (same_as<CHAR_T, wchar_t> or same_as<BufferElementType, wchar_t>))) {
                // easy case - just resize buffer, and copy data in
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
                //
                // This case is more complicated. we must 'transcode' from one 'unicode' character type to another.
                //
                // But before falling back on the expensive UTFConvert call, first try some special cases.
                //      (1) src is ASCII - can just be copied in
                //      (2) NYI, but if src is char16_t (or equiv) and target is char32_t or equiv, can just look for if is surroage or not.
                //
                //  In both cases, walk source - and find first non-complaint character. Use quick algorithm for compliant ones
                //  and if needed, slow algorithm for the rest.
                //
                //  For now, only the ASCII case is optimized --LGP 2023-09-12
                //
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
        if constexpr (same_as<CHAR_T, ASCII>) {
            Require (Character::IsASCII (span{s})); // Optimization here is just avoiding Character::CheckASCII (s); in release builds
            this->fData_.push_back_coerced (span{s});
        }
        else {
            Append (span{s});
        }
    }
    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::Append (const String& s)
    {
        Memory::StackBuffer<BufferElementType> ignored; // could use char8_t maybe here - optimizing for ASCII case or BufferElementType
        span<const BufferElementType>          p = s.GetData (&ignored);
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
        } template <typename OPTIONS>
        template <typename APPEND_ARG_T>
        inline auto StringBuilder<OPTIONS>::operator<< (APPEND_ARG_T&& a) -> StringBuilder&
        requires (Characters::Private_::IToString<APPEND_ARG_T> or
                  requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
    {
        if constexpr (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); }) {
            Append (forward<APPEND_ARG_T> (a));
        }
        else {
            Append (Characters::UnoverloadedToString (forward<APPEND_ARG_T> (a)));
        }
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
    inline void StringBuilder<OPTIONS>::ShrinkTo (size_t sz) noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        Require (sz <= this->size ());
        fData_.resize (sz);
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
    }
    template <typename OPTIONS>
    template <typename RESULT_T>
    inline RESULT_T StringBuilder<OPTIONS>::As () const
        requires (Configuration::IAnyOf<RESULT_T, String, wstring, u8string, u16string, u32string>)
    {
        if constexpr (same_as<RESULT_T, String>) {
            return str ();
        }
        if constexpr (same_as<RESULT_T, wstring>) {
            Memory::StackBuffer<wchar_t> maybeIngored;
            span<const wchar_t>          p = this->GetData (&maybeIngored);
            return wstring{p.data (), p.size ()};
        }
        if constexpr (same_as<RESULT_T, u8string>) {
            Memory::StackBuffer<char8_t> maybeIngored;
            span<const char8_t>          p = this->GetData (&maybeIngored);
            return u8string{p.data (), p.size ()};
        }
        if constexpr (same_as<RESULT_T, u16string>) {
            Memory::StackBuffer<char16_t> maybeIngored;
            span<const char16_t>          p = this->GetData (&maybeIngored);
            return u16string{p.data (), p.size ()};
        }
        if constexpr (same_as<RESULT_T, u32string>) {
            Memory::StackBuffer<char32_t> maybeIngored;
            span<const char32_t>          p = this->GetData (&maybeIngored);
            return u32string{p.data (), p.size ()};
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
    inline StringBuilder<OPTIONS>::operator u8string () const
    {
        return As<u8string> ();
    }
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::operator u16string () const
    {
        return As<u16string> ();
    }
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::operator u32string () const
    {
        return As<u32string> ();
    }
    template <typename OPTIONS>
    inline size_t StringBuilder<OPTIONS>::length () const noexcept
    {
        return size ();
    }
    template <typename OPTIONS>
    inline auto StringBuilder<OPTIONS>::data () -> span<BufferElementType>
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_}; // doesn't do much good bit a little
        return fData_.data ();
    }
    template <typename OPTIONS>
    inline auto StringBuilder<OPTIONS>::data () const -> span<const BufferElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_}; // doesn't do much good bit a little
        return fData_.data ();
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
