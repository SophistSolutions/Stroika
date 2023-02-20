/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

/**
 *  \file
 *      Simple wrapper on std::codecvt, abstracting commonalities between this and UTFConverter, to map characters <--> bytes
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    /**
     *  \brief list of external UNICODE character encodings, for file IO (eDefault = eUTF8)
     */
    enum class UnicodeExternalEncodings {
        eUTF7,
        eUTF8,
        eUTF16_BE,
        eUTF16_LE,
        eUTF16 = std::endian::native == std::endian::big ? eUTF16_BE : eUTF16_LE,
        eUTF32_BE,
        eUTF32_LE,
        eUTF32 = std::endian::native == std::endian::big ? eUTF32_BE : eUTF32_LE,

        eDefault = eUTF8,
    };

    namespace Private_ {
        template <class>
        inline constexpr bool IsStdCodeCvt_ = false;
        template <class _Elem, class _Byte, class _Statype>
        inline constexpr bool IsStdCodeCvt_<std::codecvt<_Elem, _Byte, _Statype>> = true;
    }
    /**
     *   @todo confused thy this is working with codecvt_by_name - think I need to change for that.
     */
    template <typename STD_CODECVT_T>
    concept IsStdCodeCVTT = Private_::IsStdCodeCvt_<STD_CODECVT_T>;

    /*
     *  \brief CodeCvt unifies byte<-> unicode conversions, vaguely inspired by (and wraps) std::codecvt, as well as UTFConverter etc, to map between span<bytes> and a span<UNICODE code-point>
     * 
     *  Enhancements over std::codecvt:
     *      o   You can subclass IRep (to provide your own CodeCvt implementation) and copy CodeCvt objects.
     *          (unless I'm missing something, you can do one or the other with std::codecvt, but not both)
     *      o   Simpler backend virtual API, so easier to create your own compliant CodeCvt object.
     *          o   CodeCvt leverages these two things via UTFConverter (which uses different library backends to do
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
     *          within this API).
     *      o   no 'noconv' error code.
     * 
     *  Enhancements over UTFConverter:
     *      o   UTFConverter only supports UNICODE <-> UNICODE translations, even if in different
     *          UNICODE encodings. This API supports UNICODE <-> any arbitrary output binary format.
     *      o   So in particular, it supports translating between UNICODE characters and locale encodings (e.g. SHIFT_JIS, or whatever).
     * 
     *  And: 
     *      o   All the existing codecvt objects (which map to/from UNICODE) can easily be wrapped in a CodeCvt
     *
     *  CodeCvt as smart Ptr class, and an 'abstract class' (IRep) in that only for some CHAR_T types
     *  can it be instantiated direcly (the ones std c++ supports, char_16_t, char32_t, and wchar_t with locale).
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    class CodeCvt {
    public:
        using intern_type = CHAR_T; // what codecvt calls the (internal/CHAR_T) character type

    public:
        struct IRep;

    public:
        /**
         *  Default CTOR:
         *      Produces the fastest available CodeCvt(), between the templated UNICODE code-point
         *      and UTF-8 (as the binary format).
         * 
         *   To use (wrap) existing std::codecvt<A,B,C> class:
         *      Quirky, because classes not generally directly instantiatable, so instead specify CLASS as template param
         *      and ARGS to CTOR.
         *          CodeCvt<CHAR_T,std::codecvt<CHAR_T, BINARY_T, MBSTATE_T>> {args to that class}
         *          Note works with subclasses of std::codecvt like std::codecvt_byname
         * 
         *  To get OTHER conversions, say between char16_t, and char32_t (combines/chains CodeCvt's):
         *      CodeCvt<CHAR_T>{UnicodeExternalEncodings}               -   Uses UTFConverter, along with any needed byte swapping
         *      CodeCvt<CHAR_T>{const CodeCvt<OTHER_CHAR_T> basedOn}    -   Use this to combine CodeCvt's (helpful for locale one)
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
         *      \endcode
         */
        CodeCvt ();
        CodeCvt (const locale& l);
        CodeCvt (UnicodeExternalEncodings e);
        template <Character_UNICODECanAlwaysConvertTo INTERMEDIATE_CHAR_T>
        CodeCvt (const CodeCvt<INTERMEDIATE_CHAR_T>& basedOn);
        CodeCvt (const shared_ptr<IRep>& rep);

    public:
        /**
         *  Note, though logically this should be a CodeCvt, since you cannot directly construct
         *  the STD_CODECVT, it cannot be passed by argument to the constructor. And so their
         *  appears now way to deduce or specify those constructor template arguments. But that can be done
         *  explicitly with a static function, and that is what we do with mkFromStdCodeCvt.
         */
        template <IsStdCodeCVTT STD_CODECVT, typename... ARGS>
        static CodeCvt mkFromStdCodeCvt (ARGS... args);

    public:
        /**
         *  \brief convert span byte (external serialized format) parameters to characters (like std::codecvt<>::in () - but with spans, and simpler api)
         * 
         *  Convert bytes 'from' to characters 'to'. 
         *
         *  Arguments:
         *      o   span<byte> from - initially all of which will be converted or an exeception thrown (only if data corrupt/unconvertable) (updated to point to bytes which form part of a single additional character)
         *      o   span<CHAR_T> to - buffer to have data converted 'into', which MUST be large enuf (call ComputeTargetCharacterBufferSize)
         *
         *  Returns:
         *      subspan of 'to', with converted characters.
         *      Throws on failure.
         *      And '*from' updated to reflect any remaining bytes that are part of the next character.
         * 
         *  Source bytes must begin on a valid character boundary (unlike codecvt - no mbstate).
         *  If the input buffer ends with any incomplete characters, *from will refer to those characters
         *  on function completion.
         * 
         *  The caller typically will wish to save those, and resubmit their BytesToCharacter call
         *  with a new buffer, starting with those (but there is no requirement to do so).
         * 
         *  No state is maintained. ALL the input is converted expect possibly a few bytes at the end of the input
         *  which consitute a partial character.
         *
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in every case I'm aware of
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to std::byte, and the codecvt API calls this extern_type
         * 
         *  \req to.size () >= ComputeTargetCharacterBufferSize (*from) on input.
         */
        nonvirtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const;

    public:
        /*
         *  \brief convert span<character> parameter to a span of bytes (like std::codecvt<>::out () - but with spans, and otherwise simpler API)
         * 
         *  Convert characters 'from' to bytes 'to'. 
         * 
         *  Arguments:
         *      o   span<character> from - all of which will be converted or an exeception thrown (only if data corrupt/unconvertable).
         *      o   span<byte> to - buffer to have data converted 'into', which MUST be large enuf (call ComputeTargetByteBufferSize)
         *
         *  Returns:
         *      (sub)subspan of 'to', with characters converted to appropriate span of bytes.
         *      Throws on failure.
         * 
         *  No state is maintained. ALL the input is converted to all the output, on character
         *  boundaries.
         * 
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in most cases
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to std::byte, and the codecvt API calls this extern_type
         * 
         *  \req to.size () >= ComputeTargetByteBufferSize (from) on input.
         */
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

    private:
        shared_ptr<IRep> fRep_;

    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertRep_;

    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertSwappedRep_;

    private:
        template <typename INTERMEDIATE_CHAR_T>
        struct UTF2UTFRep_;

    private:
        // requires CHAR_T = typename STD_CODE_CVT_T::intern_type
        template <typename STD_CODE_CVT_T>
        struct CodeCvt_WrapStdCodeCvt_;
    };

    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    struct CodeCvt<CHAR_T>::IRep {
        virtual ~IRep ()                                                                                    = default;
        virtual span<CHAR_T> Bytes2Characters (span<const byte>* from, span<CHAR_T> to) const               = 0;
        virtual span<byte>   Characters2Bytes (span<const CHAR_T> from, span<byte> to) const                = 0;
        virtual size_t       ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const = 0;
        virtual size_t       ComputeTargetByteBufferSize (variant<span<const CHAR_T>, size_t> src) const    = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodeCvt.inl"

#endif /*_Stroika_Foundation_Characters_CodeCvt_h_*/
