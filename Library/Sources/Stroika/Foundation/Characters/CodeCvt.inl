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
        shared_ptr<CodeCvt<char16_t>::IRep> mk_UNICODECodeCvt_<char16_t> (const locale& l);
        template <>
        shared_ptr<CodeCvt<char32_t>::IRep> mk_UNICODECodeCvt_<char32_t> (const locale& l);
    }

    /*
     ********************************************************************************
     ******************************* CodeCvt<CHAR_T> ********************************
     ********************************************************************************
     */
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt ()
        : fRep_{Private_::mk_UNICODECodeCvt_<CHAR_T> ()}
    {
    }
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l)
        : fRep_{Private_::mk_UNICODECodeCvt_<CHAR_T> (l)}
    {
    }
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    inline auto CodeCvt<CHAR_T>::in (MBState& _State, const byte* _First1, const byte* _Last1, const byte*& _Mid1, CHAR_T* _First2,
                                     CHAR_T* _Last2, CHAR_T*& _Mid2) const -> result
    {
        return fRep_->in (_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    inline auto CodeCvt<CHAR_T>::out (MBState& _State, const CHAR_T* _First1, const CHAR_T* _Last1, const CHAR_T*& _Mid1, byte* _First2,
                                      byte* _Last2, byte*& _Mid2) const -> result
    {
        return fRep_->out (_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

}

#endif /*_Stroika_Foundation_Characters_CodeCvt_inl_*/
