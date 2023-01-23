/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

#include "CString/Utilities.h"
#include "UTFConvert.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline StringBuilder::StringBuilder (const String& initialValue) { Append (initialValue); }
    template <Character_Compatible CHAR_T>
    inline StringBuilder::StringBuilder (span<const CHAR_T> initialValue)
    {
        Append (initialValue);
    }
    inline StringBuilder& StringBuilder::operator= (const String& rhs)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        clear ();
        Append (rhs);
        return *this;
    }

    template <Character_Compatible CHAR_T>
    inline void StringBuilder::Append (span<const CHAR_T> s)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        size_t                                                 spanSize = s.size ();
        if (spanSize != 0) {
            if constexpr (is_same_v<CHAR_T, Character_ASCII>) {
                Character::CheckASCII (s);
            }
            using BUFFER_ELT_TYPE = decltype (fData_)::value_type;
            if constexpr (sizeof (CHAR_T) == sizeof (BUFFER_ELT_TYPE)) {
                size_t i = fData_.size ();
                fData_.GrowToSize_uninitialized (i + spanSize);
                const BUFFER_ELT_TYPE* copyFrom = reinterpret_cast<const BUFFER_ELT_TYPE*> (s.data ());
                std::copy (copyFrom, copyFrom + spanSize, fData_.data () + i);
            }
            else {
                // @todo  OPTIMZIATION OPPORTUNITY - if given an ascii span, can just do those chars one at a time...

                Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<char32_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                Append (String{UTFConverter::kThe.ConvertSpan (s, span{buf.data (), buf.size ()})});
#else
                Append (String{UTFConverter::kThe.ConvertSpan (s, span{buf})});
#endif
            }
        }
    }
    template <Character_Compatible CHAR_T>
    inline void StringBuilder::Append (span<CHAR_T> s)
    {
        Append (Memory::ConstSpan (s));
    }
    template <Character_Compatible CHAR_T>
    inline void StringBuilder::Append (const CHAR_T* s)
    {
        Append (span{s, CString::Length (s)});
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline void StringBuilder::Append (const basic_string<CHAR_T>& s)
    {
#if qCompilerAndStdLib_spanOfContainer_Buggy
        Append (span{s.data (), s.size ()});
#else
        Append (span{s});
#endif
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline void StringBuilder::Append (const basic_string_view<CHAR_T>& s)
    {
#if qCompilerAndStdLib_spanOfContainer_Buggy
        Append (span{s.data (), s.size ()});
#else
        Append (span{s});
#endif
    }
    inline void StringBuilder::Append (const String& s)
    {
        Memory::StackBuffer<char32_t> ignored;
        span                          p = s.GetData<char32_t> (&ignored);
        if (not p.empty ()) {
            Append (p);
        }
    }
    inline void StringBuilder::Append (Character c)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        size_t                                                 len = fData_.size ();
        fData_.resize_uninitialized (len + 1);
        fData_[len] = c.GetCharacterCode ();
    }

    #if !qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy
    template <typename APPEND_ARG_T>
    inline StringBuilder& StringBuilder::operator+= (APPEND_ARG_T&& a)
        requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
    {
        Append (forward<APPEND_ARG_T> (a));
        return *this;
    }

    template <typename APPEND_ARG_T>
    inline StringBuilder& StringBuilder::operator<< (APPEND_ARG_T&& a)
        requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
    {
        Append (forward<APPEND_ARG_T> (a));
        return *this;
    }
    #endif

    inline void StringBuilder::push_back (Character c) { Append (c); }

    inline size_t StringBuilder::size () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return fData_.size ();
    }

    inline bool StringBuilder::empty () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return fData_.size () == 0;
    }

    inline Character StringBuilder::GetAt (size_t index) const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        Require (index < fData_.size ());
        return fData_[index];
    }

    inline void StringBuilder::SetAt (Character item, size_t index) noexcept
    {
        Require (index < fData_.size ());
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_[index] = item.GetCharacterCode ();
    }

    inline void StringBuilder::clear () noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.resize (0);
    }

    inline String StringBuilder::str () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return String{Memory::ConstSpan (span{fData_.data (), fData_.size ()})};
    }

    template <>
    inline void StringBuilder::As (String* into) const
    {
        RequireNotNull (into);
        // @todo could do more efficiently
        *into = str ();
    }
    template <>
    inline String StringBuilder::As () const
    {
        return str ();
    }
    template <>
    inline void StringBuilder::As (wstring* into) const
    {
        RequireNotNull (into);
        // @todo could do more efficiently
        *into = str ().As<wstring> ();
    }
    template <>
    inline wstring StringBuilder::As () const
    {
        // @todo could do more efficiently
        return str ().As<wstring> ();
    }

    inline StringBuilder::operator String () const { return As<String> (); }

    inline StringBuilder::operator wstring () const { return As<wstring> (); }

    inline size_t StringBuilder::length () const noexcept { return size (); }

    inline size_t StringBuilder::capacity () const noexcept { return fData_.capacity (); }

    inline void StringBuilder::reserve (size_t newCapacity)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.reserve (newCapacity);
    }

    template <Character_Compatible CHAR_T>
    span<const CHAR_T> StringBuilder::GetData (Memory::StackBuffer<CHAR_T>* probablyIgnoredBuf) const
        requires (not is_const_v<CHAR_T>)
    {
        RequireNotNull (probablyIgnoredBuf); // required param even if not used
        if constexpr (sizeof (CHAR_T) == sizeof (char32_t)) {
            return span{reinterpret_cast<const CHAR_T*> (fData_.data ()), fData_.size ()};
        }
        else {
#if qCompilerAndStdLib_spanOfContainer_Buggy
            probablyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_T> (span{fData_.data (), fData_.size ()}));
            return UTFConverter::kThe.ConvertSpan (span{fData_.data (), fData_.size ()},
                                                   span{probablyIgnoredBuf->data (), probablyIgnoredBuf->size ()});
#else
            probablyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_T> (span{fData_}));
            return UTFConverter::kThe.ConvertSpan (span{fData_}, span{*probablyIgnoredBuf});
#endif
        }
    }

}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
