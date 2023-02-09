/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"
#include "../Memory/BlockAllocated.h"

#include "CodeCvt.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

namespace {
    // crazy - from https://en.cppreference.com/w/cpp/locale/codecvt
    template <class Facet>
    struct deletable_facet_ : Facet {
        template <class... Args>
        deletable_facet_ (Args&&... args)
            : Facet (std::forward<Args> (args)...)
        {
        }
        ~deletable_facet_ () {}
    };
}

namespace {
    using std::byte;

    template <typename CHAR_T>
    struct CodeCvt_BasicUnicodeRep_CHAR_T : CodeCvt<CHAR_T>::IRep {
        deletable_facet_<codecvt<CHAR_T, char8_t, std::mbstate_t>> fCodeCvt_;
        using result  = CodeCvt<CHAR_T>::result;
        using MBState = CodeCvt<CHAR_T>::MBState;
        virtual result in (MBState& _State, const byte* _First1, const byte* _Last1, const byte*& _Mid1, CHAR_T* _First2, CHAR_T* _Last2,
                           CHAR_T*& _Mid2) const override
        {
            return fCodeCvt_.in (_State, reinterpret_cast<const char8_t*> (_First1), reinterpret_cast<const char8_t*> (_Last1),
                                 reinterpret_cast<const char8_t*&> (_Mid1), _First2, _Last2, _Mid2);
        }
        virtual result out (MBState& _State, const CHAR_T* _First1, const CHAR_T* _Last1, const CHAR_T*& _Mid1, byte* _First2, byte* _Last2,
                            byte*& _Mid2) const override
        {
            return fCodeCvt_.out (_State, _First1, _Last1, _Mid1, reinterpret_cast<char8_t*> (_First2), reinterpret_cast<char8_t*> (_Last2),
                                  reinterpret_cast<char8_t*&> (_Mid2));
        }
    };

}

/*
 ********************************************************************************
 ******************** Characters::Private_::mk_UNICODECodeCvt_ ******************
 ********************************************************************************
 */

template <>
shared_ptr<CodeCvt<char16_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<char16_t> ()
{
    return Memory::MakeSharedPtr<CodeCvt_BasicUnicodeRep_CHAR_T<char16_t>> ();
}

template <>
shared_ptr<CodeCvt<char32_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<char32_t> ()
{
    return Memory::MakeSharedPtr<CodeCvt_BasicUnicodeRep_CHAR_T<char32_t>> ();
}

template <>
shared_ptr<CodeCvt<char16_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<char16_t> (const locale& l)
{
    // next todo is add COPY CTOR given codecvt facet, and extract from locale and wrap in this class
    // CodeCvt_BasicUnicodeRep_CHAR_T
    return Memory::MakeSharedPtr<CodeCvt_BasicUnicodeRep_CHAR_T<char16_t>> ();
}

template <>
shared_ptr<CodeCvt<char32_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<char32_t> (const locale& l)
{
    // next todo is add COPY CTOR given codecvt facet, and extract from locale and wrap in this class
    // CodeCvt_BasicUnicodeRep_CHAR_T
    return Memory::MakeSharedPtr<CodeCvt_BasicUnicodeRep_CHAR_T<char32_t>> ();
}