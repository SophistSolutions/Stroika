/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Basic_inl_
#define _Stroika_Foundation_Memory_Basic_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            //  class   Memory::GlobalAllocationStatistics
            inline  GlobalAllocationStatistics::GlobalAllocationStatistics ():
                fTotalOutstandingAllocations (0),
                fTotalOutstandingBytesAllocated (0),
                fPageFaultCount (0),
                fWorkingSetSize (0),
                fPagefileUsage (0)
            {
            }


            // Module initialization
            namespace   Private {
                struct  INIT {
                    INIT ();
                    ~INIT ();
                };
            }

#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
            namespace Private {
                template<class _InIt, class _OutIt>
                inline  void    VC_BWA_std_copy (_InIt _First, _InIt _Last, _OutIt _Dest)
                {
                    auto    o   =   _Dest;
                    for (auto i = _First; i != _Last; ++i, ++o) {
                        *o = *i;
                    }
                }
            }
#endif

        }
    }
}


namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Memory::Private::INIT>   _StroikaFoundationMemoryPrivateINIT_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Memory_Basic_inl_*/
