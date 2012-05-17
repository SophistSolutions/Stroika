/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Basics_inl_
#define _Stroika_Foundation_Configuration_Basics_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {

            template    <typename   ENUM>
            inline  ENUM    Inc (ENUM e) {
                return static_cast<ENUM> (e + 1);
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Basics_inl_*/



