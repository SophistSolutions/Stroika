/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_   1

#include    "../../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Configuration/Common.h"
#include    "../../Exceptions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Characters::SDKString;


            namespace   Platform {
                namespace   Windows {


                    class   StructuredException {
                    private:
                        unsigned int fSECode;

                    public:
                        explicit StructuredException (unsigned int n);
                        operator unsigned int () const;

                    public:
                        static      SDKString LookupMessage (unsigned int n);
                        nonvirtual  SDKString LookupMessage () const;

                    public:
                        static  void    RegisterHandler ();
                    private:
                        static  void    trans_func_ (unsigned int u, EXCEPTION_POINTERS* pExp);
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StructuredException.inl"

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_*/
