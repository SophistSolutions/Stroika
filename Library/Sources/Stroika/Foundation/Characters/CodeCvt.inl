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

    /*
     ********************************************************************************
     *********************** CodeCvt<CHAR_T>::UTFConvertRep_ ************************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename SERIALIZED_CHAR_T>
    struct CodeCvt<CHAR_T>::UTFConvertRep_ : CodeCvt<CHAR_T>::IRep {
        using ConversionResult = UTFConverter::ConversionResult;
        UTFConvertRep_ (const UTFConverter& utfCodeCvt)
            : fCodeConverter_{utfCodeCvt}
        {
        }
        static span<const SERIALIZED_CHAR_T> ReinterpretBytes_ (span<const byte> s)
        {
            return span<const SERIALIZED_CHAR_T>{reinterpret_cast<const SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        static span<SERIALIZED_CHAR_T> ReinterpretBytes_ (span<byte> s)
        {
            return span<SERIALIZED_CHAR_T>{reinterpret_cast<SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        virtual void Bytes2Characters (span<const byte> from, span<CHAR_T>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= ComputeTargetCharacterBufferSize (from));

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<const SERIALIZED_CHAR_T> serializedFrom = ReinterpretBytes_ (from);
            Assert (serializedFrom.size_bytes () <= from.size ()); // note - serializedFrom could be smaller than from in bytespan
            ConversionResult r = fCodeConverter_.Convert (serializedFrom, *to);
            Require (r.fSourceConsumed == from.size ());
            *to = to->subspan (0, r.fTargetProduced); // point ACTUAL copied data
        }
        virtual void Characters2Bytes (span<const CHAR_T> from, span<byte>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= ComputeTargetByteBufferSize (from));

            // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
            span<SERIALIZED_CHAR_T> serializedTo = ReinterpretBytes_ (*to);
            ConversionResult        r            = fCodeConverter_.ConvertQuietly (from, serializedTo);
            Require (r.fSourceConsumed == from.size ());
            *to = to->subspan (0, r.fTargetProduced * sizeof (SERIALIZED_CHAR_T)); // point ACTUAL copied data
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConverter::ComputeTargetBufferSize<CHAR_T, SERIALIZED_CHAR_T> (*i);
            }
            else {
                return UTFConverter::ComputeTargetBufferSize<CHAR_T> (ReinterpretBytes_ (get<span<const byte>> (src)));
            }
        }
        virtual size_t ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConverter::ComputeTargetBufferSize<SERIALIZED_CHAR_T, CHAR_T> (*i) * sizeof (SERIALIZED_CHAR_T);
            }
            else {
                return UTFConverter::ComputeTargetBufferSize<SERIALIZED_CHAR_T> (get<span<const CHAR_T>> (src)) * sizeof (SERIALIZED_CHAR_T);
            }
        }
        UTFConverter fCodeConverter_;
    };

    /*
     ********************************************************************************
     ****************** CodeCvt<CHAR_T>::UTFConvertSwappedRep_ **********************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename SERIALIZED_CHAR_T>
    struct CodeCvt<CHAR_T>::UTFConvertSwappedRep_ : UTFConvertRep_<SERIALIZED_CHAR_T> {
        using inherited = UTFConvertRep_<SERIALIZED_CHAR_T>;

        UTFConvertSwappedRep_ (const UTFConverter& utfCodeCvt)
            : inherited{utfCodeCvt}
        {
        }
        virtual void Bytes2Characters (span<const byte> from, span<CHAR_T>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= this->ComputeTargetCharacterBufferSize (from));
            inherited::Bytes2Characters (from, to);
            for (CHAR_T& i : *to) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{Memory::byteswap (i.template As<char32_t> ())};
                }
                else {
                    i = Memory::byteswap (i);
                }
            }
        }
        virtual void Characters2Bytes (span<const CHAR_T> from, span<byte>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= this->ComputeTargetByteBufferSize (from));
            Memory::StackBuffer<CHAR_T> buf{from};
            for (CHAR_T& i : buf) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{Memory::byteswap (i.template As<char32_t> ())};
                }
                else {
                    i = Memory::byteswap (i);
                }
            }
            auto useFrom = span<const CHAR_T>{buf.begin (), buf.size ()};
            inherited::Characters2Bytes (useFrom, to);
        }
    };

    /*
     ********************************************************************************
     *********************** CodeCvt<CHAR_T>::UTF2UTFRep_ ***************************
     ********************************************************************************
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename INTERMEDIATE_CHAR_T>
    struct CodeCvt<CHAR_T>::UTF2UTFRep_ : CodeCvt<CHAR_T>::IRep {
        using result                     = typename CodeCvt<CHAR_T>::result;
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;
        static_assert (sizeof (CHAR_T) != sizeof (INTERMEDIATE_CHAR_T)); // use another rep for that case
        UTF2UTFRep_ (const CodeCvt<INTERMEDIATE_CHAR_T>& origCodeCvt, const UTFConverter& secondStep)
            : fBytesVSIntermediateCvt_{origCodeCvt}
            , fIntermediateVSFinalCHARCvt_{secondStep}
        {
        }
        virtual void Bytes2Characters (span<const byte> from, span<CHAR_T>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= ComputeTargetCharacterBufferSize (from));

            /*
             *  Big picture: fBytesVSIntermediateCvt_ goes bytes <--> INTERMEDIATE_CHAR_T, so we use it first.
             */

            // Because we KNOW everything will fit, we can allocate a temporary buffer for the intermediate state, and be done with
            // it by the end of this routine (stay stateless)
            Memory::StackBuffer<INTERMEDIATE_CHAR_T> intermediateBuf{fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (from)};
            span<INTERMEDIATE_CHAR_T>                intermediateSpan{intermediateBuf.data (), intermediateBuf.size ()};

            fBytesVSIntermediateCvt_.Characters2Bytes (from, &intermediateBuf);

            // then use fBytesVSIntermediateCvt_ to perform final mapping
            *to = fBytesVSIntermediateCvt_.Convert (intermediateBuf, *to);
        }
        virtual result Characters2Bytes (span<const CHAR_T> from, span<byte>* to) const override
        {
            RequireNotNull (to);
            Require (to->size () >= ComputeTargetByteBufferSize (from));

            /*
             *  Because we KNOW everything will fit, we can allocate a temporary buffer for the intermediate state, and be done with
             *  it by the end of this routine (stay stateless)
             */
            Memory::StackBuffer<INTERMEDIATE_CHAR_T> intermediateBuf{fIntermediateVSFinalCHARCvt_.ComputeTargetBufferSize<INTERMEDIATE_CHAR_T> (from)};

            /*
             *  first translate to something usable by fBytesVSIntermediateCvt_
             */
            span<INTERMEDIATE_CHAR_T> intermediateSpan =
                fIntermediateVSFinalCHARCvt_.Convert (from, span<INTERMEDIATE_CHAR_T>{intermediateBuf.data (), intermediateBuf.size ()});

            // Then use fBytesVSIntermediateCvt_, no need to track anything in intermediateBuf, we require all used, no partials etc.
            fBytesVSIntermediateCvt_.Characters2Bytes (intermediateBuf, to);
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            // @todo fix wrong (INSIDE OUT) - other case ComputeTargetByteBufferSize is right
            if (const size_t* i = get_if<size_t> (&src)) {
                return fBytesVSIntermediateCvt_.ComputeTargetByteBufferSize (
                    fIntermediateVSFinalCHARCvt_.ComputeTargetBufferSize<INTERMEDIATE_CHAR_T, CHAR_T> (*i));
            }
            else {
                fIntermediateVSFinalCHARCvt_.ComputeTargetBufferSize<CHAR_T, INTERMEDIATE_CHAR_T> (
                    fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (src));
            }
        }
        virtual size_t ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return fBytesVSIntermediateCvt_.ComputeTargetByteBufferSize (
                    fIntermediateVSFinalCHARCvt_.ComputeTargetBufferSize<INTERMEDIATE_CHAR_T, CHAR_T> (*i));
            }
            else {
                return fBytesVSIntermediateCvt_.ComputeTargetByteBufferSize (
                    fIntermediateVSFinalCHARCvt_.ComputeTargetBufferSize<INTERMEDIATE_CHAR_T> (get<span<const CHAR_T>> (src)));
            }
        }
        CodeCvt<INTERMEDIATE_CHAR_T> fBytesVSIntermediateCvt_;
        UTFConverter                 fIntermediateVSFinalCHARCvt_;
    };

    /*
     *  This is crazy complicated because codecvt goes out of its way to be hard to copy, hard to move, but with
     *  a little care, can be made to work with unique_ptr.
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <typename STD_CODE_CVT_T>
    struct CodeCvt<CHAR_T>::CodeCvt_WrapStdCodeCvt_ : CodeCvt<CHAR_T>::IRep {
        unique_ptr<STD_CODE_CVT_T> fCodeCvt_;
        using result      = typename CodeCvt<CHAR_T>::result;
        using extern_type = typename STD_CODE_CVT_T::extern_type;
        static_assert (is_same_v<CHAR_T, typename STD_CODE_CVT_T::intern_type>);
        CodeCvt_WrapStdCodeCvt_ (unique_ptr<STD_CODE_CVT_T>&& codeCvt)
            : fCodeCvt_{move (codeCvt)}
        {
        }
        virtual void Bytes2Characters (span<const byte> from, span<CHAR_T>* to) const override
        {
            Require (to->size () >= ComputeTargetCharacterBufferSize (from));
            const extern_type* _First1 = reinterpret_cast<const extern_type*> (from.data ());
            const extern_type* _Last1  = _First1 + from.size ();
            const extern_type* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            CHAR_T*            _First2 = to->data ();
            CHAR_T*            _Last2  = _First2 + to->size ();
            CHAR_T*            _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            mbstate_t          ignoredMBState{};
            auto               r = fCodeCvt_->in (ignoredMBState, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            if (r != STD_CODE_CVT_T::ok) {
                //static const auto kException_ = Execution::RuntimeErrorException{"Error converting bytes to characters"sv};
                // Execution::Throw (kException_);
            }
            Require (_Mid1 == _Last1);              // used all input
            *to = to->subspan (0, _Mid2 - _First2); // point ACTUAL copied data
        }
        virtual void Characters2Bytes (span<const CHAR_T> from, span<byte>* to) const override
        {
            Require (to->size () >= ComputeTargetByteBufferSize (from));
            const CHAR_T* _First1 = from.data ();
            const CHAR_T* _Last1  = _First1 + from.size ();
            const CHAR_T* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            extern_type*  _First2 = reinterpret_cast<extern_type*> (to->data ());
            extern_type*  _Last2  = _First2 + to->size ();
            extern_type*  _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            mbstate_t     ignoredMBState{};
            auto          r = fCodeCvt_->out (ignoredMBState, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            if (r != STD_CODE_CVT_T::ok) {
                //static const auto kException_ = Execution::RuntimeErrorException{"Error converting characters to output format"sv};
                // Execution::Throw (kException_);
            }
            Require (_Mid1 == _Last1);              // used all input
            *to = to->subspan (0, _Mid2 - _First2); // point ACTUAL copied data
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            // at most one character per byte, and std::codecvt doesn't appear to offer API to compute better
            if (const size_t* i = get_if<size_t> (&src)) {
                return *i;
            }
            else {
                return get<span<const byte>> (src).size ();
            }
        }
        virtual size_t ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return (*i) * fCodeCvt_->max_length ();
            }
            else {
                // std::codecvt doesn't appear to provide an API to compute needed buffer length (just the reverse -
                // for a buffer length, how many bytes consumed).
                return get<span<const CHAR_T>> (src).size () * fCodeCvt_->max_length ();
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
    {
        if constexpr (sizeof (CHAR_T) == 1) {
            fRep_ = make_shared<UTFConvertRep_<char8_t>> (UTFConverter::kThe);
        }
        else if constexpr (sizeof (CHAR_T) == 2) {
            fRep_ = make_shared<UTFConvertRep_<char16_t>> (UTFConverter::kThe);
        }
        else if constexpr (sizeof (CHAR_T) == 4) {
            fRep_ = make_shared<UTFConvertRep_<char32_t>> (UTFConverter::kThe);
        }
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l)
    {
        auto baseRep = make_shared<CodeCvt_WrapStdCodeCvt_<Private_::deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>>> (l);
        if constexpr (is_same_v<CHAR_T, wchar_t>) {
            fRep_ = move (baseRep);
        }
        else if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
            fRep_ = reinterpret_pointer_cast<IRep> (baseRep);
        }
        else {
            fRep_ = make_shared<UTF2UTFRep_<wchar_t>> (baseRep);
        }
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    template <IsStdCodeCVTT STD_CODECVT, typename... ARGS>
    inline CodeCvt<CHAR_T>::CodeCvt (ARGS... args)
        : fRep_{make_shared<CodeCvt_WrapStdCodeCvt_<Private_::deletable_facet_<STD_CODECVT>>> (forward<ARGS> (args)...)}
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
    template <Character_UNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const CodeCvt<INTERMEDIATE_CHAR_T>& basedOn)
        : fRep_{make_shared<UTF2UTFRep_<INTERMEDIATE_CHAR_T>> (basedOn)}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const shared_ptr<IRep>& rep)
        : fRep_{(RequireNotNull (rep), rep)}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte> from, span<CHAR_T> to) const -> span<CHAR_T>
    {
        AssertNotNull (fRep_);
        Require (to->size () >= ComputeTargetCharacterBufferSize (from));
        fRep_->Bytes2Characters (from, &to);
        return to;
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Characters2Bytes (span<const CHAR_T> from, span<byte> to) const -> span<byte>
    {
        AssertNotNull (fRep_);
        Require (to.size () >= ComputeTargetByteBufferSize (from));
        fRep_->Characters2Bytes (from, &to);
        return to;
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetCharacterBufferSize (span<const byte> src) const
    {
        return fRep_->ComputeTargetCharacterBufferSize (src);
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetCharacterBufferSize (size_t srcSize) const
    {
        return fRep_->ComputeTargetCharacterBufferSize (srcSize);
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetByteBufferSize (span<const CHAR_T> src) const
    {
        return fRep_->ComputeTargetByteBufferSize (src);
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetByteBufferSize (size_t srcSize) const
    {
        return fRep_->ComputeTargetByteBufferSize (srcSize);
    }

}

#endif /*_Stroika_Foundation_Characters_CodeCvt_inl_*/
