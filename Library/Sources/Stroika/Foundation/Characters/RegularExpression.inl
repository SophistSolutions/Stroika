/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_inl_
#define _Stroika_Foundation_Characters_RegularExpression_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            //  class  RegularExpression
            inline  RegularExpression::RegularExpression (const String& re, SyntaxType syntaxType)
                : fVal_ (re)
                , fSyntaxType_ (syntaxType)
            {
            }
            inline  String  RegularExpression::GetAsStr () const
            {
                return fVal_;
            }
            inline  RegularExpression::SyntaxType  RegularExpression::GetSyntaxType () const
            {
                return fSyntaxType_;
            }

        }
    }
}


#endif // _Stroika_Foundation_Characters_RegularExpression_inl_
