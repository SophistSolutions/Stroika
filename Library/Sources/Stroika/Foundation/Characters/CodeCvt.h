/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodeCvt_h_
#define _Stroika_Foundation_Characters_CodeCvt_h_ 1

#include "../StroikaPreComp.h"

#include <bit>
#include <locale>
#include <optional>
#include <span>
#include <variant>

#include "Character.h"
#include "Charset.h"
#include "CodePage.h"
#include "UTFConvert.h"

/**
 *  \file
 *      Simple wrapper on std::codecvt, abstracting commonalities between std::codecvt and UTFConvert, to map characters (UNICODE) <--> bytes
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    class String; // forward declare to reference without deadly embrace

    namespace Private_ {
        template <typename>
        inline constexpr bool IsStdCodeCvt_ = false;
        template <class _Elem, class _Byte, class _Statype>
        inline constexpr bool IsStdCodeCvt_<const std::codecvt_byname<_Elem, _Byte, _Statype>*> = true; // @todo NOT CLEAR WHY THIS IS NEEDED???
        template <class _Elem, class _Byte, class _Statype>
        inline constexpr bool IsStdCodeCvt_<const std::codecvt<_Elem, _Byte, _Statype>*> = true;
    }

    /**
     *  Is std::codecvt or subclass of std::codecvt (such as codecvt_byname).
     */
    template <typename STD_CODECVT_T>
    concept IStdCodeCVT = Private_::IsStdCodeCvt_<const STD_CODECVT_T*>;
    static_assert (IStdCodeCVT<std::codecvt<char16_t, char8_t, std::mbstate_t>>);
    static_assert (IStdCodeCVT<std::codecvt<char32_t, char8_t, std::mbstate_t>>);
    static_assert (IStdCodeCVT<std::codecvt<wchar_t, char, std::mbstate_t>>);
    static_assert (IStdCodeCVT<std::codecvt_byname<wchar_t, char, std::mbstate_t>>);

    /*
     *  \brief CodeCvt unifies byte <-> unicode conversions, vaguely inspired by (and wraps) std::codecvt, as well as UTFConvert etc, to map between span<bytes> and a span<UNICODE code-point>
     * 
     *  Note, UTFConvert is probably a slightly better API, and better designed, and faster. HOWEVER, it ONLY converts to/from UNICODE. std::codecvt can convert to/from
     *  any locale code page, and is is more general.
     * 
     *  Use the CodeCvt<> API when your code conversions may involve non UNICODE byte representations.
     * 
     *  Note that this class - like codecvt - can be used to 'page' over an input, and incrementally convert it (though how it does this 
     *  differs from codecvt - not maintaining a partial state - but instead adjusting the amount consumed from the input to reflect
     *  full-character conversions).
     * 
     *  \note - if encountering invalid data in the input (invalid characters) - this will 'THROW' and not just fill in special bogus replacement
     *          characters.
     * 
     *  \note - the BINARY format character is OPAQUE given this API (you get/set bytes). Thie CHAR_T in the template argument
     *          refers to the 'CHARACTER' format you map to/from binary format (so typically wchar_t, or char32_t maybe).
     *
     *  Enhancements over std::codecvt:
     *      o   this is a span<> based API
     *      o   You can subclass IRep (to provide your own CodeCvt implementation) and copy CodeCvt objects.
     *          (unless I'm missing something, you can do one or the other with std::codecvt, but not both)
     *      o   Simpler backend virtual API, so easier to create your own compliant CodeCvt object.
     *          o   CodeCvt leverages these two things via UTFConvert (which uses different library backends to do
     *              the UTF code conversion, hopefully enuf faster to make up for the virtual call overhead this
     *              class introduces).
     *      o   Don't support 'partial' conversion.
     *          If there is insufficient space in the target buffer, this is an ASSERTION erorr - UNSUPPORTED.
     *          ALL 'srcSpan' CHARACTER data MUST be consumed/converted (for byte data; we allow
     *          only a single partial character at the end for Bytes2Characters takes ptr to span and updates span
     *          to reflect remaining bytes).
     *      o   Dont bother templating output byte type (std::covert supports all the useless
     *          ones but misses the most useful, at least for fileIO, binary IO)
     *      o   Don't support mbstate_t. Its opaque, and a PITA. And redundant.
     *      o   lots of templated combinations (codecvt) dont make sense and dont work and there is no hint/validation
     *          clarity about which you can use/make sense and which you cannot with std::codecvt. Hopefully
     *          this class will make more sense.
     *          It can be used to convert (abstract API) between ANY combination of 'target hidden in implementation'
     *          and exposed CHAR_T characters (reading or writing). DEFAULT CTORS only provide the combinations
     *          supported by stdc++ (and a little more). To get other combinations, you must use subclass.
     *      o   'equivilent' code-point types automatically supported (e.g wchar_t == char16 or char32, and
     *          Character==char32_t).
     *      o   No explicit 'external_type' exposed. Just bytes go in and out vs (CHAR_T) UNICODE characters.
     *          This erasure of the 'encoding' type from the CodeCvt<CHAR_T> allows it to be used generically
     *          where its hidden in the 'rep' what kind of encoding is used.
     * 
     *  Difference:
     *      o   Maybe enhancement, maybe step back:
     *          Must call ComputeTargetCharacterBufferSize/ComputeTargetByteBufferSize and provide
     *          an output buffer large enuf. This way, can NEVER get get partial conversion due to lack of output buffer space (which simplfies alot
     *          within this API). NOTE - large enuf doesn't necessarily mean as large as ComputeTargetCharacterBufferSize/ComputeTargetByteBufferSize would say, as those
     *          provide safe estimate. If you know for special reasons, you can use a smaller size, but the call must always FIT - no 'targetExhausted' exceptions thrown.
     *      o   no 'noconv' error code (better in that simpler, but worse in that forces throw on bad characters)
     * 
     *  Enhancements over UTFConvert:
     *      o   UTFConvert only supports UNICODE <-> UNICODE translations, even if in different
     *          UNICODE encodings. This API supports UNICODE <-> any arbitrary output binary format.
     *      o   So in particular, it supports translating between UNICODE characters and locale encodings (e.g. SHIFT_JIS, or whatever).
     * 
     *  And: 
     *      o   All the existing codecvt objects (which map to/from UNICODE) can easily be wrapped in a CodeCvt
     *
     *  CodeCvt as smart Ptr class, and an 'abstract class' (IRep) in that only for some CHAR_T types
     *  can it be instantiated direcly (the ones std c++ supports, char_16_t, char32_t, and wchar_t with locale).
     */
    template <IUNICODECanAlwaysConvertTo CHAR_T = Character>
    class CodeCvt {
    public:
        using intern_type = CHAR_T; // what codecvt calls the (internal/CHAR_T) character type

    public:
        struct IRep;

    public:
        /**
         */
        struct Options {
            /**
             *  NOTE - do as New not CTOR, so can still construct Options using designated initializers.
             */
            template <IUNICODECanAlwaysConvertTo FROM_CHAR_T_OPTIONS>
            static constexpr inline Options New (typename CodeCvt<FROM_CHAR_T_OPTIONS>::Options o)
            #if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
             {
        return Options{.fInvalidCharacterReplacement = o.fInvalidCharacterReplacement};
    }
            #else
            ;
            #endif

            /**
             *  if fInvalidCharacterReplacement is nullopt (the default) - conversion APIs throw on invalid characters, and
             *  otherwise use the value provided in fInvalidCharacterReplacement as the replacement for invalid characters.
             * 
             *  Note - CodeCvt allows the Character to be comprised of multiple CHAR_T characters potentially.
             */
            optional<Character> fInvalidCharacterReplacement;
        };

    public:
        /**
         *  Default CTOR:
         *      Produces the fastest available CodeCvt(), between the templated UNICODE code-point
         *      and UTF-8 (as the binary format).
         * 
         *  CodeCvt (const locale& l):
         *      Produces a CodeCvt which maps (back and forth) between bytes in the 'locale' character set, and
         *      UNICODE Characters.
         * 
         *  CodeCvt (const string& localeName):
         *      Is equivilent to mkFromStdCodeCvt<...> (std::codecvt_byname {localeName}) - so it can throw if no such locale name
         * 
         *  CodeCvt (span<const byte>* guessFormatFrom)
         *      the initial part of the span data (up to kMaxBOMSize bytes) are examined and used to select the CodeCvt to create (else default CodeCvt created).
         *      If a BOM is found, guessFormatFrom is adjusted to skip it.
         *  
         *  CodeCvt (CodePage):
         *      Can throw if the code page is not recognized. NOTE - CodePage is a Windows concept, and though many code pages
         *      are provided portable (@todo list) - many more are not, and will fail on non-windows, and succeed only on windows.
         *      The (Windows) code pages which are always (portably) provided include:
         *          WellKnownCodePages::kANSI
         *          WellKnownCodePages::kMAC
         *          WellKnownCodePages::PC,             //  IBM PC code page 437
         *          WellKnownCodePages::PCA,            //  IBM PC code page 850, used by IBM Personal System/2
         *          WellKnownCodePages::kGreek,
         *          WellKnownCodePages::kTurkish,
         *          WellKnownCodePages::kHebrew,
         *          WellKnownCodePages::kArabic,
         *          WellKnownCodePages::kUNICODE_WIDE, // Standard UNICODE for MS-Windows
         *          WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN,
         *          WellKnownCodePages::UTF8
         * 
         *   To use (wrap) existing std::codecvt<A,B,C> class:
         *      Quirky, because classes not generally directly instantiatable, so instead specify CLASS as template param
         *      and ARGS to CTOR.
         *          CodeCvt<CHAR_T,std::codecvt<CHAR_T, BINARY_T, MBSTATE_T>> {args to that class}
         *          Note works with subclasses of std::codecvt like std::codecvt_byname
         * 
         *  To get OTHER conversions, say between char16_t, and char32_t (combines/chains CodeCvt's):
         *      CodeCvt<CHAR_T>{UnicodeExternalEncodings}               -   Uses UTFConvert, along with any needed byte swapping
         *      CodeCvt<CHAR_T>{const CodeCvt<OTHER_CHAR_T> basedOn}    -   Use this to combine CodeCvt's (helpful for locale one)
         * 
         *  \par Example Usage:
         *      \code
         *          CodeCvt cc{"en_US.UTF8"};
         *          constexpr char8_t        someRandomText[] = u8"hello mom";
         *          span<const byte>         someRandomTextBinarySpan =  as_bytes (span<const char8_t> {someRandomText, Characters::CString::Length (someRandomText)});
         *          StackBuffer<Character> buf{cc.ComputeTargetCharacterBufferSize (someRandomTextBinarySpan)};
         *          auto b = cc.Bytes2Characters (&someRandomTextBinarySpan, span{buf});
         *          EXPECT_TRUE (someRandomTextBinarySpan.size () == 0);   // ALL CONSUMED
         *          EXPECT_TRUE (b.size () == 9 and b[0] == 'h');
         *      \endcode
         * 
         *  \par Example Usage:
         *      \code
         *          // codeCvt Between UTF16 Characters And UTF8BinaryFormat, best/fastest way
         *          CodeCvt<char16_t> codeCvt1{};           
         * 
         *          // codeCvt Between UTF16 Characters And UTF8BinaryFormat using std::codecvt<char16_t, char8_t, std::mbstate_t>
         *          CodeCvt<char16_t> codeCvt2 = CodeCvt<char16_t>::mkFromStdCodeCvt<std::codecvt<char16_t, char8_t, std::mbstate_t>> ();
         * 
         *          // codeCvt Between UTF16 Characters using codecvt_byname
         *          CodeCvt<char16_t> codeCvt3 = CodeCvt<char16_t,std::codecvt_byname>>{locale{"en_US.UTF8"}};
         * 
         *          // or equivilently
         *          CodeCvt<char16_t> codeCvt4{"en_US.UTF8"};
         *      \endcode
         * 
         * 
         *  @todo - ASSURE THESE 'locale-names' always work:
         *      (case insensitive equals)
         *          utf-8
         *          ISO-8859-1
         */
        CodeCvt (const Options& options = Options{});
        CodeCvt (const CodeCvt& src) = default;
        CodeCvt (CodeCvt&& src)      = default;
        CodeCvt (const locale& l, const Options& options = Options{});
        CodeCvt (const Charset& charset, const Options& options = Options{});
        CodeCvt (UnicodeExternalEncodings e, const Options& options = Options{});
        CodeCvt (span<const byte>* guessFormatFrom, const optional<CodeCvt>& useElse = nullopt, const Options& options = Options{});
        CodeCvt (CodePage e, const Options& options = Options{});
        template <IUNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
        CodeCvt (const CodeCvt<INTERMEDIATE_CHAR_T>& basedOn);
        CodeCvt (const shared_ptr<IRep>& rep);

    public:
        /**
         *  Note, though logically this should be a CodeCvt constructor, since you cannot directly construct
         *  the STD_CODECVT, it cannot be passed by argument to the constructor. And so their
         *  appears no way to deduce or specify those constructor template arguments. But that can be done
         *  explicitly with a static function, and that is what we do with mkFromStdCodeCvt.
         * 
         *  \note - everything else has options last argument, but since we use ... parameter pack, options must be first here.
         */
        template <IStdCodeCVT STD_CODECVT, typename... ARGS>
        static CodeCvt mkFromStdCodeCvt (const Options& options = {}, ARGS... args)
            requires (is_same_v<CHAR_T, typename STD_CODECVT::intern_type>);

    public:
        nonvirtual CodeCvt& operator= (const CodeCvt& rhs) = default;
        nonvirtual CodeCvt& operator= (CodeCvt&& rhs)      = default;

    public:
        /**
         */
        nonvirtual Options GetOptions () const;

    public:
        /**
         *  \brief convert span byte (external serialized format) parameters to characters (like std::codecvt<>::in () - but with spans, and simpler api)
         * 
         *  Convert bytes 'from' to characters 'to'. 
         *
         *  Arguments:
         *      o   span<byte> from - initially all of which will be converted or an exception thrown (only if data corrupt/unconvertable) (updated to point to bytes which form part of a single additional character)
         *      o   span<CHAR_T> to - buffer to have data converted 'into'
         *          NOTE - all we require is that the result fit into 'to'. BUt we offer a quick way to compute a buffer 'large enough' -  (call ComputeTargetCharacterBufferSize).
         *          But (a more expnsive) way is to call Bytes2Characters/1 and that will tell you exactly how many needed.
         *  Returns:
         *      subspan of 'to', with converted characters.
         *      Throws on failure (corrupt source content).
         *      And '*from' updated to reflect any remaining bytes that are part of the next character.
         * 
         *  Source bytes must begin on a valid character boundary (unlike codecvt - no mbstate).
         *  If the input buffer ends with any incomplete characters, *from will refer to those characters
         *  on function completion.
         * 
         *  The overload taking pointer to from returns the amount left. The overload taking span<> - not pointer - 
         *  throws if not all consumed.
         * 
         *  The caller typically will wish to save those, and resubmit their BytesToCharacter call
         *  with a new buffer, starting with those (but there is no requirement to do so).
         * 
         *  No state is maintained. ALL the input is converted expect possibly a few bytes at the end of the input
         *  which consitute a partial character.
         * 
         *  This implies that given a 'lead byte' as argument to Bytes2Characters, this function can return
         *  an EMPTY span, and that would not be an error (so no throw).
         *
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in every case I'm aware of
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to byte, and the codecvt API calls this extern_type
         * 
         *  /2 overload \req to.size () >= min(Bytes2Characters(*from), ComputeTargetCharacterBufferSize (*from)) on input.
         * 
         *  \see also Bytes2String for similar function, but operating on strings
         */
        nonvirtual size_t Bytes2Characters (span<const byte> from) const;
        nonvirtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const;
        nonvirtual span<CHAR_T> Bytes2Characters (span<const byte> from, span<CHAR_T> to) const;

    public:
        /*
         *  \brief convert span<character> parameter to a span of bytes (like std::codecvt<>::out () - but with spans, and otherwise simpler API)
         * 
         *  Convert characters 'from' to bytes 'to'. 
         * 
         *  Arguments:
         *      o   span<character> from - all of which will be converted or an exception thrown (only if data corrupt/unconvertable).
         *      o   OPTIONAL span<byte> to - buffer to have data converted 'into', which MUST be large enuf (call ComputeTargetByteBufferSize)
         *      o   span<span> to - buffer to have data converted 'into'
         *          NOTE - all we require is that the result fit into 'to'. BUt we offer a quick way to compute a buffer 'large enough' -  (call ComputeTargetByteBufferSize).
         *          But (a more expensive) way is to call Characters2Bytes/1 and that will tell you exactly how many needed.
         *
         *  Returns:
         *      (sub)subspan of 'to', (if provided)with characters converted to appropriate span of bytes.
         *      Else returns number of bytes it would have been converted to /1 overload).
         *      Throws on failure.
         * 
         *  No state is maintained. ALL the input is converted to all the output, on character
         *  boundaries.
         * 
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in most cases
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to byte, and the codecvt API calls this extern_type
         * 
         *  /2 overload \req to.size () >= min(ComputeTargetByteBufferSize (from),Characters2Bytes(from)) on input.
         * 
         *  \see also String2Bytes for similar function, but operating on strings
         */
        nonvirtual size_t Characters2Bytes (span<const CHAR_T> from) const;
        nonvirtual span<byte> Characters2Bytes (span<const CHAR_T> from, span<byte> to) const;

    public:
        /*
         *  \brief Compute the size of 'to' span to use in call to Bytes2Characters
         * 
         *  \note this may guess a size too large, but will always guess a size large enuf
         *  In the case of the size_t overload, its obviously a worst-case guess
         */
        nonvirtual size_t ComputeTargetCharacterBufferSize (span<const byte> src) const;
        nonvirtual size_t ComputeTargetCharacterBufferSize (size_t srcSize) const;

    public:
        /*
         *  \brief Compute the size of 'to' span to use in call to Characters2Bytes
         * 
         *  \note this may guess a size too large, but will always guess a size large enuf
         *  In the case of the size_t overload, its obviously a worst-case guess
         */
        nonvirtual size_t ComputeTargetByteBufferSize (span<const CHAR_T> src) const;
        nonvirtual size_t ComputeTargetByteBufferSize (size_t srcSize) const;

    public:
        /**
         *  Convert a span of bytes (in a coding defined by the constructor to CodeCvt) to a 'string' like object - anything constructible from a 'span' of characters (e.g. String or wstring)
         * 
         *  NOTE - when converting Bytes2String, the String must be encoded using CHAR_T characters.
         *  The binary rep - can be anything - of course.
         * 
         *  \par Example Usage
         *      \code
         *          span<const byte> bytes = from_somewhere;
         *          static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
         *          wstring result = kCvt_.Bytes2String<wstring> (bytes);
         *      \endcode
         * 
         *  \par Example Usage
         *      \code
         *          span<const byte> bytes = from_somewhere;
         *          wstring result = CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (bytes);
         *      \endcode
         */
        template <constructible_from<const CHAR_T*, const CHAR_T*> STRINGISH>
        nonvirtual STRINGISH Bytes2String (span<const byte> from) const;

    public:
        /**
         *  Convert a span of characters ('string') to a BLOB-like object - anything constructible from a 'span' of bytes; note that container of a span of bytes maybe 'string' (special case).
         * 
         *  NOTE - when converting String2Bytes, the String must be encoded using CHAR_T characters.
         *  The binary rep - can be anything - of course.
         * 
         *  \par Example Usage
         *      \code
         *          span<const wchar_t> s = from_somewhere;
         *          static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
         *          string utf8String = kCvt_.String2Bytes<string> (s);
         *      \endcode
         * 
         *  \par Example Usage
         *      \code
         *          span<const wchar_t> s = from_somewhere;
         *          Memory::BLOB localeFormatRenderingOfUnicodeInputAsLocaleFormatByteStream =  CodeCvt<wchar_t>{locale{}}.String2Bytes<Memory::BLOB> (s);
         *      \endcode
         * 
         *  \par Example Usage
         *      \code
         *          span<const wchar_t> s = from_somewhere;
         *          string localeFormatRenderingOfUnicodeInputAsLocaleFormatByteStream =  CodeCvt<wchar_t>{locale{}}.String2Bytes<string> (s);
         *      \endcode
         */
        template <constructible_from<const byte*, const byte*> BLOBISH>
        nonvirtual BLOBISH String2Bytes (span<const CHAR_T> from) const;

    private:
        shared_ptr<IRep> fRep_;

#if qCompilerAndStdLib_template_second_concept_Buggy
    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertRep_;

    private:
        struct Latin1ConvertRep_;

    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertSwappedRep_;

    private:
        template <typename INTERMEDIATE_CHAR_T>
        struct UTF2UTFRep_;
#else
    private:
        template <IUNICODECanAlwaysConvertTo SERIALIZED_CHAR_T>
        struct UTFConvertRep_;

    private:
        struct Latin1ConvertRep_;

    private:
        template <IUNICODECanAlwaysConvertTo SERIALIZED_CHAR_T>
        struct UTFConvertSwappedRep_;

    private:
        template <IUNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
        struct UTF2UTFRep_;
#endif

    private:
        // requires CHAR_T = typename STD_CODE_CVT_T::intern_type
        template <typename STD_CODE_CVT_T>
        struct CodeCvt_WrapStdCodeCvt_;
    };

    template <IUNICODECanAlwaysConvertTo CHAR_T>
    struct CodeCvt<CHAR_T>::IRep {
        virtual ~IRep ()                                                                                    = default;
        virtual Options      GetOptions () const                                                            = 0;
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const               = 0;
        virtual span<byte>   Characters2Bytes (span<const CHAR_T> from, span<byte> to) const                = 0;
        virtual size_t       ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const = 0;
        virtual size_t       ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const    = 0;

    protected:
        // handy utility for subclasses / requires calls
        nonvirtual size_t _Bytes2Characters (span<const byte> from) const;
        nonvirtual size_t _Characters2Bytes (span<const CHAR_T> from) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodeCvt.inl"

#endif /*_Stroika_Foundation_Characters_CodeCvt_h_*/
