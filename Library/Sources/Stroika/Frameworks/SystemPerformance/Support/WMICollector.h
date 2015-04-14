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
 *
 *      @todo   ASAP support multiple instances at a time.
 *
 *      @todo   Support remove of counters and instances.
 *
 *      @todo   See if its possible to support other 'value' return types. Sterl thinks
 *              and there is some evidence to suppor tthis - that there should be w way to get back
 *              strings (for stuff like disk name, or process name). And the API seems to ahve some
 *              support for this, but getformattedvalue doesnt seem to support it?
 *
 *      @todo   Support 'wildcards/array' somehow. Not sure what this means, but maybe just a more compact
 *              way to do some queries.
 *
 *      @todo   Consider if we should support multiple objects at a time.
 *
 *      @todo   Consider use of Atom manager for tokens - as would be more efficeint than strings.
 *              Would be a good place to expierment with private 'scoped' context for atom values.
 *
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
                 *      double x = tmp.GetCurrentValue (L"% Processor Time");
                 *
                 *  Example:
                 *      WMICollector  tmp { L"LogicalDisk", L"E:",  Set<String> {L"% Free Space"} };
                 *      double x = tmp.GetCurrentValue (L"% Free Space");
                 *
                 * Use the Windows Performance Monitor tool and click PerformanceMonitor and "Add Counters" to see more/list
                */
                class  WMICollector {
                    DurationSecondsType             fTimeOfLastCollection {};
                    String                          fObjectName_;
                    String                          fInstanceIndex_;
                    PDH_HQUERY                      fQuery {};              // @todo use Synchonized<> on this as a locker
                    Mapping<String, PDH_HCOUNTER>   fCounters {};

                public:
                    /**
                     * Instance index is not numeric.. Often value is _Total.
                     *
                     *  \note the constructors may internally invoke 'collect'. (sensible for objectname/etc ctor, but less sensible
                     *          for copy CTOR, but I know of know other way to clone the queries/counters). Maybe we can fix the later?
                     */
                    WMICollector (const String& objectName, const String& instanceIndex, const Iterable<String>& counterName);
                    WMICollector() = delete;
                    WMICollector (const WMICollector& from);

                public:
                    ~WMICollector ();

                public:
                    WMICollector& operator= (const WMICollector& rhs);

                public:
                    /**
                     */
                    nonvirtual  void    Collect ();

                public:
                    /**
                     */
                    nonvirtual  void    Add (const String& counterName);

                public:
                    /**
                     */
                    nonvirtual  double GetCurrentValue (const String& name);
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
