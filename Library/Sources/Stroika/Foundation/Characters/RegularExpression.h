/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_ 1

#include "../StroikaPreComp.h"

#include <regex>

#include "../Containers/Sequence.h"

#include "String.h"

/**
 *  \file
 *
 * Description:
 *      o   See http://cpprocks.com/wp-content/uploads/c++11-regex-cheatsheet.pdf for examples
 *
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief RegularExpression is a compiled regular expression which can be used to match on a String class
     *
     *  This class is a simple wrapper on the std::wregex class.
     */
    class RegularExpression {
    public:
        /**
         *  not sure what these types mean - find out and document clearly
         *
         *  \note   We chose ECMAScript as a default, to match what stdC++ chose as the default.
         */
        enum class SyntaxType {
            /**
             *  http://en.cppreference.com/w/cpp/regex/ecmascript
             */
            eECMAScript = regex_constants::ECMAScript,
            eBasic      = regex_constants::basic,
            eExtended   = regex_constants::extended,
            eAwk        = regex_constants::awk,
            eGrep       = regex_constants::grep,
            eEGrep      = regex_constants::egrep,

            eDEFAULT = eECMAScript,
        };
        using SyntaxType::eAwk;
        using SyntaxType::eBasic;
        using SyntaxType::eECMAScript;
        using SyntaxType::eEGrep;
        using SyntaxType::eExtended;
        using SyntaxType::eGrep;

    public:
        /**
         *  \note RegularExpression {} creates a special regular expression that matches nothing.
         *  \note RegularExpression (String re) throws std::regex_error () if provided an invalid regular expression.
         *  \note The default syntax is ECMAScript.
         */
        explicit RegularExpression ();
        explicit RegularExpression (const String& re, SyntaxType syntaxType = SyntaxType::eDEFAULT, CompareOptions co = eWithCase);
        explicit RegularExpression (const String& re, CompareOptions co);
        RegularExpression (const wregex& regEx);
        RegularExpression (wregex&& regEx);

    public:
        /**
         *   Predefined regular expression that matches nothing.
         *
         *   \note Since this is a static object, bewaware, it cannot be (safely) used before or after main
         *   \note Equivilent to
         *       \code
         *           const  RegularExpression   kMatchNone ("(?!)");   // OR
         *           const  RegularExpression   kMatchNoneAlternative{};
         *       \endcode
         */
        static const RegularExpression kNONE;

    public:
        /**
         *   Predefined regular expression that matches anything.
         *
         *   \note Since this is a static object, bewaware, it cannot be (safely) used before or after main
         *   \note Equivilent to
         *       \code
         *           const  RegularExpression   kAny {".*"};
         *       \endcode
         */
        static const RegularExpression kAny;

    public:
        nonvirtual const wregex& GetCompiled () const;

    private:
        wregex fCompiledRegExp_;
    };
    inline const RegularExpression RegularExpression::kNONE{"(?!)"sv};
    inline const RegularExpression RegularExpression::kAny{".*"sv};

    /**
     */
    class RegularExpressionMatch {
    public:
        RegularExpressionMatch (const String& fullMatch);
        RegularExpressionMatch (const String& fullMatch, const Containers::Sequence<String>& subMatches);
        String                       GetFullMatch () const;
        Containers::Sequence<String> GetSubMatches () const;

    private:
        String                       fFullMatch_;
        Containers::Sequence<String> fSubMatches_;
    };

    /**
     *  \brief user defined literal for RegularExpression. These are always of type RegularExpression::SyntaxType::eDEFAULT (ie eECMAScript)
     * 
     *  \req FOR 'char' overload, argument REQUIRES all text is ASCII (uses String::FromStringConstant)
     */
    RegularExpression operator"" _RegEx (const char* str, size_t len);
    RegularExpression operator"" _RegEx (const wchar_t* str, size_t len);
    RegularExpression operator"" _RegEx (const char8_t* str, size_t len);
    RegularExpression operator"" _RegEx (const char16_t* str, size_t len);
    RegularExpression operator"" _RegEx (const char32_t* str, size_t len);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RegularExpression.inl"

#endif /*_Stroika_Foundation_Characters_RegularExpression_h_*/
