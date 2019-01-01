/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_WMICollector_h_
#define _Stroika_Framework_SystemPerformance_Support_WMICollector_h_ 1

#include "../../StroikaPreComp.h"

#include <memory>
#include <optional>

#if qPlatform_Windows
#include <Pdh.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../../Foundation/Time/Realtime.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
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
 *              support for this, but getformattedvalue doesn't seem to support it?
 *
 *      @todo   Consider if we should support multiple objects at a time.
 *
 *      @todo   Consider use of Atom manager for tokens - as would be more efficeint than strings.
 *              Would be a good place to expierment with private 'scoped' context for atom values.
 *
 *      @todo   Consider implementation based on COM calls, since Sterl believes those maybe
 *              more efficient
 *
 *      @todo   Better exception message reporting - LoadModule/FormatMEssage -
 *              see https://msdn.microsoft.com/en-us/library/windows/desktop/aa373046(v=vs.85).aspx
 *
 */

namespace Stroika::Frameworks::SystemPerformance::Support {

    using Foundation::Characters::String;
    using Foundation::Containers::Mapping;
    using Foundation::Containers::Set;
    using Foundation::Time::DurationSecondsType;
    using Foundation::Traversal::Iterable;

    /**
     * Known good WMI object names:
     *      "Processor"
     *      "PhysicalDisk"
     *      "Memory"
     *      "System"
     *      "Network Interface"
     *      "LogicalDisk"
     *
     *  \par Example Usage
     *      WMICollector  tmp { L"Processor", {L"_Total"}, {L"% Processor Time"} };
     *      double x = tmp.GetCurrentValue (L"_Total", L"% Processor Time");
     *
     *  \par Example Usage
     *      WMICollector  tmp { L"LogicalDisk", {L"E:"},  {L"% Free Space"} };
     *      double x = tmp.GetCurrentValue (L"E:", L"% Free Space");
     *
     * Use the Windows 'Performance Monitor' tool and click PerformanceMonitor and "Add Counters" to see more/list
    */
    class WMICollector : private Foundation::Debug::AssertExternallySynchronizedLock {
    public:
        /*
            *      Special, and cannot be combined with other instances
            */
        static String kWildcardInstance;

    public:
        /**
         * Instance index is not numeric.. Often value is _Total.
         *
         *  \note the constructors may internally invoke 'collect'. (sensible for objectname/etc ctor, but less sensible
         *          for copy CTOR, but I know of know other way to clone the queries/counters). Maybe we can fix the later?
         */
        WMICollector (const String& objectName, const Iterable<String>& instances = {}, const Iterable<String>& counterName = {});
        WMICollector () = delete;
        WMICollector (const WMICollector& from);

    public:
        nonvirtual WMICollector& operator= (const WMICollector& rhs);

    public:
        /**
         */
        nonvirtual void Collect ();

    public:
        nonvirtual DurationSecondsType GetTimeOfLastCollection () const;

    public:
        /**
         *  Note - as a side-effect, this function also calls Collect() when its done, so that all the counters
         *  are consitent.
         *
         *  @see AddInstancesIf
         */
        nonvirtual void AddInstances (const String& instance);
        nonvirtual void AddInstances (const Iterable<String>& instances);

    public:
        /**
         *  Like AddInstancesIf, but does nothing if instance is already present, and returns true iff an addition was performed.
         *
         *  @see AddInstances
         */
        nonvirtual bool AddInstancesIf (const String& instance);
        nonvirtual bool AddInstancesIf (const Iterable<String>& instances);

    public:
        /**
         *  \note   Callers may wish to call 'Collect' after adding counters, to assure all counters are collected.
         */
        nonvirtual void AddCounters (const String& counterName);
        nonvirtual void AddCounters (const Iterable<String>& counterNames);

    public:
        /**
         *  Get the list of available instances for this object
         */
        nonvirtual Set<String> GetAvailableInstaces ();

    public:
        /**
         *  Get the list of available instances for this object
         */
        nonvirtual Set<String> GetAvailableCounters ();

    public:
        /**
         *  This will throw if there is an error capturing the given result.
         *
         *  @see PeekCurrentValue
         */
        nonvirtual double GetCurrentValue (const String& instance, const String& counterName);

    public:
        /**
         *  experimental wildcard API
         *
         *  \req WMICollector::kWildcardInstance
         */
        nonvirtual Mapping<String, double> GetCurrentValues (const String& counterName);

    public:
        /**
         *  Return 'missing' if the value is not available (for any reason, including obsolete instance, or whatever)
         *
         *  @see GetCurrentValue
         */
        nonvirtual optional<double> PeekCurrentValue (const String& instance, const String& counterName);

    private:
        DurationSecondsType fTimeOfLastCollection_{};
        String              fObjectName_;
        Set<String>         fCounterNames_;

    private:
        struct PerInstanceData_ {
            String                        fObjectName_;
            String                        fInstance_;
            PDH_HQUERY                    fQuery_{}; // @todo use Synchronized<> on this as a locker
            Mapping<String, PDH_HCOUNTER> fCounters_{};

            PerInstanceData_ (const String& objectName, const String& instance, const Iterable<String>& counterNames);
            PerInstanceData_ () = delete;
            ~PerInstanceData_ ();

            void                    AddCounter (const String& counterName);
            double                  GetCurrentValue (const String& counterName);
            optional<double>        PeekCurrentValue (const String& counterName);
            Mapping<String, double> GetCurrentValues (const String& counterName);
        };
        // Note - be careful not to ever copy fInstanceData_ since uses shared_ptr and would end up with two
        // collecters refering to the same instance handles (bad)
        //
        // @todo should fInstanceData use unique_ptr??
        Mapping<String, std::shared_ptr<PerInstanceData_>> fInstanceData_;

    private:
        nonvirtual void AddCounter_ (const String& counterName);

    private:
        nonvirtual void AddInstance_ (const String& instance);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WMICollector.inl"

#endif /*_Stroika_Framework_SystemPerformance_Support_WMICollector_h_*/
