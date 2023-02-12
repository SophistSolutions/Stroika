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

    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename SERIALIZED_CHAR_T>
    struct CodeCvt<CHAR_T>::UTFConvertRep_ : CodeCvt<Character>::IRep {
        UTFConverter fCodeConverter_;
        using result                     = typename CodeCvt<Character>::result;
        using MBState                    = typename CodeCvt<Character>::MBState;
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;

        UTFConvertRep_ (const UTFConverter& utfCodeCvt)
            : fCodeConverter_{utfCodeCvt}
        {
        }
        virtual result Bytes2Characters (span<const byte>* from, span<Character>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<const SERIALIZED_CHAR_T> serializedFrom{reinterpret_cast<const SERIALIZED_CHAR_T*> (from->data ()),
                                                         from->size () / sizeof (SERIALIZED_CHAR_T)};
            Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan

            auto handleShortTargetBuffer = [&] (span<const SERIALIZED_CHAR_T>* from, span<Character>* to) {
                // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                // to fit (and you can avoid this performance loss by using a larger output buffer)
                while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<Character> (*from) > to->size ()) {
                    // could be smarter leveraging requiredTargetBufSize, but KISS for now
                    if (from->empty ()) {
                        *to = span<Character>{}; // say nothing output, but no change to input
                        return CodeCvt<Character>::partial;
                    }
                    *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                }
                return CodeCvt<Character>::ok;
            };
            if (auto preflightResult = handleShortTargetBuffer (&serializedFrom, to) != CodeCvt<Character>::ok) {
                return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
            }
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, *to, state);
            *from = as_bytes (serializedFrom.subspan (r.fSourceConsumed)); // point to remaining to use data - typically none
            *to   = to->subspan (0, r.fTargetProduced);                    // point ACTUAL copied data
            return cvtR_ (r.fStatus);
        }
        virtual result Characters2Bytes (span<const Character>* from, span<byte>* to, MBState* state) const override
        {
            RequireNotNull (state);
            RequireNotNull (from);
            RequireNotNull (to);

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<SERIALIZED_CHAR_T> serializedTo{reinterpret_cast<SERIALIZED_CHAR_T*> (to->data ()), to->size () / sizeof (SERIALIZED_CHAR_T)};
            Assert (serializedTo.size_bytes () <= to->size ()); // note - serializedTo could be smaller than to in bytespan

            auto handleShortTargetBuffer = [&] (span<const Character>* from, span<SERIALIZED_CHAR_T>* to) {
                // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                // to fit (and you can avoid this performance loss by using a larger output buffer)
                while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<SERIALIZED_CHAR_T> (*from) > to->size ()) {
                    // could be smarter leveraging requiredTargetBufSize, but KISS for now
                    if (from->empty ()) {
                        *to = span<SERIALIZED_CHAR_T>{}; // say nothing output, but no change to input
                        return CodeCvt<Character>::partial;
                    }
                    *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                }
                return CodeCvt<Character>::ok;
            };
            if (auto preflightResult = handleShortTargetBuffer (from, &serializedTo) != CodeCvt<Character>::ok) {
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
                    return CodeCvt<Character>::ok;
                case ConversionStatusFlag::sourceExhausted:
                    return CodeCvt<Character>::partial;
                case ConversionStatusFlag::sourceIllegal:
                    return CodeCvt<Character>::error;
                default:
                    AssertNotReached ();
                    return CodeCvt<Character>::error;
            }
        }
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
        requires (is_same_v<CHAR_T, wchar_t>)
        : fRep_{Private_::mk_StdCodeCvtRep_<CHAR_T> (l)}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (UnicodeExternalEncodings e)
        : fRep_{}
    {
        switch (e) {
            case UnicodeExternalEncodings::eUTF8:
                fRep_ = make_shared<UTFConvertRep_<char8_t>> (UTFConverter::kThe);
                break;
            case UnicodeExternalEncodings::eUTF16:
                fRep_ = make_shared<UTFConvertRep_<char16_t>> (UTFConverter::kThe);
                break;
            case UnicodeExternalEncodings::eUTF32:
                fRep_ = make_shared<UTFConvertRep_<char32_t>> (UTFConverter::kThe);
                break;
            default:
                AssertNotImplemented ();
        }
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
