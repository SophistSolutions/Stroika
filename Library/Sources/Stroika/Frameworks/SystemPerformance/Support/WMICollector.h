/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_WMICollector_h_
#define _Stroika_Framework_SystemPerformance_Support_WMICollector_h_ 1

#include    "../../StroikaPreComp.h"

#include    <memory>
#if     qPlatform_Windows
#include    <Pdh.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Time/Realtime.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 *  TODO:
 *
 *      @todo   Cleanup use of  AssertExternallySynchronizedLock once we have RECURSIVE
 *              flag.
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

                using   Foundation::Characters::String;
                using   Foundation::Containers::Mapping;
                using   Foundation::Containers::Set;
                using   Foundation::Time::DurationSecondsType;
                using   Foundation::Traversal::Iterable;


                /**
                 * Known good WMI object names:
                 *      "Processor"
                 *      "PhysicalDisk"
                 *      "Memory"
                 *      "System"
                 *      "Network Interface"
                 *      "LogicalDisk"
                 *
                 *  Example:
                 *      WMICollector  tmp { L"Processor", L"_Total", {L"% Processor Time"} };
                 *      double x = tmp.GetCurrentValue ( L"_Total", L"% Processor Time");
                 *
                 *  Example:
                 *      WMICollector  tmp { L"LogicalDisk", L"E:",  {L"% Free Space"} };
                 *      double x = tmp.GetCurrentValue (L"E:", L"% Free Space");
                 *
                 * Use the Windows Performance Monitor tool and click PerformanceMonitor and "Add Counters" to see more/list
                */
                class  WMICollector : private Foundation::Debug::AssertExternallySynchronizedLock {
                public:
                    /**
                     * Instance index is not numeric.. Often value is _Total.
                     *
                     *  \note the constructors may internally invoke 'collect'. (sensible for objectname/etc ctor, but less sensible
                     *          for copy CTOR, but I know of know other way to clone the queries/counters). Maybe we can fix the later?
                     */
                    WMICollector (const String& objectName, const String& instance, const Iterable<String>& counterName);
                    WMICollector (const String& objectName, const Iterable<String>& instances, const Iterable<String>& counterName);
                    WMICollector() = delete;
                    WMICollector (const WMICollector& from);

                public:
                    nonvirtual  WMICollector& operator= (const WMICollector& rhs);

                public:
                    /**
                     */
                    nonvirtual  void    Collect ();

                public:
                    nonvirtual  DurationSecondsType GetTimeOfLastCollection () const;

                public:
                    /**
                     *  Note - as a side-effect, this function also calls Collect() when its done, so that all the counters
                     *  are consitent.
                     *
                     *  @see AddInstancesIf
                     */
                    nonvirtual  void    AddInstances (const String& instance);
                    nonvirtual  void    AddInstances (const Iterable<String>& instances);

                public:
                    /**
                     *  Like AddInstancesIf, but does nothing if instance is already present.
                     *
                     *  @see AddInstances
                     */
                    nonvirtual  void    AddInstancesIf (const String& instance);
                    nonvirtual  void    AddInstancesIf (const Iterable<String>& instances);

                public:
                    /**
                     *  Note - as a side-effect, this function also calls Collect() when its done, so that all the counters
                     *  are consitent.
                     */
                    nonvirtual  void    AddCounters (const String& counterName);
                    nonvirtual  void    AddCounters (const Iterable<String>& counterNames);

                public:
                    /**
                     */
                    nonvirtual  double  GetCurrentValue (const String& instance, const String& counterName);

                private:
                    DurationSecondsType             fTimeOfLastCollection_ {};
                    String                          fObjectName_;
                    Set<String>                     fCounterNames_;

                private:
                    struct  PerInstanceData_ {
                        String                          fObjectName_;
                        String                          fInstance_;
                        PDH_HQUERY                      fQuery_ {};              // @todo use Synchonized<> on this as a locker
                        Mapping<String, PDH_HCOUNTER>   fCounters_ {};

                        PerInstanceData_ (const String& objectName, const String& instance, const Iterable<String>& counterNames);
                        PerInstanceData_ () = delete;
                        ~PerInstanceData_ ();

                        void    AddCounter (const String& counterName);
                        double  GetCurrentValue (const String& counterName);
                    };
                    // Note - be careful not to ever copy fInstanceData_ since uses shared_ptr and would end up with two
                    // collecters refering to the same instance handles (bad)
                    //
                    // @todo should fInstanceData use unique_ptr??
                    Mapping<String, std::shared_ptr<PerInstanceData_>>  fInstanceData_;

                private:
                    nonvirtual  void    AddCounter_ (const String& counterName);

                private:
                    nonvirtual  void    AddInstance_ (const String& instance);
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
#include    "WMICollector.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Support_WMICollector_h_*/
