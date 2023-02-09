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
     *  Enhancements over std::codecvt
     * 
     *      o   You can subclass (provide your own codecvt implementation) and copy 'codecvt' objects.
     *          (unless I'm missing something, you can do one or the other with std::codecvt, but not both)
     *      o   Simpler backend virtual API (probably using spans tbd), so easier to create your own compliant
     *          CodeCvt object.
     *          o   Stroika leverages these two things in UTFConverter, using differnt library backends to do
     *              the code conversion, hopefully enuf faster to make up for the virtual call overhead this
     *              class introduces.
     *      o   Dont bother templating on MBSTATE, nor output byte type (std::covert supports all the useless
     *          ones but misses the most useful, at least for fileIO, binary IO)
     * 
     *  And: 
     *      o   All the existing codecvt objects can easily be wrapped in a CodeCvt
     *
    // LIKE codecvt, but with only in/out methods (until I see need for rest)
    // PROBABLY REDO API using SPANS (I HATE THIS API UPDATING REFERENCES TO PTRS)


            requires (is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>)
OR TBD - maybe include wchar_t?
maybe include char8_t

     */
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    class CodeCvt {
    public:
        using MBState                   = std::mbstate_t;
        using result                    = codecvt_base::result; // codecvt results - sadly seem to be int, not enum - but 4
        static constexpr result ok      = codecvt_base::ok;
        static constexpr result partial = codecvt_base::partial;
        static constexpr result error   = codecvt_base::error;
        static constexpr result noconv  = codecvt_base::noconv;

    public:
        struct IRep;

    public:
        /**
        * 
        * todo doc better WHCIH of these are created - whcih std::codecvt objects each creates
         */
        // Default impl returns 'rep' which uses std::code_cvt<CHAR_T,char8_t,mbstate_t>
        // Locale CTOR wraps codecvt fetched from that locale into a CodeCvt object
        CodeCvt ();
        CodeCvt (const locale& = locale{});
        CodeCvt (const shared_ptr<IRep>& rep);

    public:
        /**
         * // convert bytes [_First1, _Last1) to [_First2, _Last2)
         */
        result in (MBState& _State, const byte* _First1, const byte* _Last1, const byte*& _Mid1, CHAR_T* _First2, CHAR_T* _Last2, CHAR_T*& _Mid2) const;

        /*
    // convert [_First1, _Last1) to bytes [_First2, _Last2)
    */
        result out (MBState& _State, const CHAR_T* _First1, const CHAR_T* _Last1, const CHAR_T*& _Mid1, byte* _First2, byte* _Last2, byte*& _Mid2) const;

    private:
        // same methods as codecvt roughly
        shared_ptr<IRep> fRep_;
    };

#if 1
    template <Character_IsBasicUnicodeCodePoint CHAR_T>
    struct CodeCvt<CHAR_T>::IRep {
        //   using result  = CodeCvt<char16_t>::result;
        //    using MBState = CodeCvt<char16_t>::MBState;
        virtual ~IRep () = default;

        // same methods as codecvt
        // / convert bytes [_First1, _Last1) to [_First2, _Last2)
        virtual result in (MBState& _State, const byte* _First1, const byte* _Last1, const byte*& _Mid1, CHAR_T* _First2, CHAR_T* _Last2,
                           CHAR_T*& _Mid2) const = 0;

        // same methods as codecvt
        // // convert [_First1, _Last1) to bytes [_First2, _Last2)
        virtual result out (MBState& _State, const CHAR_T* _First1, const CHAR_T* _Last1, const CHAR_T*& _Mid1, byte* _First2, byte* _Last2,
                            byte*& _Mid2) const = 0;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodeCvt.inl"

#endif /*_Stroika_Foundation_Characters_CodeCvt_h_*/
