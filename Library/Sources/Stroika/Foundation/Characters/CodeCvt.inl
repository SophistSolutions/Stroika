/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodeCvt_inl_
#define _Stroika_Foundation_Characters_CodeCvt_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <bit>

#include "../Debug/Assertions.h"
#include "../Memory/StackBuffer.h"

#include "TextConvert.h"
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
        void   ThrowErrorConvertingBytes2Characters_ (size_t nSrcCharsWhereError);
        void   ThrowErrorConvertingCharacters2Bytes_ (size_t nSrcCharsWhereError);
        void   ThrowCodePageNotSupportedException_ (CodePage cp);
        string AsNarrowSDKString_ (const String& s);
    }

    /*
     ********************************************************************************
     *********************** CodeCvt<CHAR_T>::UTFConvertRep_ ************************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
#if qCompilerAndStdLib_template_second_concept_Buggy
    template <typename SERIALIZED_CHAR_T>
#else
    template <IUNICODECanAlwaysConvertTo SERIALIZED_CHAR_T>
#endif
    struct CodeCvt<CHAR_T>::UTFConvertRep_ : CodeCvt<CHAR_T>::IRep {
        using ConversionResult           = UTFConverter::ConversionResult;
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;
        UTFConvertRep_ (const UTFConverter& utfCodeCvt)
            : fCodeConverter_{utfCodeCvt}
        {
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from));
            span<const SERIALIZED_CHAR_T> serializedFrom = ReinterpretBytes_ (*from);
            Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, to);
            if (r.fStatus == ConversionStatusFlag::sourceIllegal) {
                UTFConverter::Throw (r.fStatus, r.fSourceConsumed);
            }
            *from = from->subspan (r.fSourceConsumed); // from updated to remaining data, if any
            return to.subspan (0, r.fTargetProduced);  // point ACTUAL copied data
        }
        virtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const override
        {
            Require (to.size () >= ComputeTargetByteBufferSize (from));
            span<SERIALIZED_CHAR_T> serializedTo = ReinterpretBytes_ (to);
            ConversionResult r = fCodeConverter_.Convert (from, serializedTo); // cannot have sourceExhuasted here so no need to call ConvertQuietly
            Require (r.fSourceConsumed == from.size ());                       // always use all input characters
            return to.subspan (0, r.fTargetProduced * sizeof (SERIALIZED_CHAR_T)); // point ACTUAL copied data
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConverter::ComputeTargetBufferSize<CHAR_T, SERIALIZED_CHAR_T> (*i / sizeof (SERIALIZED_CHAR_T));
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
        /*
         *  essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
         */
        static span<const SERIALIZED_CHAR_T> ReinterpretBytes_ (span<const byte> s)
        {
            return span<const SERIALIZED_CHAR_T>{reinterpret_cast<const SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        static span<SERIALIZED_CHAR_T> ReinterpretBytes_ (span<byte> s)
        {
            return span<SERIALIZED_CHAR_T>{reinterpret_cast<SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        UTFConverter fCodeConverter_;
    };

    /*
     ********************************************************************************
     ****************** CodeCvt<CHAR_T>::UTFConvertSwappedRep_ **********************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
#if qCompilerAndStdLib_template_second_concept_Buggy
    template <typename SERIALIZED_CHAR_T>
#else
    template <IUNICODECanAlwaysConvertTo SERIALIZED_CHAR_T>
#endif
    struct CodeCvt<CHAR_T>::UTFConvertSwappedRep_ : UTFConvertRep_<SERIALIZED_CHAR_T> {
        using inherited = UTFConvertRep_<SERIALIZED_CHAR_T>;
        UTFConvertSwappedRep_ (const UTFConverter& utfCodeCvt)
            : inherited{utfCodeCvt}
        {
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= this->ComputeTargetCharacterBufferSize (*from));
            auto r = inherited::Bytes2Characters (from, to);
            for (CHAR_T& i : to) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{Memory::byteswap (i.template As<char32_t> ())};
                }
                else {
                    i = Memory::byteswap (i);
                }
            }
            return r;
        }
        virtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const override
        {
            Require (to.size () >= this->ComputeTargetByteBufferSize (from));
            Memory::StackBuffer<CHAR_T> buf{from};
            for (CHAR_T& i : buf) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    i = Character{Memory::byteswap (i.template As<char32_t> ())};
                }
                else {
                    i = Memory::byteswap (i);
                }
            }
            return inherited::Characters2Bytes (span<const CHAR_T>{buf.begin (), buf.size ()}, to);
        }
    };

    /*
     ********************************************************************************
     *********************** CodeCvt<CHAR_T>::UTF2UTFRep_ ***************************
     ********************************************************************************
     */
    /*
     * Utility rep to wrap some kind of rep along with (optional) UTFConverter, to complete
     * conversion from bytes to/from desired rep generally through some intermediary rep.
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
#if qCompilerAndStdLib_template_second_concept_Buggy
    template <typename INTERMEDIATE_CHAR_T>
#else
    template <IUNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
#endif
    struct CodeCvt<CHAR_T>::UTF2UTFRep_ : CodeCvt<CHAR_T>::IRep {
        using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;
        UTF2UTFRep_ (const CodeCvt<INTERMEDIATE_CHAR_T>& origCodeCvt)
            requires (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T))
            : fBytesVSIntermediateCvt_{origCodeCvt}
        {
        }
        UTF2UTFRep_ (const CodeCvt<INTERMEDIATE_CHAR_T>& origCodeCvt, const UTFConverter& secondStep = {})
            requires (sizeof (CHAR_T) != sizeof (INTERMEDIATE_CHAR_T))
            : fBytesVSIntermediateCvt_{origCodeCvt}
            , fIntermediateVSFinalCHARCvt_{secondStep}
        {
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from));
            if constexpr (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T)) {
                return span<CHAR_T>{to.begin (),
                                    fBytesVSIntermediateCvt_.Bytes2Characters (from, Memory::SpanReInterpretCast<INTERMEDIATE_CHAR_T> (to)).size ()};
            }
            else {
                /*
                 *  Big picture: fBytesVSIntermediateCvt_ goes bytes -> INTERMEDIATE_CHAR_T, so we use it first.
                 * 
                 *  BUT - trick - even if we successfully do first conversion (bytes -> INTERMEDIATE_CHAR_T) - we might still get a split
                 *  char on the second conversion (RARE). If so - we need to backup in 'from' - to avoid this. Just allege we consumed less. This MIGHT -
                 *  in extreme cases - go all the way back to zero.
                 */
                while (true) {
                    // Because we KNOW everything will fit (disallow target exhuasted), we can allocate a temporary buffer for the intermediate state, and be done with
                    // it by the end of this routine (stay stateless)
                    Memory::StackBuffer<INTERMEDIATE_CHAR_T> intermediateBuf{fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (*from)};
                    span<const INTERMEDIATE_CHAR_T> intermediateSpan = fBytesVSIntermediateCvt_.Bytes2Characters (from, intermediateBuf); // shortens 'from' if needed

                    // then use fIntermediateVSFinalCHARCvt_ to perform final mapping INTERMEDIATE_CHAR_T -> CHAR_T
                    ConversionResultWithStatus cr = fIntermediateVSFinalCHARCvt_.ConvertQuietly (intermediateSpan, to);
                    switch (cr.fStatus) {
                        case ConversionStatusFlag::sourceIllegal:
                            UTFConverter::Throw (cr.fStatus, cr.fSourceConsumed);
                        case ConversionStatusFlag::sourceExhausted:
                            // TRICKY - if we have at least one character output, then we need to back out bytes 'from' - til this doesn't happen
                            if (not from->empty ()) {
                                *from = from->subspan (0, from->size () - 1);
                                continue; // 'goto try again'
                            }
                            else {
                                return span<CHAR_T>{}; // no update to 'from' since we consumed no characters
                            }
                        case ConversionStatusFlag::ok:
                            return to.subspan (0, cr.fTargetProduced);
                        default:
                            AssertNotReached ();
                            return span<CHAR_T>{};
                    }
                }
            }
        }
        virtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const override
        {
            Require (to.size () >= ComputeTargetByteBufferSize (from));
            if constexpr (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T)) {
                return fBytesVSIntermediateCvt_.Characters2Bytes (Memory::SpanReInterpretCast<const INTERMEDIATE_CHAR_T> (from), to);
            }
            else {
                /*
                 *  Because we KNOW everything will fit, we can allocate a temporary buffer for the intermediate state, and be done with
                 *  it by the end of this routine (stay stateless)
                 */
                Memory::StackBuffer<INTERMEDIATE_CHAR_T> intermediateBuf{
                    fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T> (from)};

                /*
                 *  first translate to something usable by fBytesVSIntermediateCvt_
                 */
                span<INTERMEDIATE_CHAR_T> intermediateSpan =
                    fIntermediateVSFinalCHARCvt_.ConvertSpan (from, span<INTERMEDIATE_CHAR_T>{intermediateBuf.data (), intermediateBuf.size ()});

                // Then use fBytesVSIntermediateCvt_, no need to track anything in intermediateBuf, we require all used, no partials etc.
                return fBytesVSIntermediateCvt_.Characters2Bytes (intermediateSpan, to);
            }
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            size_t intermediateCharCntMax = [&] () {
                if (const size_t* i = get_if<size_t> (&src)) {
                    return fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (*i);
                }
                else {
                    return fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (get<span<const byte>> (src));
                }
            }();
            if constexpr (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T)) {
                return intermediateCharCntMax;
            }
            else {
                return fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T, CHAR_T> (intermediateCharCntMax);
            }
        }
        virtual size_t ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const override
        {
            size_t intermediateCharCntMax = [&] () {
                if constexpr (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T)) {
                    if (const size_t* i = get_if<size_t> (&src)) {
                        return *i;
                    }
                    else {
                        return get<span<const CHAR_T>> (src).size ();
                    }
                }
                else {
                    if (const size_t* i = get_if<size_t> (&src)) {
                        fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T, CHAR_T> (*i);
                    }
                    else {
                        fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T> (get<span<const CHAR_T>> (src));
                    }
                }
            }();
            return fBytesVSIntermediateCvt_.ComputeTargetByteBufferSize (intermediateCharCntMax);
        }
        CodeCvt<INTERMEDIATE_CHAR_T> fBytesVSIntermediateCvt_;
        conditional_t<sizeof (CHAR_T) != sizeof (INTERMEDIATE_CHAR_T), UTFConverter, byte> fIntermediateVSFinalCHARCvt_; // would like to remove field if sizeof ==, but not sure how (void doesnt work)
    };

    /*
     *  This is crazy complicated because codecvt goes out of its way to be hard to copy, hard to move, but with
     *  a little care, can be made to work with unique_ptr.
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <typename STD_CODE_CVT_T>
    struct CodeCvt<CHAR_T>::CodeCvt_WrapStdCodeCvt_ : CodeCvt<CHAR_T>::IRep {
        unique_ptr<STD_CODE_CVT_T> fCodeCvt_;
        using extern_type = typename STD_CODE_CVT_T::extern_type;
        static_assert (is_same_v<CHAR_T, typename STD_CODE_CVT_T::intern_type>);
        CodeCvt_WrapStdCodeCvt_ (unique_ptr<STD_CODE_CVT_T>&& codeCvt)
            : fCodeCvt_{move (codeCvt)}
        {
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from));
            const extern_type* _First1 = reinterpret_cast<const extern_type*> (from->data ());
            const extern_type* _Last1  = _First1 + from->size ();
            const extern_type* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            CHAR_T*            _First2 = to.data ();
            CHAR_T*            _Last2  = _First2 + to.size ();
            CHAR_T*            _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            mbstate_t          ignoredMBState{};
            auto               r = fCodeCvt_->in (ignoredMBState, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            if (r == STD_CODE_CVT_T::partial) {
                *from = from->subspan (static_cast<size_t> (_Mid2 - _First2)); // reference remaining bytes, could be partial character at end of multibyte sequence
                Assert (from->size () != 0);
            }
            else if (r != STD_CODE_CVT_T::ok) {
                Private_::ThrowErrorConvertingBytes2Characters_ (_Mid1 - _First1);
            }
            else {
                Require (_Mid1 == _Last1);
                *from = span<const byte>{}; // used all input
            }
            return to.subspan (0, _Mid2 - _First2); // point ACTUAL copied data
        }
        virtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const override
        {
            Require (to.size () >= ComputeTargetByteBufferSize (from));
            const CHAR_T* _First1 = from.data ();
            const CHAR_T* _Last1  = _First1 + from.size ();
            const CHAR_T* _Mid1   = _First1; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            extern_type*  _First2 = reinterpret_cast<extern_type*> (to.data ());
            extern_type*  _Last2  = _First2 + to.size ();
            extern_type*  _Mid2   = _First2; // DOUBLE CHECK SPEC - NOT SURE IF THIS IS USED ON INPUT
            mbstate_t     ignoredMBState{};
            auto          r = fCodeCvt_->out (ignoredMBState, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            if (r != STD_CODE_CVT_T::ok) {
                Private_::ThrowErrorConvertingCharacters2Bytes_ (_Mid1 - _First1);
            }
            Require (_Mid1 == _Last1);              // used all input
            return to.subspan (0, _Mid2 - _First2); // point ACTUAL copied data
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

    namespace Private_ {
        // a lot of old, important character sets can be represented this way (like old PC character sets for non-asian languages)
        struct BuiltinSingleByteTableCodePageRep_ final : CodeCvt<char16_t>::IRep {
            BuiltinSingleByteTableCodePageRep_ (CodePage cp);
            virtual ~BuiltinSingleByteTableCodePageRep_ () = default;
            virtual span<char16_t> Bytes2Characters (span<const byte>* from, span<char16_t> to) const override;
            virtual span<byte>     Characters2Bytes (span<const char16_t> from, span<byte> to) const override;
            virtual size_t         ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override;
            virtual size_t         ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const override;
            const char16_t*        fMap_;
        };
#if qPlatform_Windows
        struct WindowsNative_ final : CodeCvt<char16_t>::IRep {
            constexpr WindowsNative_ (CodePage cp)
                : fCodePage_{cp}
            {
            }
            virtual ~WindowsNative_ () = default;
            virtual span<char16_t> Bytes2Characters (span<const byte>* from, span<char16_t> to) const override;
            virtual span<byte>     Characters2Bytes (span<const char16_t> from, span<byte> to) const override;
            virtual size_t         ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override;
            virtual size_t         ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const override;
            CodePage               fCodePage_;
        };
#endif
    }

    /*
     ********************************************************************************
     ******************************* CodeCvt<CHAR_T> ********************************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt ()
        : fRep_{make_shared<UTFConvertRep_<char8_t>> (UTFConverter::kThe)} // default, is to serialize to UTF-8
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l)
        : CodeCvt{l.name ()}
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (const String& localeName)
    {
        string ln = Private_::AsNarrowSDKString_ (localeName);
        if constexpr (is_same_v<CHAR_T, wchar_t>) {
            *this = mkFromStdCodeCvt<codecvt_byname<wchar_t, char, mbstate_t>> (ln);
        }
        else if constexpr (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>) {
            *this = mkFromStdCodeCvt<codecvt_byname<CHAR_T, char8_t, mbstate_t>> (ln);
        }
        else if constexpr (is_same_v<CHAR_T, Character>) {
            fRep_ = make_shared<UTF2UTFRep_<char32_t>> (CodeCvt<char32_t>::mkFromStdCodeCvt<codecvt_byname<char32_t, char8_t, mbstate_t>> (ln));
        }
        else {
            // CHAR_T COULD be UTF-8, but not clear if/why that would be useful.
            AssertNotImplemented ();
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (UnicodeExternalEncodings e)
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
            default:
                AssertNotImplemented ();
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (span<const byte>* guessFormatFrom, const optional<CodeCvt>& useElse)
        : fRep_{}
    {
        RequireNotNull (guessFormatFrom);
        if (optional<tuple<UnicodeExternalEncodings, size_t>> r = ReadByteOrderMark (*guessFormatFrom)) {
            *guessFormatFrom = guessFormatFrom->subspan (get<size_t> (*r));
            fRep_            = CodeCvt{get<UnicodeExternalEncodings> (*r)}.fRep_;
        }
        else {
            fRep_ = useElse ? useElse->fRep_ : CodeCvt{}.fRep_;
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (CodePage cp)
        : fRep_{}
    {
        // A few we have builtin table converters for (BuiltinSingleByteTableCodePageRep_);
        // a few are just UTF, and we can convert those.
        // On windows, we can delegate to WindowsNative_
        // else give up and throw not supported code page.
        switch (cp) {
            case WellKnownCodePages::kANSI:
            case WellKnownCodePages::kMAC:
            case WellKnownCodePages::kPC:
            case WellKnownCodePages::kPCA:
            case WellKnownCodePages::kGreek:
            case WellKnownCodePages::kTurkish:
            case WellKnownCodePages::kHebrew:
            case WellKnownCodePages::kArabic:
                fRep_ = make_shared<UTF2UTFRep_<char16_t>> (CodeCvt<char16_t> (make_shared<Private_::BuiltinSingleByteTableCodePageRep_> (cp)));
                break;
            case WellKnownCodePages::kUTF8:
                fRep_ = make_shared<UTFConvertRep_<char8_t>> (UTFConverter::kThe);
                break;
            case WellKnownCodePages::kUNICODE_WIDE:
                fRep_ = make_shared<UTFConvertRep_<char16_t>> (UTFConverter::kThe);
                break;
            case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN:
                fRep_ = make_shared<UTFConvertSwappedRep_<char16_t>> (UTFConverter::kThe);
                break;
            default:
#if qPlatform_Windows
                fRep_ = make_shared<UTF2UTFRep_<char16_t>> (CodeCvt<char16_t> (make_shared<Private_::WindowsNative_> (cp)));
                break;
#else
                Private_::ThrowCodePageNotSupportedException_ (cp);
#endif
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <IUNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const CodeCvt<INTERMEDIATE_CHAR_T>& basedOn)
        : fRep_{make_shared<UTF2UTFRep_<INTERMEDIATE_CHAR_T>> (basedOn)}
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const shared_ptr<IRep>& rep)
        : fRep_{(RequireNotNull (rep), rep)}
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <IStdCodeCVT STD_CODECVT, typename... ARGS>
    inline CodeCvt<CHAR_T> CodeCvt<CHAR_T>::mkFromStdCodeCvt (ARGS... args)
        requires (is_same_v<CHAR_T, typename STD_CODECVT::intern_type>)
    {
        auto u = make_unique<Private_::deletable_facet_<STD_CODECVT>> (forward<ARGS> (args)...);
        return CodeCvt<CHAR_T>{make_shared<CodeCvt_WrapStdCodeCvt_<Private_::deletable_facet_<STD_CODECVT>>> (move (u))};
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte> from) const -> size_t
    {
        //quickie reference impl @todo fix/optimize
        Memory::StackBuffer<CHAR_T> to{ComputeTargetCharacterBufferSize (from)};
        return fRep_->Bytes2Characters (&from, span{to}).size ();
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const -> span<CHAR_T>
    {
        RequireNotNull (from);
        AssertNotNull (fRep_);
        Require (to.size () >= ComputeTargetCharacterBufferSize (*from) or to.size () >= Bytes2Characters (*from)); // ComputeTargetCharacterBufferSize cheaper to compute
        return fRep_->Bytes2Characters (from, to);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Characters2Bytes (span<const CHAR_T> from) const -> size_t
    {
        // quickie reference impl
        Memory::StackBuffer<byte> to{ComputeTargetByteBufferSize (from)};
        return fRep_->Characters2Bytes (from, span{to}).size ();
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Characters2Bytes (span<const CHAR_T> from, span<byte> to) const -> span<byte>
    {
        AssertNotNull (fRep_);
        Require (to.size () >= ComputeTargetByteBufferSize (from) or to.size () >= Characters2Bytes (from)); // ComputeTargetByteBufferSize cheaper to compute
        return fRep_->Characters2Bytes (from, to);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetCharacterBufferSize (span<const byte> src) const
    {
        return fRep_->ComputeTargetCharacterBufferSize (src);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetCharacterBufferSize (size_t srcSize) const
    {
        return fRep_->ComputeTargetCharacterBufferSize (srcSize);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetByteBufferSize (span<const CHAR_T> src) const
    {
        return fRep_->ComputeTargetByteBufferSize (src);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline size_t CodeCvt<CHAR_T>::ComputeTargetByteBufferSize (size_t srcSize) const
    {
        return fRep_->ComputeTargetByteBufferSize (srcSize);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <typename STRINGISH>
    STRINGISH CodeCvt<CHAR_T>::Bytes2String (span<const byte> from) const
    {
        Memory::StackBuffer<CHAR_T> buf{this->ComputeTargetCharacterBufferSize (from)};
        span<CHAR_T>                r = this->Bytes2Characters (&from, span{buf});
        if (not from.empty ()) {
            /// THROW SPLIT CHAR... BAD CHAR - @todo
        }
        return STRINGISH{r.begin (), r.end ()};
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <typename BLOBISH>
    BLOBISH CodeCvt<CHAR_T>::String2Bytes (span<const CHAR_T> from) const
    {
        Memory::StackBuffer<byte> buf{Memory::eUninitialized, this->ComputeTargetByteBufferSize (from)};
        const span<const byte>    r = this->Characters2Bytes (from, span{buf});
        if constexpr (same_as<BLOBISH, string>) {
            return string{reinterpret_cast<const char*> (r.data ()), reinterpret_cast<const char*> (r.data ()) + r.size ()};
        }
        else {
            return BLOBISH{r.begin (), r.end ()};
        }
    }

}

#endif /*_Stroika_Foundation_Characters_CodeCvt_inl_*/
