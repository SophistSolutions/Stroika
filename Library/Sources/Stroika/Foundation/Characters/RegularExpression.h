/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_    1

#include    "../StroikaPreComp.h"

#if     !qCompilerAndStdLib_regex_Buggy
#include    <regex>
#endif

#include    "String.h"



/**
 *  \file
 *
 * Description:
 *
 *      o   See http://cpprocks.com/wp-content/uploads/c++11-regex-cheatsheet.pdf for examples
 *
 * TODO:
 *
 *      o   ...
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


#if     !qCompilerAndStdLib_regex_Buggy
            /**
             *  \brief RegularExpression is a compiled regular expression which can be used to match on a String class
             *
             *  This class is a simple wrapper on the std::wregex class.
             */
            class   RegularExpression {
            public:
                /**
                 *  not sure what these types mean - find out and document clearly
                 *
                 **** MAYBE REDO SO DEFAULT IS ECMASCRIPT?
                 */
                enum    class   SyntaxType {
                    eECMAScript     =   regex_constants::ECMAScript,
                    eBasic          =   regex_constants::basic,
                    eExtended       =   regex_constants::extended,
                    eAwk            =   regex_constants::awk,
                    eGrep           =   regex_constants::grep,
                    eEGrep          =   regex_constants::egrep,

                    eDEFAULT = SyntaxType::eBasic,
                };

            public:
                explicit RegularExpression (const String& re, SyntaxType syntaxType = SyntaxType::eDEFAULT, CompareOptions co = CompareOptions::eWithCase);
                explicit RegularExpression (const String& re, CompareOptions co);
                RegularExpression (const wregex& regEx);

            public:
                nonvirtual  const wregex&   GetCompiled () const;

            private:
                wregex      fCompiledRegExp_;
            };
#endif


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "RegularExpression.inl"

#endif  /*_Stroika_Foundation_Characters_RegularExpression_h_*/
