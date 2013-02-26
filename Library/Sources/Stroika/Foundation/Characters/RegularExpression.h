/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_h_
#define _Stroika_Foundation_Characters_RegularExpression_h_    1

/*
 *
 * Description:
 *
 *
 *     ...
 */

#include    "../StroikaPreComp.h"

#include    "String.h"



/*
 * TODO:
 *
 *
 *      o   ...
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            /**
             *      @todo   VERY PRELIMINARY DRAFT
             */
            class   RegularExpression {
            public:
                // not sure what these types mean - find out and document clearly
                enum class SyntaxType : uint8_t { eECMAScript, eBasic };
            public:
                explicit RegularExpression (const String& re, SyntaxType syntaxType = SyntaxType::eBasic);

            private:
                SyntaxType fSyntaxType_;

            public:
                String  GetAsStr () const;
                SyntaxType  GetSyntaxType () const;
            private:
                String  fVal_;
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


