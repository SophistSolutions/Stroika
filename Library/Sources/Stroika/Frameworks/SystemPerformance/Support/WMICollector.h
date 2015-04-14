/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_WMICollector_h_
#define _Stroika_Framework_SystemPerformance_Support_WMICollector_h_ 1

#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Pdh.h>
#include    <PdhMsg.h>
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Time/Realtime.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 *  TODO:
 *      @todo   Consider implementation based on COM calls, since Sterl believes those maybe
 *              more efficient
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Support {

                using   Foundation::Containers::Mapping;
                using   Foundation::Characters::String;
                using   Foundation::Traversal::Iterable;
                using   Foundation::Time::DurationSecondsType;


                /**
                 *  @todo FIX THREADSAFTY OF THIS OBJECT!!!! (or add check to assure used extnerally Debug::AssertExtenrallySynchonized...)
                 *
                 *
                 * Known good WMI object names:
                 *  "Processor"
                 *  "PhysicalDisk"
                 *  "Memory"
                 *  "System"
                 *  "Network Interface"
                 *  "LogicalDisk"
                 *
                 *  Example:
                 *      WMICollector  tmp { L"Processor", L"_Total", Set<String> {L"% Processor Time"} };
                 *      double x = tmp.getCurrentValue (L"% Processor Time");
                 *
                 *  Example:
                 *      WMICollector  tmp { L"LogicalDisk", L"E:",  Set<String> {L"% Free Space"} };
                 *      double x = tmp.getCurrentValue (L"% Free Space");
                 *
                 * Use the Windows Performance Monitor tool and click PerformanceMonitor and "Add Counters" to see more/list
                */
                struct  WMICollector {
                    DurationSecondsType             fTimeOfLastCollection {};
                    String                          fObjectName_;
                    String                          fInstanceIndex_;
                    PDH_HQUERY                      fQuery {};              // @todo use Synchonized<> on this as a locker
                    Mapping<String, PDH_HCOUNTER>   fCounters {};

                    /**
                     * Instance index is not numeric.. Often value is _Total
                     */
                    WMICollector (const String& objectName, const String& instanceIndex, const Iterable<String>& counterName);
                    WMICollector() = delete;
                    WMICollector (const WMICollector& from);
                    ~WMICollector ();
                    WMICollector& operator= (const WMICollector& rhs);

                    /**
                     */
                    void    Collect ();

                    /**
                     */
                    void    Add (const String& counterName);

                    /**
                     */
                    double getCurrentValue (const String& name);
                };
            }


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Support_WMICollector_h_*/
