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
             **************************** SystemConfiguration::CPU **************************
             ********************************************************************************
             */
            inline  unsigned int    SystemConfiguration::CPU::GetNumberOfLogicalCores () const
            {
                return fCores.size ();
            }
            inline  String    SystemConfiguration::CPU::GetCPUModelPrintName () const
            {
                return fCores.empty () ? String () : fCores[0].fModelName;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_SystemConfiguration_inl_*/
