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
    template <typename FACET>
    struct deletable_facet_ final : FACET {
        template <typename... Args>
        deletable_facet_ (Args&&... args)
            : FACET{forward<Args> (args)...}
        {
        }
        ~deletable_facet_ () {}
    };
}

namespace {
    using std::byte;

    template <typename CHAR_T, typename STD_CODE_CVT_T = deletable_facet_<codecvt<CHAR_T, char8_t, std::mbstate_t>>>
    struct CodeCvt_BasicUnicodeRep_CHAR_T : CodeCvt<CHAR_T>::IRep {
        STD_CODE_CVT_T fCodeCvt_;
        using result      = CodeCvt<CHAR_T>::result;
        using MBState     = CodeCvt<CHAR_T>::MBState;
        using extern_type = typename STD_CODE_CVT_T::extern_type;

        CodeCvt_BasicUnicodeRep_CHAR_T ()
            : fCodeCvt_{}
        {
        }
        CodeCvt_BasicUnicodeRep_CHAR_T (STD_CODE_CVT_T&& codeCtt)
            : fCodeCvt_{move<STD_CODE_CVT_T> (codeCtt)}
        {
        }

        virtual result in (MBState* state, span<const byte>* from, span<CHAR_T>* to) const override
        {
            const extern_type* _First1 = reinterpret_cast<const extern_type*> (from->data ());
            const extern_type* _Last1  = _First1 + from->size ();
            const extern_type* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            CHAR_T*            _First2 = to->data ();
            CHAR_T*            _Last2  = _First2 + to->size ();
            CHAR_T*            _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            auto               r       = fCodeCvt_.in (*state, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            *from                      = from->subspan (_Mid1 - _First1);  // point to remaining to use data - typically none
            *to                        = to->subspan (0, _Mid2 - _First2); // point ACTUAL copied data
            return r;
        }
        virtual result out (MBState* state, span<const CHAR_T>* from, span<byte>* to) const override
        {
            const CHAR_T* _First1 = from->data ();
            const CHAR_T* _Last1  = _First1 + from->size ();
            const CHAR_T* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            extern_type*  _First2 = reinterpret_cast<extern_type*> (to->data ());
            extern_type*  _Last2  = _First2 + to->size ();
            extern_type*  _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            auto          r       = fCodeCvt_.out (*state, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            *from                 = from->subspan (_Mid1 - _First1);  // point to remaining to use data - typically none
            *to                   = to->subspan (0, _Mid2 - _First2); // point ACTUAL copied data
            return r;
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
shared_ptr<CodeCvt<wchar_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<wchar_t> (const locale& l)
{
    // next todo is add COPY CTOR given codecvt facet, and extract from locale and wrap in this class
    // CodeCvt_BasicUnicodeRep_CHAR_T

    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()}; // only works to wchar_t.

    Destructible_codecvt_byname&& a = move (cvt);

    //CodeCvt_BasicUnicodeRep_CHAR_T<wchar_t, Destructible_codecvt_byname> aa{cvt};   // this is what I need to get to compile
    return nullptr;
    // debug why this fails and do that instead of above...
    // todo check if sizes match and if so cast, and otherwise need adapter from 32_t
    // return Memory::MakeSharedPtr<CodeCvt_BasicUnicodeRep_CHAR_T<wchar_t, Destructible_codecvt_byname>> (move (cvt));
}
