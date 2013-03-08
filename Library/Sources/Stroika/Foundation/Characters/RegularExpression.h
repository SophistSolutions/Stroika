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
             *      @todo   VERY PRELIMINARY DRAFT
             *      @todo   add , CompareOptions co = CompareOptions::eWithCase support, and map to
             *              regex_constants::icase in regex CTOR...
             */
            class   RegularExpression {
            public:
                /**
                 *  not sure what these types mean - find out and document clearly
                 *
                 **** MAYBE REDO SO DEFAULT IS ECMASCRIPT?
                 */
                enum    class   SyntaxType : uint8_t {
                    eECMAScript,
                    eBasic ,

                    DEFAULT = SyntaxType::eBasic,
                };

            public:
                explicit RegularExpression (const String& re, SyntaxType syntaxType = SyntaxType::DEFAULT, CompareOptions co = CompareOptions::eWithCase);
                explicit RegularExpression (const String& re, CompareOptions co);
                RegularExpression (const wregex& regEx);

            public:
                nonvirtual  const wregex&   GetCompiled () const;

#if 0
            public:
                nonvirtual  const  String&  GetAsStr () const;

            public:
                nonvirtual  SyntaxType  GetSyntaxType () const;
#endif

            private:
#if 0
                SyntaxType  fSyntaxType_;
                String      fVal_;
#endif
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


