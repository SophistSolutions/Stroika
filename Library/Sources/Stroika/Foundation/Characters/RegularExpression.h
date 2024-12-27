/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <regex>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"

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
         *  \note   We chose ECMAScript as a default, to match what stdC++ chose as the default.
         */
        enum class SyntaxType {
            /**
             *  http://en.cppreference.com/w/cpp/regex/ecmascript
             */
            eECMAScript = regex_constants::ECMAScript,

            /**
             *  https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03
             */
            eBasic = regex_constants::basic,

            /**
             * https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_04
             */
            eExtended = regex_constants::extended,

            /**
             *  https://pubs.opengroup.org/onlinepubs/9699919799/utilities/awk.html#tag_20_06_13_04
             */
            eAwk = regex_constants::awk,

            /**
             *  https://pubs.opengroup.org/onlinepubs/9699919799/utilities/grep.html
             */
            eGrep  = regex_constants::grep,
            eEGrep = regex_constants::egrep,

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
         * 
         *  \par Example Usage
         *      \code
         *          // see https://www.gnu.org/software/emacs/manual/html_node/elisp/Char-Classes.html#Char-Classes
         *          const auto kSingleWhitespaceChar = RegularExpression{RegularExpression::eBasic, "[:blank:]"sv};
         *          const auto kSeriesOfWhitespaceCharacters = RegularExpression{RegularExpression::eBasic, "[:blank:]+"sv};
         *      \endcode
         */
        explicit RegularExpression ();
        explicit RegularExpression (SyntaxType syntaxType, const String& re, CompareOptions co = eWithCase);
        explicit RegularExpression (const String& re, CompareOptions co = eWithCase);
        RegularExpression (const wregex& regEx);
        RegularExpression (wregex&& regEx);

    public:
        /**
         *   Predefined regular expression that matches nothing.
         *
         *   \note Since this is a static object, beware, it cannot be (safely) used before or after main
         *   \note Equivalent to
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
         *   \note Since this is a static object, beware, it cannot be (safely) used before or after main
         *   \note Equivalent to
         *       \code
         *           const  RegularExpression   kAny {".*"};
         *       \endcode
         */
        static const RegularExpression kAny;

    public:
        nonvirtual const wregex& GetCompiled () const;

    public:
        [[deprecated ("Since Stroika v3.0d14 - use SyntaxType before the regexp")]]
        explicit RegularExpression (const String& re, SyntaxType syntaxType, CompareOptions co = eWithCase)
            : RegularExpression (syntaxType, re, co)
        {
        }

    private:
        wregex fCompiledRegExp_;
    };
    inline const RegularExpression RegularExpression::kNONE{"(?!)"sv};
    inline const RegularExpression RegularExpression::kAny{".*"sv};

    /**
     *  This class doesn't do anything. It just collects together a match result/response.
     */
    class RegularExpressionMatch {
    public:
        RegularExpressionMatch (const String& fullMatch);
        RegularExpressionMatch (const String& fullMatch, const Containers::Sequence<String>& subMatches);

    public:
        String GetFullMatch () const;

    public:
        Containers::Sequence<String> GetSubMatches () const;

    private:
        String                       fFullMatch_;
        Containers::Sequence<String> fSubMatches_;
    };

    inline namespace Literals {
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RegularExpression.inl"

#endif /*_Stroika_Foundation_Characters_RegularExpression_h_*/
