/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodeCvt_inl_
#define _Stroika_Foundation_Characters_CodeCvt_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <bit>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

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
            ~deletable_facet_ () = default;
        };
        void   ThrowErrorConvertingBytes2Characters_ (size_t nSrcCharsWhereError);
        void   ThrowErrorConvertingCharacters2Bytes_ (size_t nSrcCharsWhereError);
        void   ThrowCodePageNotSupportedException_ (CodePage cp);
        void   ThrowCharsetNotSupportedException_ (const Charset& charset);
        void   ThrowInvalidCharacterProvidedDoesntFitWithProvidedCodeCvt_ ();
        string AsNarrowSDKString_ (const String& s);
    }

    /*
     ********************************************************************************
     ***************************** CodeCvt<CHAR_T>::IRep ****************************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    size_t CodeCvt<CHAR_T>::IRep::_Bytes2Characters (span<const byte> from) const
    {
        Memory::StackBuffer<CHAR_T> to{this->ComputeTargetCharacterBufferSize (from)};
        return this->Bytes2Characters (&from, span{to}).size ();
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    size_t CodeCvt<CHAR_T>::IRep::_Characters2Bytes (span<const CHAR_T> from) const
    {
        Memory::StackBuffer<byte> to{this->ComputeTargetByteBufferSize (from)};
        return this->Characters2Bytes (from, span{to}).size ();
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
        using ConversionResult           = UTFConvert::ConversionResult;
        using ConversionResultWithStatus = UTFConvert::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConvert::ConversionStatusFlag;
        UTFConvertRep_ (const Options& o)
            : fCodeConverter_{o.fInvalidCharacterReplacement
                                  ? UTFConvert{UTFConvert::Options{.fInvalidCharacterReplacement = *o.fInvalidCharacterReplacement}}
                                  : UTFConvert::kThe}
        {
        }
        virtual Options GetOptions () const override
        {
            return Options{.fInvalidCharacterReplacement = fCodeConverter_.GetOptions ().fInvalidCharacterReplacement};
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from));
            span<const SERIALIZED_CHAR_T> serializedFrom = ReinterpretBytes_ (*from);
            Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, to);
            if (r.fStatus == ConversionStatusFlag::sourceIllegal) {
                UTFConvert::Throw (r.fStatus, r.fSourceConsumed);
            }
            *from = from->subspan (r.fSourceConsumed); // from updated to remaining data, if any
            return to.subspan (0, r.fTargetProduced);  // point ACTUAL copied data
        }
        virtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const override
        {
            Require (to.size () >= ComputeTargetByteBufferSize (from));
            span<SERIALIZED_CHAR_T> serializedTo = ReinterpretBytes_ (to);
            ConversionResult r = fCodeConverter_.Convert (from, serializedTo); // cannot have sourceExhausted here so no need to call ConvertQuietly
            Require (r.fSourceConsumed == from.size ());                       // always use all input characters
            return to.subspan (0, r.fTargetProduced * sizeof (SERIALIZED_CHAR_T)); // point ACTUAL copied data
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConvert::ComputeTargetBufferSize<CHAR_T, SERIALIZED_CHAR_T> (*i / sizeof (SERIALIZED_CHAR_T));
            }
            else {
                return UTFConvert::ComputeTargetBufferSize<CHAR_T> (ReinterpretBytes_ (get<span<const byte>> (src)));
            }
        }
        virtual size_t ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConvert::ComputeTargetBufferSize<SERIALIZED_CHAR_T, CHAR_T> (*i) * sizeof (SERIALIZED_CHAR_T);
            }
            else {
                return UTFConvert::ComputeTargetBufferSize<SERIALIZED_CHAR_T> (get<span<const CHAR_T>> (src)) * sizeof (SERIALIZED_CHAR_T);
            }
        }
        /*
         *  essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConvert)
         */
        static span<const SERIALIZED_CHAR_T> ReinterpretBytes_ (span<const byte> s)
        {
            return span<const SERIALIZED_CHAR_T>{reinterpret_cast<const SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        static span<SERIALIZED_CHAR_T> ReinterpretBytes_ (span<byte> s)
        {
            return span<SERIALIZED_CHAR_T>{reinterpret_cast<SERIALIZED_CHAR_T*> (s.data ()), s.size () / sizeof (SERIALIZED_CHAR_T)};
        }
        UTFConvert fCodeConverter_;
    };

    /*
     ********************************************************************************
     ********************* CodeCvt<CHAR_T>::Latin1ConvertRep_ ***********************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    struct CodeCvt<CHAR_T>::Latin1ConvertRep_ : CodeCvt<CHAR_T>::IRep {
        using ConversionResult           = UTFConvert::ConversionResult;
        using ConversionResultWithStatus = UTFConvert::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConvert::ConversionStatusFlag;
        Latin1ConvertRep_ (const Options& o)
            : fCodeConverter_{o.fInvalidCharacterReplacement
                                  ? UTFConvert{UTFConvert::Options{.fInvalidCharacterReplacement = *o.fInvalidCharacterReplacement}}
                                  : UTFConvert::kThe}
        {
        }
        virtual Options GetOptions () const override
        {
            return Options{.fInvalidCharacterReplacement = fCodeConverter_.GetOptions ().fInvalidCharacterReplacement};
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from));
            span<const Latin1> serializedFrom = ReinterpretBytes_ (*from);
            Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan
            ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, to);
            if (r.fStatus == ConversionStatusFlag::sourceIllegal) {
                UTFConvert::Throw (r.fStatus, r.fSourceConsumed);
            }
            *from = from->subspan (r.fSourceConsumed); // from updated to remaining data, if any
            return to.subspan (0, r.fTargetProduced);  // point ACTUAL copied data
        }
        virtual span<byte> Characters2Bytes ([[maybe_unused]] span<const CHAR_T> from, [[maybe_unused]] span<byte> to) const override
        {
            RequireNotReached (); // doesn't work in general, so disallow
            return span<byte>{};
        }
        virtual size_t ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override
        {
            if (const size_t* i = get_if<size_t> (&src)) {
                return UTFConvert::ComputeTargetBufferSize<CHAR_T, Latin1> (*i / sizeof (Latin1));
            }
            else {
                return UTFConvert::ComputeTargetBufferSize<CHAR_T> (ReinterpretBytes_ (get<span<const byte>> (src)));
            }
        }
        virtual size_t ComputeTargetByteBufferSize ([[maybe_unused]] variant<span<const CHAR_T>, size_t> src) const override
        {
            RequireNotReached (); // doesn't work in general, so disallow
            return 0;
        }
        /*
         *  essentially 'cast' from bytes to from Latin1 (could be char8_t, char16_t or whatever works with UTFConvert)
         */
        static span<const Latin1> ReinterpretBytes_ (span<const byte> s)
        {
            return span<const Latin1>{reinterpret_cast<const Latin1*> (s.data ()), s.size () / sizeof (Latin1)};
        }
        static span<Latin1> ReinterpretBytes_ (span<byte> s)
        {
            return span<Latin1>{reinterpret_cast<Latin1*> (s.data ()), s.size () / sizeof (Latin1)};
        }
        UTFConvert fCodeConverter_;
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
        UTFConvertSwappedRep_ (const Options& o)
            : inherited{o}
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
     * Utility rep to wrap some kind of rep along with (optional) UTFConvert, to complete
     * conversion from bytes to/from desired rep generally through some intermediary rep.
     * 
     *  NOTE - this code allows INTERMEDIATE_CHAR_T == CHAR_T special case, and is optimized to do
     *  nothing for that case (or should be - maybe needs a bit more tweaking of implementation for that to be fully true).
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
#if qCompilerAndStdLib_template_second_concept_Buggy
    template <typename INTERMEDIATE_CHAR_T>
#else
    template <IUNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
#endif
    struct CodeCvt<CHAR_T>::UTF2UTFRep_ : CodeCvt<CHAR_T>::IRep {
        using ConversionResultWithStatus = UTFConvert::ConversionResultWithStatus;
        using ConversionStatusFlag       = UTFConvert::ConversionStatusFlag;
        UTF2UTFRep_ (const CodeCvt<INTERMEDIATE_CHAR_T>& origCodeCvt)
            requires (sizeof (CHAR_T) == sizeof (INTERMEDIATE_CHAR_T))
            : fBytesVSIntermediateCvt_{origCodeCvt}
        {
        }
        UTF2UTFRep_ (const CodeCvt<INTERMEDIATE_CHAR_T>& origCodeCvt, const UTFConvert& secondStep = {})
            requires (sizeof (CHAR_T) != sizeof (INTERMEDIATE_CHAR_T))
            : fBytesVSIntermediateCvt_{origCodeCvt}
            , fIntermediateVSFinalCHARCvt_{secondStep}
        {
        }
        virtual Options GetOptions () const override
        {
            // Not 100% right cuz ignoring  - fIntermediateVSFinalCHARCvt_ - LGP - 2023-08-07
            return Options{.fInvalidCharacterReplacement = fBytesVSIntermediateCvt_.GetOptions ().fInvalidCharacterReplacement};
        }
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const override
        {
            RequireNotNull (from);
            Require (to.size () >= ComputeTargetCharacterBufferSize (*from) or to.size () >= this->_Bytes2Characters (*from));
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
                    // Because we KNOW everything will fit (disallow target exhausted), we can allocate a temporary buffer for the intermediate state, and be done with
                    // it by the end of this routine (stay stateless)
                    Memory::StackBuffer<INTERMEDIATE_CHAR_T> intermediateBuf{fBytesVSIntermediateCvt_.ComputeTargetCharacterBufferSize (*from)};
                    span<const INTERMEDIATE_CHAR_T> intermediateSpan = fBytesVSIntermediateCvt_.Bytes2Characters (from, intermediateBuf); // shortens 'from' if needed

                    // then use fIntermediateVSFinalCHARCvt_ to perform final mapping INTERMEDIATE_CHAR_T -> CHAR_T
                    ConversionResultWithStatus cr = fIntermediateVSFinalCHARCvt_.ConvertQuietly (intermediateSpan, to);
                    switch (cr.fStatus) {
                        case ConversionStatusFlag::sourceIllegal:
                            UTFConvert::Throw (cr.fStatus, cr.fSourceConsumed);
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
            Require (to.size () >= ComputeTargetByteBufferSize (from) or to.size () >= this->_Characters2Bytes (from));
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
                        return fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T, CHAR_T> (*i);
                    }
                    else {
                        return fIntermediateVSFinalCHARCvt_.template ComputeTargetBufferSize<INTERMEDIATE_CHAR_T> (get<span<const CHAR_T>> (src));
                    }
                }
            }();
            return fBytesVSIntermediateCvt_.ComputeTargetByteBufferSize (intermediateCharCntMax);
        }
        CodeCvt<INTERMEDIATE_CHAR_T> fBytesVSIntermediateCvt_;
        conditional_t<sizeof (CHAR_T) != sizeof (INTERMEDIATE_CHAR_T), UTFConvert, byte> fIntermediateVSFinalCHARCvt_; // would like to remove field if sizeof ==, but not sure how (void doesnt work)
    };

    /*
     *  This is crazy complicated because codecvt goes out of its way to be hard to copy, hard to move, but with
     *  a little care, can be made to work with unique_ptr.
     * 
     *  Also, std::codecvt doesn't natively support fInvalidCharacterReplacement, so we have to support manually.
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <typename STD_CODE_CVT_T>
    struct CodeCvt<CHAR_T>::CodeCvt_WrapStdCodeCvt_ : CodeCvt<CHAR_T>::IRep {
        unique_ptr<STD_CODE_CVT_T> fCodeCvt_;
        optional<Character>        fInvalidCharacterReplacement_;
        optional<span<byte>>       fInvalidCharacterReplacementBytes_;
        using extern_type = typename STD_CODE_CVT_T::extern_type;
        extern_type fInvalidCharacterReplacementBytesBuf[8]; // WAG at sufficient size, but sb enuf
        static_assert (is_same_v<CHAR_T, typename STD_CODE_CVT_T::intern_type>);
#if qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
        Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
        CodeCvt_WrapStdCodeCvt_ (const Options& options, unique_ptr<STD_CODE_CVT_T>&& codeCvt)
            : fCodeCvt_{move (codeCvt)}
            , fInvalidCharacterReplacement_{options.fInvalidCharacterReplacement}
        {
            if (fInvalidCharacterReplacement_) {
                mbstate_t                   ignoredMBState{};
                Memory::StackBuffer<CHAR_T> tmpBuf;
                span<const CHAR_T>          invalCharPartlyEncode = fInvalidCharacterReplacement_->As<CHAR_T> (&tmpBuf);
                const CHAR_T*               ignoreCharsConsumed   = nullptr;
                extern_type*                bytesInvalChar        = fInvalidCharacterReplacementBytesBuf;
                auto                        r =
                    fCodeCvt_->out (ignoredMBState, invalCharPartlyEncode.data (), invalCharPartlyEncode.data () + invalCharPartlyEncode.size (),
                                    ignoreCharsConsumed, fInvalidCharacterReplacementBytesBuf,
                                    fInvalidCharacterReplacementBytesBuf + Memory::NEltsOf (fInvalidCharacterReplacementBytesBuf), bytesInvalChar);
                if (r == STD_CODE_CVT_T::ok) {
                    fInvalidCharacterReplacementBytes_ = as_writable_bytes (
                        span{fInvalidCharacterReplacementBytesBuf}.subspan (0, bytesInvalChar - fInvalidCharacterReplacementBytesBuf));
                }
                else {
                    Private_::ThrowInvalidCharacterProvidedDoesntFitWithProvidedCodeCvt_ ();
                }
            }
        }
        virtual Options GetOptions () const override
        {
            return Options{.fInvalidCharacterReplacement = fInvalidCharacterReplacement_};
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
            size_t             bytesDone = 0;
            size_t             charsDone = 0;
        continueWith:
            auto r = fCodeCvt_->in (ignoredMBState, _First1 + bytesDone, _Last1, _Mid1, _First2 + charsDone, _Last2, _Mid2);
            if (r == STD_CODE_CVT_T::partial) {
                *from = from->subspan (charsDone + static_cast<size_t> (_Mid2 - _First2)); // reference remaining bytes, could be partial character at end of multibyte sequence
                Assert (from->size () != 0);
            }
            else if (r != STD_CODE_CVT_T::ok) {
                if (fInvalidCharacterReplacement_) {
                    bytesDone = _Mid1 - _First1 + 1; // skip one byte and try again (no idea how many bytes would have been best to skip)
                    charsDone = _Mid2 - _First2;

                    Memory::StackBuffer<CHAR_T> badCharTmpBuf;
                    span<const CHAR_T>          badCharReplaceSpan = fInvalidCharacterReplacement_->As<CHAR_T> (&badCharTmpBuf);
                    span<CHAR_T>                copied = Memory::CopySpanData (badCharReplaceSpan, span{&_First2[charsDone], _Last2});
                    Assert (copied.size () >= 0);
                    charsDone += copied.size ();
                    Assert (charsDone <= to.size ());
                    goto continueWith;
                }
                else {
                    Private_::ThrowErrorConvertingBytes2Characters_ (_Mid1 - _First1);
                }
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
            size_t        charsDone = 0;
            size_t        bytesDone = 0;
        continueWith:
            auto r = fCodeCvt_->out (ignoredMBState, _First1 + charsDone, _Last1, _Mid1, _First2 + bytesDone, _Last2, _Mid2);
            if (r != STD_CODE_CVT_T::ok) {
                if (fInvalidCharacterReplacement_) {
                    charsDone = _Mid1 - _First1 + 1; // skip one character and try again
                    bytesDone = _Mid2 - _First2;
                    memcpy (_First2 + bytesDone, fInvalidCharacterReplacementBytes_->data (), fInvalidCharacterReplacementBytes_->size ());
                    bytesDone += fInvalidCharacterReplacementBytes_->size ();
                    goto continueWith;
                }
                else {
                    Private_::ThrowErrorConvertingCharacters2Bytes_ (_Mid1 - _First1);
                }
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
            BuiltinSingleByteTableCodePageRep_ (CodePage cp, optional<Character> invalidCharacterReplacement);
            virtual ~BuiltinSingleByteTableCodePageRep_ () = default;
            virtual CodeCvt<char16_t>::Options GetOptions () const override
            {
                optional<char16_t> invalRepChar;
                if (fInvalidCharacterReplacementByte_ != nullopt) {
                    char16_t x;
                    auto     byteSpan = span{&*fInvalidCharacterReplacementByte_, 1};
                    (void)this->Bytes2Characters (&byteSpan, span{&x, 1});
                    invalRepChar = x;
                }
                return CodeCvt<char16_t>::Options{.fInvalidCharacterReplacement = invalRepChar};
            }
            virtual span<char16_t> Bytes2Characters (span<const byte>* from, span<char16_t> to) const override;
            virtual span<byte>     Characters2Bytes (span<const char16_t> from, span<byte> to) const override;
            virtual size_t         ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override;
            virtual size_t         ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const override;
            const char16_t*        fMap_;
            optional<byte>         fInvalidCharacterReplacementByte_;
        };
#if qPlatform_Windows
        struct WindowsNative_ final : CodeCvt<char16_t>::IRep {
            constexpr WindowsNative_ (CodePage cp)
                : fCodePage_{cp}
            {
            }
            virtual ~WindowsNative_ () = default;
            virtual CodeCvt<char16_t>::Options GetOptions () const override
            {
                return {};
            }
            virtual span<char16_t> Bytes2Characters (span<const byte>* from, span<char16_t> to) const override;
            virtual span<byte>     Characters2Bytes (span<const char16_t> from, span<byte> to) const override;
            virtual size_t         ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const override;
            virtual size_t         ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const override;
            CodePage               fCodePage_;
        };
#endif
    }

#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    /*
     ********************************************************************************
     ************************* CodeCvt<CHAR_T>::Options *****************************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <IUNICODECanAlwaysConvertTo FROM_CHAR_T_OPTIONS>
    constexpr inline auto CodeCvt<CHAR_T>::Options::New (typename CodeCvt<FROM_CHAR_T_OPTIONS>::Options o) -> Options
    {
        return Options{.fInvalidCharacterReplacement = o.fInvalidCharacterReplacement};
    }
#endif

    /*
     ********************************************************************************
     ******************************* CodeCvt<CHAR_T> ********************************
     ********************************************************************************
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const Options& options)
        : fRep_{make_shared<UTFConvertRep_<char8_t>> (options)} // default, is to serialize to UTF-8
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline CodeCvt<CHAR_T>::CodeCvt (const locale& l, const Options& options)
    {
        if constexpr (is_same_v<CHAR_T, wchar_t>) {
            *this = mkFromStdCodeCvt<codecvt_byname<wchar_t, char, mbstate_t>> (options, l.name ());
        }
        else if constexpr (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>) {
            *this = mkFromStdCodeCvt<codecvt_byname<CHAR_T, char8_t, mbstate_t>> (options, l.name ());
        }
        else if constexpr (is_same_v<CHAR_T, Character>) {
            fRep_ = make_shared<UTF2UTFRep_<char32_t>> (CodeCvt<char32_t>::mkFromStdCodeCvt<codecvt_byname<char32_t, char8_t, mbstate_t>> (
                CodeCvt<char32_t>::Options::New<CHAR_T> (options), l.name ()));
        }
        else {
            // CHAR_T COULD be UTF-8, but not clear if/why that would be useful.
            AssertNotImplemented ();
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (const Charset& charset, const Options& options)
    {
        if (charset == WellKnownCharsets::kISO_8859_1) {
            fRep_ = make_shared<Latin1ConvertRep_> (options);
        }
        else if (charset == WellKnownCharsets::kUTF8) {
            *this = CodeCvt<CHAR_T>{UnicodeExternalEncodings::eUTF8};
        }
        else if (is_same_v<CHAR_T, Character>) {
            // best hope is to treat it as a locale name, and hope its found
            fRep_ = make_shared<UTF2UTFRep_<char32_t>> (CodeCvt<char32_t>::mkFromStdCodeCvt<codecvt_byname<char32_t, char8_t, mbstate_t>> (
                CodeCvt<char32_t>::Options::New<CHAR_T> (options), charset.AsNarrowSDKString ()));
        }
        else {
            Private_::ThrowCharsetNotSupportedException_ (charset);
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (UnicodeExternalEncodings e, const Options& options)
        : fRep_{}
    {
        switch (e) {
            case UnicodeExternalEncodings::eUTF8:
                fRep_ = make_shared<UTFConvertRep_<char8_t>> (options);
                break;
            case UnicodeExternalEncodings::eUTF16_BE:
            case UnicodeExternalEncodings::eUTF16_LE:
                if (e == UnicodeExternalEncodings::eUTF16) {
                    fRep_ = make_shared<UTFConvertRep_<char16_t>> (options);
                }
                else {
                    fRep_ = make_shared<UTFConvertSwappedRep_<char16_t>> (options);
                }
                break;
            case UnicodeExternalEncodings::eUTF32_BE:
            case UnicodeExternalEncodings::eUTF32_LE:
                if (e == UnicodeExternalEncodings::eUTF32) {
                    fRep_ = make_shared<UTFConvertRep_<char32_t>> (options);
                }
                else {
                    fRep_ = make_shared<UTFConvertSwappedRep_<char32_t>> (options);
                }
                break;
            default:
                AssertNotImplemented ();
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (span<const byte>* guessFormatFrom, const optional<CodeCvt>& useElse, const Options& options)
        : fRep_{}
    {
        RequireNotNull (guessFormatFrom);
        Require (useElse == nullopt or useElse->GetOptions ().fInvalidCharacterReplacement == options.fInvalidCharacterReplacement);
        if (optional<tuple<UnicodeExternalEncodings, size_t>> r = ReadByteOrderMark (*guessFormatFrom)) {
            *guessFormatFrom = guessFormatFrom->subspan (get<size_t> (*r));
            fRep_            = CodeCvt{get<UnicodeExternalEncodings> (*r), options}.fRep_;
        }
        else {
            fRep_ = useElse ? useElse->fRep_ : CodeCvt{options}.fRep_;
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    CodeCvt<CHAR_T>::CodeCvt (CodePage cp, const Options& options)
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
                fRep_ = make_shared<UTF2UTFRep_<char16_t>> (
                    CodeCvt<char16_t> (make_shared<Private_::BuiltinSingleByteTableCodePageRep_> (cp, options.fInvalidCharacterReplacement)));
                break;
            case WellKnownCodePages::kUTF8:
                fRep_ = make_shared<UTFConvertRep_<char8_t>> (options);
                break;
            case WellKnownCodePages::kUNICODE_WIDE:
                fRep_ = make_shared<UTFConvertRep_<char16_t>> (options);
                break;
            case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN:
                fRep_ = make_shared<UTFConvertSwappedRep_<char16_t>> (options);
                break;
            default:
#if qPlatform_Windows
                if (options.fInvalidCharacterReplacement) {
                    Private_::ThrowCodePageNotSupportedException_ (cp); // WindowsNative_ doesn't support fInvalidCharacterReplacement
                }
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
        : fRep_{(RequireExpression (rep != nullptr), rep)}
    {
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <IStdCodeCVT STD_CODECVT, typename... ARGS>
    inline CodeCvt<CHAR_T> CodeCvt<CHAR_T>::mkFromStdCodeCvt (const Options& options, ARGS... args)
        requires (is_same_v<CHAR_T, typename STD_CODECVT::intern_type>)
    {
        auto u = make_unique<Private_::deletable_facet_<STD_CODECVT>> (forward<ARGS> (args)...);
        return CodeCvt<CHAR_T>{make_shared<CodeCvt_WrapStdCodeCvt_<Private_::deletable_facet_<STD_CODECVT>>> (options, move (u))};
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::GetOptions () const -> Options
    {
        return fRep_->GetOptions ();
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte> from) const -> size_t
    {
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
    inline auto CodeCvt<CHAR_T>::Bytes2Characters (span<const byte> from, span<CHAR_T> to) const -> span<CHAR_T>
    {
        AssertNotNull (fRep_);
        Require (to.size () >= ComputeTargetCharacterBufferSize (from) or to.size () >= Bytes2Characters (from)); // ComputeTargetCharacterBufferSize cheaper to compute
        size_t origSize = from.size ();
        auto   result   = fRep_->Bytes2Characters (&from, to);
        if (not from.empty ()) {
            Private_::ThrowErrorConvertingBytes2Characters_ (origSize - from.size ());
        }
        return result;
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    inline auto CodeCvt<CHAR_T>::Characters2Bytes (span<const CHAR_T> from) const -> size_t
    {
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
    template <constructible_from<const CHAR_T*, const CHAR_T*> STRINGISH>
    STRINGISH CodeCvt<CHAR_T>::Bytes2String (span<const byte> from) const
    {
        size_t                      origSize = from.size ();
        Memory::StackBuffer<CHAR_T> buf{this->ComputeTargetCharacterBufferSize (from)};
        span<CHAR_T>                r = this->Bytes2Characters (&from, span{buf});
        if (not from.empty ()) {
            Private_::ThrowErrorConvertingBytes2Characters_ (origSize - from.size ());
        }
        return STRINGISH{r.data (), r.data () + r.size ()};
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
    template <constructible_from<const byte*, const byte*> BLOBISH>
    BLOBISH CodeCvt<CHAR_T>::String2Bytes (span<const CHAR_T> from) const
    {
        Memory::StackBuffer<byte> buf{Memory::eUninitialized, this->ComputeTargetByteBufferSize (from)};
        const span<const byte>    r = this->Characters2Bytes (from, span{buf});
        if constexpr (same_as<BLOBISH, string>) {
            return string{reinterpret_cast<const char*> (r.data ()), reinterpret_cast<const char*> (r.data ()) + r.size ()};
        }
        else {
            return BLOBISH{r.data (), r.data () + r.size ()};
        }
    }

}

#endif /*_Stroika_Foundation_Characters_CodeCvt_inl_*/
