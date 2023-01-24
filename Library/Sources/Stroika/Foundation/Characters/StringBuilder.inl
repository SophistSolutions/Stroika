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
    template <typename OPTIONS>
    inline StringBuilder<OPTIONS>::StringBuilder (const String& initialValue)
    {
        Append (initialValue);
    }
    template <typename OPTIONS>
    template <Character_Compatible CHAR_T>
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
    template <Character_Compatible CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (span<const CHAR_T> s)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        size_t                                                 spanSize = s.size ();
        if (spanSize != 0) {
            if constexpr (is_same_v<CHAR_T, Character_ASCII>) {
                Character::CheckASCII (s);
            }
            if constexpr (is_same_v<CHAR_T, BufferElementType>) {
                size_t i = fData_.size ();
                fData_.GrowToSize_uninitialized (i + spanSize);
                const BufferElementType* copyFrom = reinterpret_cast<const BufferElementType*> (s.data ());
                std::copy (copyFrom, copyFrom + spanSize, fData_.data () + i);
            }
            else {
                // @todo  OPTIMIZATION OPPORTUNITY - if given an ascii span, can just do those chars one at a time...
                Memory::StackBuffer<BufferElementType> buf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<BufferElementType> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                Append (UTFConverter::kThe.ConvertSpan (s, span{buf.data (), buf.size ()}));
#else
                Append (UTFConverter::kThe.ConvertSpan (s, span{buf}));
#endif
            }
        }
    }
    template <typename OPTIONS>
    template <Character_Compatible CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (span<CHAR_T> s)
    {
        Append (Memory::ConstSpan (s));
    }
    template <typename OPTIONS>
    template <Character_Compatible CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const CHAR_T* s)
    {
        Append (span{s, CString::Length (s)});
    }
    template <typename OPTIONS>
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const basic_string<CHAR_T>& s)
    {
#if qCompilerAndStdLib_spanOfContainer_Buggy
        Append (span{s.data (), s.size ()});
#else
        Append (span{s});
#endif
    }
    template <typename OPTIONS>
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (const basic_string_view<CHAR_T>& s)
    {
#if qCompilerAndStdLib_spanOfContainer_Buggy
        Append (span{s.data (), s.size ()});
#else
        Append (span{s});
#endif
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
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void StringBuilder<OPTIONS>::Append (CHAR_T c)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (is_same_v<BufferElementType, char32_t>) {
            size_t len = fData_.size ();
            fData_.resize_uninitialized (len + 1);
            if constexpr (is_same_v<CHAR_T, Character>) {
                fData_[len] = c.GetCharacterCode ();
            }
            else {
                fData_[len] = c;
            }
            return;
        }
        else if constexpr (is_same_v<BufferElementType, char8_t>) {
            if constexpr (is_same_v<CHAR_T, Character>) {
                if (c.IsASCII ()) [[likely]] {
                    size_t byteLen = fData_.size ();
                    fData_.resize_uninitialized (byteLen + 1);
                    fData_[byteLen] = c.GetAsciiCode ();
                }
            }
            else {
                if (isascii (c)) [[likely]] {
                    size_t byteLen = fData_.size ();
                    fData_.resize_uninitialized (byteLen + 1);
                    fData_[byteLen] = c;
                }
            }
            return;
        }
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
            return UTFConverter::kThe.ComputeCharacterLength (fData_);
            //return UTFConverter::kThe.ComputeCharacterLength (span{fData_.data (), fData_.size ()});
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
        if constexpr (is_same_v<BufferElementType, char32_t>) {
            Require (index < fData_.size ());
            // @todo fix for mutli-code-point case
            return fData_[index];
        }
        else {
            AssertNotImplemented ();
            return 0;
        }
    }

    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::SetAt (Character item, size_t index) noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        if constexpr (is_same_v<BufferElementType, char32_t>) {
            Require (index < fData_.size ());
            // @todo fix for mutli-code-point case
            fData_[index] = item.GetCharacterCode ();
        }
        else {
            AssertNotImplemented ();
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
        return String{Memory::ConstSpan (span{fData_.data (), fData_.size ()})};
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

#if 0
    template <typename OPTIONS>
    inline size_t StringBuilder<OPTIONS>::capacity () const noexcept
    {
        // @todo fix API NAME/DESIGN for mutli-code-point case
        return fData_.capacity ();
    }

    template <typename OPTIONS>
    inline void StringBuilder<OPTIONS>::reserve (size_t newCapacity)
    {
        // @todo fix API NAME/DESIGN for mutli-code-point case
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.reserve (newCapacity);
    }
#endif

    template <typename OPTIONS>
    template <Character_Compatible CHAR_T>
    span<const CHAR_T> StringBuilder<OPTIONS>::GetData (Memory::StackBuffer<CHAR_T>* probablyIgnoredBuf) const
        requires (not is_const_v<CHAR_T>)
    {
        RequireNotNull (probablyIgnoredBuf); // required param even if not used
        if constexpr (sizeof (CHAR_T) == sizeof (BufferElementType)) {
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
