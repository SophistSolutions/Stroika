/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_    1

/**
 *
 * Description:
 *
 *
 *     ...
 */

#include    "../StroikaPreComp.h"

#include    <regex>

#include    "String.h"



/**
 * TODO:
 *
 *      o   ...
 *
 */



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
                enum    class   SyntaxType : uint8_t {
                    eBasic ,
                    eECMAScript,

                    DEFAULT = SyntaxType::eBasic,

                    Define_Start_End_Count(eBasic, eECMAScript)
                };

            public:
                explicit RegularExpression (const String& re, SyntaxType syntaxType = SyntaxType::DEFAULT, CompareOptions co = CompareOptions::eWithCase);
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


