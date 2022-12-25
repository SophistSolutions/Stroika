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
    inline StringBuilder::StringBuilder (const String& initialValue)
    {
        Append (initialValue);
    }
    template <typename CHAR_T>
    inline StringBuilder::StringBuilder (span<const CHAR_T> initialValue)
        requires (is_same_v<CHAR_T, char8_t> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t> or is_same_v<CHAR_T, wchar_t>)
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
    template <typename CHAR_T>
    inline void StringBuilder::Append (span<const CHAR_T> s)
        requires (is_same_v<CHAR_T, char8_t> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t> or is_same_v<CHAR_T, wchar_t> or is_same_v<CHAR_T, Character>)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        size_t                                                 rhsLen = s.size ();
        if (rhsLen != 0) {
            if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                size_t i = fLength_;
                fData_.GrowToSize_uninitialized (i + rhsLen);
                fLength_ += rhsLen;
                (void)::memcpy (fData_.begin () + i, &*s.begin (), sizeof (wchar_t) * rhsLen);
            }
            else {
                Memory::StackBuffer<wchar_t> buf;
                auto                         r = UTFConverter::kThe.Convert (s, span{buf});
                Append (String{&*buf.begin (), &*buf.begin () + r.fTargetProduced});
            }
        }
    }
    template <typename CHAR_T>
    inline void StringBuilder::Append (const CHAR_T* s)
        requires (is_same_v<CHAR_T, char8_t> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t> or is_same_v<CHAR_T, wchar_t> or is_same_v<CHAR_T, Character>)
    {
        Append (span{s, CString::Length (s)});
    }
    template <typename CHAR_T>
    inline void StringBuilder::Append (const basic_string<CHAR_T>& s)
        requires (is_same_v<CHAR_T, char8_t> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t> or is_same_v<CHAR_T, wchar_t>)
    {
        Append (span{s});
    }
    template <typename CHAR_T>
    inline void StringBuilder::Append (const basic_string_view<CHAR_T>& s)
        requires (is_same_v<CHAR_T, char8_t> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t> or is_same_v<CHAR_T, wchar_t>)
    {
        Append (span{s});
    }
    inline void StringBuilder::Append (const String& s)
    {
        pair<const wchar_t*, const wchar_t*> p = s.GetData<wchar_t> ();
        if (p.first != p.second) {
            Append (span{p.first, p.second});
        }
    }
    inline void StringBuilder::Append (wchar_t c)
    {
        Append (span<const wchar_t>{&c, 1});    // @todo get working with explicit template param!
    }
    inline void StringBuilder::Append (Character c)
    {
        Append (c.GetCharacterCode ());
    }
    template <typename APPEND_ARG_T>
    inline StringBuilder& StringBuilder::operator+= (APPEND_ARG_T&& a)
    //requires (requires (APPEND_ARG_T a) { Append (a); })
    {
        Append (forward<APPEND_ARG_T> (a));
        return *this;
    }
    template <typename APPEND_ARG_T>
    inline StringBuilder& StringBuilder::operator<< (APPEND_ARG_T&& a)
    //requires (requires (APPEND_ARG_T a) { Append (a); })
    {
        Append (forward<APPEND_ARG_T> (a));
        return *this;
    }

    inline void StringBuilder::push_back (Character c)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.GrowToSize_uninitialized (fLength_ + 1);
        fData_[fLength_] = c.GetCharacterCode ();
        ++fLength_;
    }
    inline size_t StringBuilder::size () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return fLength_;
    }
    inline bool StringBuilder::empty () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return fLength_ == 0;
    }
    inline Character StringBuilder::GetAt (size_t index) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        Require (index < fLength_);
        return fData_[index];
    }
    inline void StringBuilder::SetAt (Character item, size_t index)
    {
        Require (index < fLength_);
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_[index] = item.GetCharacterCode ();
    }
    inline const wchar_t* StringBuilder::c_str () const
    {
        // @todo PROBABLY DEPREACTE else -fix the constness at least - and make this WriteContext...
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        fData_.GrowToSize_uninitialized (fLength_ + 1);
        fData_[fLength_] = '\0';
        return fData_.begin ();
    }
    inline void StringBuilder::clear ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fLength_ = 0;
    }
    inline String StringBuilder::str () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
        return String{fData_.begin (), fData_.begin () + fLength_};
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
    inline StringBuilder::operator String () const
    {
        return As<String> ();
    }
    inline StringBuilder::operator wstring () const
    {
        return As<wstring> ();
    }
    inline size_t StringBuilder::length () const
    {
        return size ();
    }
    inline const wchar_t* StringBuilder::begin ()
    {
        return c_str ();
    }
    inline const wchar_t* StringBuilder::end ()
    {
        return c_str () + length ();
    }
    inline size_t StringBuilder::capacity () const
    {
        return fData_.capacity ();
    }
    inline void StringBuilder::reserve (size_t newCapacity)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySyncrhonized_};
        fData_.reserve (newCapacity);
    }

}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
