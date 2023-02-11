/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodeCvt_inl_
#define _Stroika_Foundation_Characters_CodeCvt_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <typename CHAR_T>
        shared_ptr<typename CodeCvt<CHAR_T>::IRep> mk_UNICODECodeCvt_ ();
        template <typename CHAR_T>
        shared_ptr<typename CodeCvt<CHAR_T>::IRep> mk_UNICODECodeCvt_ (const locale& l);
        template <>
        shared_ptr<CodeCvt<char16_t>::IRep> mk_UNICODECodeCvt_<char16_t> ();
        template <>
        shared_ptr<CodeCvt<char32_t>::IRep> mk_UNICODECodeCvt_<char32_t> ();
        template <>
        shared_ptr<CodeCvt<wchar_t>::IRep> mk_UNICODECodeCvt_<wchar_t> (const locale& l);
    }

    /*
     ********************************************************************************
     ******************************* CodeCvt<CHAR_T> ********************************
     ********************************************************************************
     */
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt ()
        requires (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>)
        : fRep_{Private_::mk_UNICODECodeCvt_<CHAR_T> ()}
    {
    }
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l)
        requires (is_same_v<CHAR_T, wchar_t>)
        : fRep_{Private_::mk_UNICODECodeCvt_<CHAR_T> (l)}
    {
    }
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const shared_ptr<IRep>& rep)
        : fRep_{(RequireNotNull (rep), rep)}
    {
    }
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const -> result
    {
        AssertNotNull (fRep_);
        RequireNotNull (state);
        RequireNotNull (from);
        RequireNotNull (to);
        return fRep_->Bytes2Characters (from, to, state);
    }
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline auto CodeCvt<CHAR_T>::Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const -> result
    {
        AssertNotNull (fRep_);
        RequireNotNull (state);
        RequireNotNull (from);
        RequireNotNull (to);
        return fRep_->Characters2Bytes (from, to, state);
    }

}

#endif /*_Stroika_Foundation_Characters_CodeCvt_inl_*/
