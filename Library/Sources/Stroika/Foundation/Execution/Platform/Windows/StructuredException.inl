/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {
                    //  class   StructuredException
                    inline  StructuredException::StructuredException (unsigned int seCode)
                        : fSECode (seCode)
                    {
                    }
                    inline  StructuredException::operator unsigned int () const
                    {
                        return fSECode;
                    }
                    inline  SDKString StructuredException::LookupMessage () const
                    {
                        return LookupMessage (fSECode);
                    }
                }
            }

            template    <>
            inline  void    _NoReturn_  DoThrow (const Platform::Windows::StructuredException& e2Throw)
            {
                DbgTrace ("Throwing Win32StructuredException: fSECode = 0x%x", static_cast<int> (e2Throw));
                throw e2Throw;
            }


        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_*/
