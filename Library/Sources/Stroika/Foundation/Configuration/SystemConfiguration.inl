/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_inl_
#define _Stroika_Foundation_Configuration_SystemConfiguration_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             ****************** SystemConfiguration GetSystemConfiguration ******************
             ********************************************************************************
             */
            inline  SystemConfiguration GetSystemConfiguration ()
            {
                return SystemConfiguration {
                    GetSystemConfiguration_CPU (),
                    GetSystemConfiguration_Memory (),
                    GetSystemConfiguration_OperatingSystem (),
                    GetSystemConfiguration_ComputerNames ()
                };
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_SystemConfiguration_inl_*/
