/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_inl_
#define _Stroika_Foundation_Configuration_Enumeration_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             *********************************Configuration::Inc ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline  ENUM    Inc (ENUM e)
            {
                Require (e >= typename ENUM::eSTART and e < typename ENUM::eEND);
                return static_cast<ENUM> (e + 1);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Enumeration_inl_*/
