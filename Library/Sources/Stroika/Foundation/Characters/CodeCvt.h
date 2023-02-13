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
     *      o   You can subclass Rep (provide your own CodeCvt implementation) and copy CodeCvt objects.
     *          (unless I'm missing something, you can do one or the other with std::codecvt, but not both)
     *      o   Simpler backend virtual API, so easier to create your own compliant CodeCvt object.
     *          o   CodeCvt leverages these two things via, UTFConverter (which uses different library backends to do
     *              the UTF code conversion, hopefully enuf faster to make up for the virtual call overhead this
     *              class introduces).
     *      o   Dont bother templating on MBSTATE, nor output byte type (std::covert supports all the useless
     *          ones but misses the most useful, at least for fileIO, binary IO)
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
     *  Enhancements over UTFConverter:
     *      o   Supports translating from/to locale encodings (e.g. SHIFT_JIS, or whatever).
     *      o   (UTFConverter only supports UNICODE <-> UNICODE translations, even if in different
     *          UNICODE encodings).
     * 
     *  And: 
     *      o   All the existing codecvt objects (which map to/from UNICODE) can easily be wrapped in a CodeCvt
     *
     *  CodeCvt as smart Ptr class, and an 'abstract class' (IRep) in that only for some CHAR_T types
     *  can it be instantiated direcly (the ones std c++ supports, char_16_t, char32_t, and wchar_t with locale).
     * 
     *  \note About Target Buffer Sizes:
     *        Unlike UTFConverter, CodeCvt allows an undersized target buff (@todo RECONSIDER IF GOOD IDEA).
     *        That causes TONS of problem.
     *        MUCH more complex code.
     *        But upshot - dont do this - always give enuf target buffer, to at least avoid performance penalties
     */
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    class CodeCvt {
    public:
        using MBState = std::mbstate_t; // may need to enhance this MBState at this level of API to handle wchar_t locale stuff

        using intern_type = CHAR_T; // what codecvt calls the character type

    public:
        /*
         *  Notes about error results, and partial status/error code.
         * 
         *  Some codecvt implementations (namely the std c++ - so primarily locale - conversions may use the 
         *  std::mbstate_t to store data from partial conversions. Some implementations (such as UTFConverter)
         *  may not. This API is sadly general enuf to allow for either case (cuz not too hard to accomodate
         *  in use, but unfortunate).
         * 
         *  Just understand, regardless of mbstate support, not all the input or output characters will
         *  necessarily be processed. But each Bytes2Characters/Characters2Bytes call tells how
         *  many of each were processed. And just track the MBState as you progress through your
         *  input buffer, and you will be fine, regardless of the underlying implementation (whther it uses mbstate or not).
         */
        using result                    = codecvt_base::result;  // codecvt results - sadly seem to be int, not enum - but 4
        static constexpr result ok      = codecvt_base::ok;      // conversion succeeded
        static constexpr result partial = codecvt_base::partial; // not all source converted because target buffer too small
        static constexpr result error = codecvt_base::error; // some data maybe converted (see in/out params) - but typically doesn't matter - just throw
        static constexpr result noconv = codecvt_base::noconv; // probably cannot happen with this class (TBD)

    public:
        struct IRep;

    public:
        /**
         *  Default CTOR:
         *      Produces the fastest available CodeCvt(), between the templated UNICODE code-point
         *      and UTF-8 (as the binary format).
         * 
         *   To use (wrap) existing std::codecvt<A,B,C> class:
         *      Tricky, because classes not generally directly instantiatable, so instead specify CLASS as template param
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
         *          CodeCvt<char16_t> codeCvt2 = CodeCvt<char16_t,std::codecvt<char16_t, char8_t, std::mbstate_t>>{};
         * 
         *          // codeCvt Between UTF16 Characters using codecvt_byname
         *          CodeCvt<char16_t> codeCvt3 = CodeCvt<char16_t,std::codecvt_byname>>{locale{"en_US.UTF8"}};
         *      \endcode
         */
        CodeCvt ();
        CodeCvt (const locale& = locale{});
        CodeCvt (UnicodeExternalEncodings e);
        template <Character_UNICODECanAlwaysConvertTo OTHER_CHAR_T>
        CodeCvt (const CodeCvt<OTHER_CHAR_T>& basedOn);
        template <IsStdCodeCVTT STD_CODECVT, typename... ARGS>
        CodeCvt (ARGS... args);
        CodeCvt (const shared_ptr<IRep>& rep);

    public:
        /**
         *  \brief convert span byte parameters to characters (like std::codecvt<>::in () - but with spans, and use ptr to be clear in/out)
         * 
         *  convert bytes 'from' to characters 'to'. Spans on input, src and target buffers. spans on output 
         *  are amount remaining to be used 'from' and amount actually filled into 'to'.
         *  state is used to carry forward incomplete conversions from one call to the next.
         * 
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in every case I'm aware of
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to std::byte, and the codecvt API calls this extern_type
         * 
         *  \see the docs on 'error results, and partial status/error code' above
         */
        nonvirtual result Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const;

    public:
        /*
         *  \brief convert span characerter parameters to a span of bytes (like std::codecvt<>::out () - but with spans, and use ptr to be clear in/out)
         * 
         *  convert characters 'from' to bytes 'to'. Spans on input, src and target buffers. spans on output 
         *  are amount remaining to be used 'from' and amount actually filled into 'to'.
         *  state is used to carry forward incomplete conversions from one call to the next.
         * 
         *  \note we use the name 'Bytes' - because its suggestive of meaning, and in every case I'm aware of
         *        the target type will be char, or char8_t, or byte. But its certainly not guaranteed to be serialized
         *        to std::byte, and the codecvt API calls this extern_type
         * 
         *  \see the docs on 'error results, and partial status/error code' above
         */
        nonvirtual result Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const;

    private:
        shared_ptr<IRep> fRep_;

    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertRep_;

    private:
        template <typename SERIALIZED_CHAR_T>
        struct UTFConvertSwappedRep_;

    private:
        template <typename OTHER_CHAR_T>
        struct UTF2UTFRep_;

    private:
        // requires CHAR_T = typename STD_CODE_CVT_T::intern_type
        template <typename STD_CODE_CVT_T>
        struct CodeCvt_WrapStdCodeCvt_;
    };

    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    struct CodeCvt<CHAR_T>::IRep {
        virtual ~IRep ()                                                                                 = default;
        virtual result Bytes2Characters (span<const byte>* from, span<CHAR_T>* to, MBState* state) const = 0;
        virtual result Characters2Bytes (span<const CHAR_T>* from, span<byte>* to, MBState* state) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodeCvt.inl"

#endif /*_Stroika_Foundation_Characters_CodeCvt_h_*/
