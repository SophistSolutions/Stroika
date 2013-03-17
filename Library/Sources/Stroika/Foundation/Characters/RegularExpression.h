/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_    1

/**
 *  \file
 *
 * Description:
 *
 *
 * TODO:
 *
 *      o   ...
 */

#include    "../StroikaPreComp.h"

#include    <regex>

#include    "String.h"




namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


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


        }
    }
}

#endif  /*_Stroika_Foundation_Characters_RegularExpression_h_*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "RegularExpression.inl"


