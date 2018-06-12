/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "../StroikaPreComp.h"

#include <cstdarg>
#include <ios>
#include <locale>

#include "../Configuration/Common.h"
#include "../Memory/Optional.h"

#include "String.h"

/**
 * TODO:
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 *
 *      @todo   See if I can come up with a more flexibe and/or typesafe variant of Format using
 *              variadic templates? Maybe using a nicer syntax widely done in ruby/C# etc, like
 *              Format ("{1}{3}", "x", 3);??? VERY INCHOATE THOUGHTS HERE...
 */

namespace Stroika::Foundation {
    namespace Characters {

        /*
         * Format is the Stroika wrapper on sprintf().
         * The main differences between sprintf () and Format are:
         *      (1)     No need for buffer management. ANY sized buffer will be automatically allocated internally and returned as a
         *              String.
         *
         *      (2)     This version of format has a SUBTLE - but important difference from std::c++ / sprintf() in the interpretation of
         *              %s format strings in Format(const wchar_t*): %s is assumed to match a const wchar_t* string in the variable
         *              argument list.
         *
         *              This deviation from the c++ standard (technically not a deviation because Stroika::Foundation::Characters::Format() isn't
         *              covered by the stdc++ ;-)) - is because of two reasons:
         *                  a)  Microsoft has this interpretation.
         *                  b)  Its a BETTER interpretation when using wide characters.
         *
         *  \note   The type of 'format' must be - basically  a POD type - not String - due to:
         *          From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf
         *
         *              If the parameter parmN is of a reference type, or of a type that is not compatible
         *              with the type that results when passing an argument for which there is no parameter,
         *              the behavior is undefined
         *          
         *          I have to say, I don't understand this. It makes sense for the parameters after paramN, but not for the parameter
         *          to va_start () - and BTW, I've used it with type String for a while, and except for compiler warnings, never saw
         *          a problem.
         */
        String FormatV (const wchar_t* format, va_list argsList);
        String Format (const wchar_t* format, ...);
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Format.inl"

#endif /*_Stroika_Foundation_Characters_Format_h_*/
