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
    struct deletable_facet_ : FACET {
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

    /*
     *  This is crazy complicated because codecvt goes out of its way to be hard to copy, hard to move, but with
     *  a little care, can be made to work with unique_ptr.
     */
    template <typename STD_CODE_CVT_T, typename CHAR_T = typename STD_CODE_CVT_T::intern_type>
    struct CodeCvt_WrapStdCodeCvt_CHAR_T : CodeCvt<CHAR_T>::IRep {
        unique_ptr<STD_CODE_CVT_T> fCodeCvt_;
        using result      = typename CodeCvt<CHAR_T>::result;
        using MBState     = typename CodeCvt<CHAR_T>::MBState;
        using extern_type = typename STD_CODE_CVT_T::extern_type;

        CodeCvt_WrapStdCodeCvt_CHAR_T (unique_ptr<STD_CODE_CVT_T>&& codeCvt)
            : fCodeCvt_{move (codeCvt)}
        {
        }
        virtual result Bytes2Characters (MBState* state, span<const byte>* from, span<CHAR_T>* to) const override
        {
            const extern_type* _First1 = reinterpret_cast<const extern_type*> (from->data ());
            const extern_type* _Last1  = _First1 + from->size ();
            const extern_type* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            CHAR_T*            _First2 = to->data ();
            CHAR_T*            _Last2  = _First2 + to->size ();
            CHAR_T*            _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            auto               r       = fCodeCvt_->in (*state, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            *from                      = from->subspan (_Mid1 - _First1);  // point to remaining to use data - typically none
            *to                        = to->subspan (0, _Mid2 - _First2); // point ACTUAL copied data
            return r;
        }
        virtual result Characters2Bytes (MBState* state, span<const CHAR_T>* from, span<byte>* to) const override
        {
            const CHAR_T* _First1 = from->data ();
            const CHAR_T* _Last1  = _First1 + from->size ();
            const CHAR_T* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            extern_type*  _First2 = reinterpret_cast<extern_type*> (to->data ());
            extern_type*  _Last2  = _First2 + to->size ();
            extern_type*  _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            auto          r       = fCodeCvt_->out (*state, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
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
    using CODE_CVT              = codecvt<char16_t, char8_t, std::mbstate_t>;
    using D_CODE_CVT            = deletable_facet_<CODE_CVT>;
    using BASIC_UNICODE_WRAPPER = CodeCvt_WrapStdCodeCvt_CHAR_T<D_CODE_CVT>;
    return Memory::MakeSharedPtr<BASIC_UNICODE_WRAPPER> (make_unique<D_CODE_CVT> ());
}
template <>
shared_ptr<CodeCvt<char32_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<char32_t> ()
{
    using CODE_CVT              = codecvt<char32_t, char8_t, std::mbstate_t>;
    using D_CODE_CVT            = deletable_facet_<CODE_CVT>;
    using BASIC_UNICODE_WRAPPER = CodeCvt_WrapStdCodeCvt_CHAR_T<D_CODE_CVT>;
    return Memory::MakeSharedPtr<BASIC_UNICODE_WRAPPER> (make_unique<D_CODE_CVT> ());
}
template <>
shared_ptr<CodeCvt<wchar_t>::IRep> Characters::Private_::mk_UNICODECodeCvt_<wchar_t> (const locale& l)
{
    using CODE_CVT              = codecvt_byname<wchar_t, char, mbstate_t>;
    using D_CODE_CVT            = deletable_facet_<CODE_CVT>;
    using BASIC_UNICODE_WRAPPER = CodeCvt_WrapStdCodeCvt_CHAR_T<D_CODE_CVT>;
    return Memory::MakeSharedPtr<BASIC_UNICODE_WRAPPER> (make_unique<D_CODE_CVT> (l.name ()));
}
