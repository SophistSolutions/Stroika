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

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
             ********************************************************************************
             ********************** Characters::RegularExpression ***************************
             ********************************************************************************
             */
            inline  RegularExpression::RegularExpression (const wregex& regEx)
                :  fCompiledRegExp_ (regEx)
            {
            }
            inline  const wregex&  RegularExpression::GetCompiled () const
            {
                return fCompiledRegExp_;
            }
#if 0
            inline  const String&  RegularExpression::GetAsStr () const
            {
                return fVal_;
            }
            inline  RegularExpression::SyntaxType  RegularExpression::GetSyntaxType () const
            {
                return fSyntaxType_;
            }
#endif


        }
    }
}


#endif // _Stroika_Foundation_Characters_RegularExpression_inl_
