/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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


#if     !qCompilerAndStdLib_regex_Buggy
            /*
             ********************************************************************************
             ********************** Characters::RegularExpression ***************************
             ********************************************************************************
             */
            inline  RegularExpression::RegularExpression ()
                :  fCompiledRegExp_ ()
            {
            }
            inline  RegularExpression::RegularExpression (const wregex& regEx)
                :  fCompiledRegExp_ (regEx)
            {
            }
            inline  RegularExpression::RegularExpression (wregex&& regEx)
                :  fCompiledRegExp_ (move (regEx))
            {
            }
            inline  const wregex&  RegularExpression::GetCompiled () const
            {
                return fCompiledRegExp_;
            }
#endif


        }
    }
}


#endif // _Stroika_Foundation_Characters_RegularExpression_inl_
