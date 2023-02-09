/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodeCvt_h_
#define _Stroika_Foundation_Characters_CodeCvt_h_ 1

#include "../StroikaPreComp.h"

#include <locale>
#include <optional>
#include <span>

#include "Character.h"

/**
 *  \file
 *      Simple wrapper on std::codecvt, with a few enhancements
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    /*
     *  \brief CodeCvt unifies byte<-> unicode conversions, vaguely inspired by (and wraps) std::codecvt, as well as UTFConverter etc, to map between 'bytes' and a UNICODE code-point span
     * 
     *  Enhancements over std::codecvt:
     *      o   You can subclass (provide your own codecvt implementation) and copy 'codecvt' objects.
     *          (unless I'm missing something, you can do one or the other with std::codecvt, but not both)
     *      o   Simpler backend virtual API, so easier to create your own compliant CodeCvt object.
     *          o   Stroika leverages these two things in UTFConverter, using differnt library backends to do
     *              the code conversion, hopefully enuf faster to make up for the virtual call overhead this
     *              class introduces.
     *      o   Dont bother templating on MBSTATE, nor output byte type (std::covert supports all the useless
     *          ones but misses the most useful, at least for fileIO, binary IO)
     *      o   lots of templated combinations dont make sense and dont work and there is no hint/validation
     *          clarity about which you can use/make sense and which you cannot with std::codecvt. Hopefully
     *          this class will make more sense.
     *          It can be used to convert (abstract API) between ANY combination of 'target hidden in implementation'
     *          and exposed CHAR_T characters (reading or writing). DEFAULT CTORS only provide the combinations
     *          supported by stdc++ (and a little more). To get other combinations, you must use subclass.
     * 
     *  And: 
     *      o   All the existing codecvt objects can easily be wrapped in a CodeCvt
     *
     *  Mostly, you can think of CodeCvt as an 'abstract class' in that only for some CHAR_T types
     *  can it be instantiated direcly (the ones std c++ supports, char_16_t, char32_t, and wchar_t with locale).
     * 
     *  But it also can be thought of as a smart pointer class, to underlying 'reps' - which can make generic
     *  the functionality in UTFConverter, and std::codecvt.
     */
    template <Character_IsUnicodeCodePoint CHAR_T>
    class CodeCvt {
    public:
        using MBState = std::mbstate_t; // may need to enhance this MBState at this level of API to handle wchar_t locale stuff

        using result                    = codecvt_base::result; // codecvt results - sadly seem to be int, not enum - but 4
        static constexpr result ok      = codecvt_base::ok;
        static constexpr result partial = codecvt_base::partial;
        static constexpr result error   = codecvt_base::error;
        static constexpr result noconv  = codecvt_base::noconv;

    public:
        struct IRep;

    public:
        /**
         *  These default APIs are provided by std c++:
         *      CodeCvt<char16_t>{}         -   std::codecvt<char16_t, char8_t, std::mbstate_t>
         *      CodeCvt<char32_t>{}         -   std::codecvt<char32_t, char8_t, std::mbstate_t>
         *      CodeCvt<wchar_t>{locale}    -   std::codecvt<wchar_t, char, std::mbstate_t>
         * 
         *  To get OTHER conversions, say between char16_t, and char32_t, you must use UTFConvert (NYI adapter for CodeCvt).
         */
        CodeCvt ()
            requires (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>);
        CodeCvt (const locale& = locale{})
            requires (is_same_v<CHAR_T, wchar_t>);
        CodeCvt (const shared_ptr<IRep>& rep);

    public:
        /**
         *  \brief like std::codecvt<>::in () - but with spans, and use ptr to be clear in/out
         * 
         *  convert bytes 'from' to characters 'to'. Spans on input, src and target buffers. spans on output 
         *  are amount remaining to be used 'from' and amount actually filled into 'to'.
         *  state is used to carry forward incomplete conversions from one call to the next.
         */
        nonvirtual result in (MBState* state, span<const byte>* from, span<CHAR_T>* to) const;

    public:
        /*
         *  \brief like std::codecvt<>::out () - but with spans, and use ptr to be clear in/out
         * 
         *  convert characters 'from' to bytes 'to'. Spans on input, src and target buffers. spans on output 
         *  are amount remaining to be used 'from' and amount actually filled into 'to'.
         *  state is used to carry forward incomplete conversions from one call to the next.
        */
        nonvirtual result out (MBState* state, span<const CHAR_T>* from, span<byte>* to) const;

    private:
        shared_ptr<IRep> fRep_;
    };

    template <Character_IsUnicodeCodePoint CHAR_T>
    struct CodeCvt<CHAR_T>::IRep {
        virtual ~IRep ()                                                                    = default;
        virtual result in (MBState* state, span<const byte>* from, span<CHAR_T>* to) const  = 0;
        virtual result out (MBState* state, span<const CHAR_T>* from, span<byte>* to) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodeCvt.inl"

#endif /*_Stroika_Foundation_Characters_CodeCvt_h_*/
