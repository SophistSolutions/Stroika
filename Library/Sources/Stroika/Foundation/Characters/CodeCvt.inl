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

#include <bit>

#include "UTFConvert.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <typename CHAR_T>
        shared_ptr<typename CodeCvt<CHAR_T>::IRep> mk_StdCodeCvtRep_ ();
        template <typename CHAR_T>
        shared_ptr<typename CodeCvt<CHAR_T>::IRep> mk_StdCodeCvtRep_ (const locale& l);
        template <>
        shared_ptr<CodeCvt<char16_t>::IRep> mk_StdCodeCvtRep_<char16_t> ();
        template <>
        shared_ptr<CodeCvt<char32_t>::IRep> mk_StdCodeCvtRep_<char32_t> ();
        template <>
        shared_ptr<CodeCvt<wchar_t>::IRep> mk_StdCodeCvtRep_<wchar_t> (const locale& l);
    }

    /*
     ********************************************************************************
     ******************** CodeCvt<CHAR_T>::UTFConvertRep_ ***************************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename SERIALIZED_CHAR_T>
    struct CodeCvt<CHAR_T>::UTFConvertRep_ : CodeCvt<CHAR_T>::IRep {
        using result                     = typename CodeCvt<CHAR_T>::result;
        using MBState                    = typename CodeCvt<CHAR_T>::MBState;
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;
        UTFConvertRep_ (const UTFConverter& utfCodeCvt)
            : fCodeConverter_{utfCodeCvt}
        {
        }
        virtual result Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<const SERIALIZED_CHAR_T> serializedFrom{reinterpret_cast<const SERIALIZED_CHAR_T*> (from->data ()),
                                                         from->size () / sizeof (SERIALIZED_CHAR_T)};
            Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan

            auto handleShortTargetBuffer = [&] (span<const SERIALIZED_CHAR_T>* from, span<CHAR_T>* to) {
                // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                // to fit (and you can avoid this performance loss by using a larger output buffer)
                while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<CHAR_T> (*from) > to->size ()) {
                    // could be smarter leveraging requiredTargetBufSize, but KISS for now
                    if (from->empty ()) {
                        *to = span<CHAR_T>{}; // say nothing output, but no change to input
                        return CodeCvt<CHAR_T>::partial;
                    }
                    *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                }
                return CodeCvt<CHAR_T>::ok;
            };
            if (auto preflightResult = handleShortTargetBuffer (&serializedFrom, to); preflightResult != CodeCvt<CHAR_T>::ok) {
                return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
            }
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, *to, state);
            *from = as_bytes (serializedFrom.subspan (r.fSourceConsumed)); // point to remaining to use data - typically none
            *to   = to->subspan (0, r.fTargetProduced);                    // point ACTUAL copied data
            return cvtR_ (r.fStatus);
        }
        virtual result Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<SERIALIZED_CHAR_T> serializedTo{reinterpret_cast<SERIALIZED_CHAR_T*> (to->data ()), to->size () / sizeof (SERIALIZED_CHAR_T)};
            Assert (serializedTo.size_bytes () <= to->size ()); // note - serializedTo could be smaller than to in bytespan

            auto handleShortTargetBuffer = [&] (span<const CHAR_T>* from, span<SERIALIZED_CHAR_T>* to) {
                // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                // to fit (and you can avoid this performance loss by using a larger output buffer)
                while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<SERIALIZED_CHAR_T> (*from) > to->size ()) {
                    // could be smarter leveraging requiredTargetBufSize, but KISS for now
                    if (from->empty ()) {
                        *to = span<SERIALIZED_CHAR_T>{}; // say nothing output, but no change to input
                        return CodeCvt<CHAR_T>::partial;
                    }
                    *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                }
                return CodeCvt<CHAR_T>::ok;
            };
            if (auto preflightResult = handleShortTargetBuffer (from, &serializedTo); preflightResult != CodeCvt<CHAR_T>::ok) {
                return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
            }
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (*from, serializedTo, state);
            *from                        = from->subspan (r.fSourceConsumed); // point to remaining to use data - typically none
            *to                          = to->subspan (0, r.fTargetProduced * sizeof (SERIALIZED_CHAR_T)); // point ACTUAL copied data
            return cvtR_ (r.fStatus);
        }
        static result cvtR_ (UTFConverter::ConversionStatusFlag status)
        {
            switch (status) {
                case ConversionStatusFlag::ok:
                    return CodeCvt<CHAR_T>::ok;
                case ConversionStatusFlag::sourceExhausted:
                    return CodeCvt<CHAR_T>::partial;
                case ConversionStatusFlag::sourceIllegal:
                    return CodeCvt<CHAR_T>::error;
                default:
                    AssertNotReached ();
                    return CodeCvt<CHAR_T>::error;
            }
        }
        UTFConverter fCodeConverter_;
    };

    /*
     ********************************************************************************
     ***************** CodeCvt<CHAR_T>::UTFConvertSwappedRep_ ***********************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename SERIALIZED_CHAR_T>
    struct CodeCvt<CHAR_T>::UTFConvertSwappedRep_ : CodeCvt<CHAR_T>::UTFConvertRep_<SERIALIZED_CHAR_T> {
        using result    = typename CodeCvt<CHAR_T>::result;
        using MBState   = typename CodeCvt<CHAR_T>::MBState;
        using inherited = UTFConvertRep_<SERIALIZED_CHAR_T>;

        UTFConvertSwappedRep_ (const UTFConverter& utfCodeCvt)
            : inherited{utfCodeCvt}
        {
        }
        virtual result Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);
            auto r = inherited::Bytes2Characters (from, to, state);
            for (CHAR_T& i : *to) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{byteswap (i.As<char32_t> ())};
                }
                else {
                    i = byteswap (i);
                }
            }
            return r;
        }
        virtual result Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);
            Memory::StackBuffer<CHAR_T> buf{*from};
            for (CHAR_T& i : buf) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{byteswap (i.As<char32_t> ())};
                }
                else {
                    i = byteswap (i);
                }
            }
            auto useFrom = span<const CHAR_T>{buf.begin (), buf.size ()};
            auto r       = Characters2Bytes (&useFrom, to, state);
            *from        = from->subspan (0, useFrom.size ());
            return r;
        }
    };

    /*
     ********************************************************************************
     *********************** CodeCvt<CHAR_T>::UTF2UTFRep_ ***************************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename OTHER_CHAR_T>
    struct CodeCvt<CHAR_T>::UTF2UTFRep_ : CodeCvt<CHAR_T>::IRep {
        using result                     = typename CodeCvt<CHAR_T>::result;
        using MBState                    = typename CodeCvt<CHAR_T>::MBState;
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;
        static_assert (sizeof (CHAR_T) != sizeof (OTHER_CHAR_T)); // use another rep for that case
        UTF2UTFRep_ (const CodeCvt<OTHER_CHAR_T>& origCodeCvt, const UTFConverter& secondStep)
            : fOrigCodeCvt_{origCodeCvt}
            , fCodeConverter_{secondStep}
        {
        }
        virtual result Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // @todo INADEQUATE - FIRST DRAFT

            span<const byte>                  startFrom = *from;
            Memory::StackBuffer<OTHER_CHAR_T> intermediateBuf{1024}; // wrong size
            span<const OTHER_CHAR_T>          intermediateSpan   = span<OTHER_CHAR_T>{intermediateBuf.data (), intermediateBuf.size ()};
            result                            intermediateResult = fOrigCodeCvt_.Bytes2Characters (&startFrom, &intermediateSpan, state);
            if (intermediateResult != ok) {
                *from = startFrom;
                *to   = span<CHAR_T>{}; // nothing
                return intermediateResult;
            }
            // OK - now use fCodeConverter_ to perform the last step

            // @todo something like handleShortTargetBuffer - to handle that case - but trickier

            if (fCodeConverter_.ComputeTargetBufferSize<CHAR_T> (intermediateBuf) <= to->size ()) {
                // efficient, map directly
                *to = fCodeConverter_.ConvertSpan (intermediateBuf, *to);
                return ok;
            }
            else {
                // world of hurt...
                AssertNotImplemented ();
                return CodeCvt<CHAR_T>::error;
            }
        }
        virtual result Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // @todo INADEQUATE - FIRST DRAFT

            // first translate to something usable by fOrigCodeCvt_
            Memory::StackBuffer<OTHER_CHAR_T> intermediateBuf{fCodeConverter_.ComputeTargetBufferSize<OTHER_CHAR_T> (*from)};
            span<OTHER_CHAR_T>                intermediateSpan =
                fCodeConverter_.Convert (*from, span<OTHER_CHAR_T>{intermediateBuf.data (), intermediateBuf.size ()});

            result r = fOrigCodeCvt_.Characters2Bytes (&intermediateBuf, to, state);
            if (r == CodeCvt<CHAR_T>::ok) {
                // to has been updated
                // must fix from - in this case, we appear to have used all of from
                *from = span<const CHAR_T>{};
                return r;
            }
            else {
                AssertNotImplemented ();
                return CodeCvt<CHAR_T>::error;
            }
        }
        CodeCvt<OTHER_CHAR_T> fOrigCodeCvt_;
        UTFConverter          fCodeConverter_;
    };

    /*
     ********************************************************************************
     ******************************* CodeCvt<CHAR_T> ********************************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt ()
        requires (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>)
        : fRep_{Private_::mk_StdCodeCvtRep_<CHAR_T> ()}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l)
    {
        if constexpr (is_same_v<CHAR_T, wchar_t>) {
            fRep_ = Private_::mk_StdCodeCvtRep_<wchar_t> (l);
        }
        else if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
            fRep_ = reinterpret_pointer_cast<IRep> (Private_::mk_StdCodeCvtRep_<wchar_t> (l));
        }
        else {
            fRep_ = make_shared<UTF2UTFRep_<wchar_t>> (Private_::mk_StdCodeCvtRep_<wchar_t> (l));
        }
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (UnicodeExternalEncodings e)
        : fRep_{}
    {
        switch (e) {
            case UnicodeExternalEncodings::eUTF8:
                fRep_ = make_shared<UTFConvertRep_<char8_t>> (UTFConverter::kThe);
                break;
            case UnicodeExternalEncodings::eUTF16_BE:
            case UnicodeExternalEncodings::eUTF16_LE:
                if (e == UnicodeExternalEncodings::eUTF16) {
                    fRep_ = make_shared<UTFConvertRep_<char16_t>> (UTFConverter::kThe);
                }
                else {
                    fRep_ = make_shared<UTFConvertSwappedRep_<char16_t>> (UTFConverter::kThe);
                }
                break;
            case UnicodeExternalEncodings::eUTF32_BE:
            case UnicodeExternalEncodings::eUTF32_LE:
                if (e == UnicodeExternalEncodings::eUTF32) {
                    fRep_ = make_shared<UTFConvertRep_<char32_t>> (UTFConverter::kThe);
                }
                else {
                    fRep_ = make_shared<UTFConvertSwappedRep_<char32_t>> (UTFConverter::kThe);
                }
                break;

            // @todo UTF7 (maybe good enuf there to -- not ure - codecvt, or JIRA defer)
            default:
                AssertNotImplemented ();
        }
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <Character_UNICODECanAlwaysConvertTo OTHER_CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const CodeCvt<OTHER_CHAR_T>& basedOn)
        : fRep_{make_shared<UTF2UTFRep_<OTHER_CHAR_T>> (basedOn)}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const shared_ptr<IRep>& rep)
        : fRep_{(RequireNotNull (rep), rep)}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const -> result
    {
        AssertNotNull (fRep_);
        RequireNotNull (state);
        RequireNotNull (from);
        RequireNotNull (to);
        return fRep_->Bytes2Characters (from, to, state);
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
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
