/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Capturer_h_
#define _Stroika_Framework_SystemPerformance_Capturer_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Execution/Function.h"
#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/Time/Duration.h"

#include "CaptureSet.h"
#include "Measurement.h"

/*
 * TODO:
 *      @todo   Somehow make captrueset comparable, so that we can use it as a KEY with mreasumrnes
 *              and then store latest for each captureset.
 *
 *              Then combine into one massive measuremnets list fMeasurmentsMapping.Values() (once
 *              we have impelemtend values() on mapping).
 *
 *      @todo   VERY primitive, but for simple cases functional implementation. FIX IT, so its better, and
 *              more complete (and efficient).
 *
 *      @todo   Current impl only supports one CaptureSet<>. To support more, we need a strategy to merge
 *              or handle composites.
 *
 *      @todo   make it optional storing the most recent measurement set.
 *
 *      @todo   Use threadpool CORRECTLY!. Maybe push ALL measurements into
 *              threadpool at just the right time, and then paralell process as much as the threadpool size allows.
 *              That seems the best appraoch! Default to theadpool size of one! One recurring task in threadpool
 *              would be to re-schedule rest of measurements...
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Collection;
    using Containers::Set;
    using Execution::Function;
    using Time::Duration;

    /**
     *  A Capturer is a utility you MAY wish to use the the SystemPerformance framework. It provides some
     *  fairly handy default mechanisms to manage (possibly multiple) CaptureSets, and either store the last result
     *  for each, or to run callbacks on those results.
     *
     *  This also runs its capturing on a (single) background thread. This has implications for how much its able to
     *  keep up with and maintain all the measurements in question.
     *
     *  Note - there is no reason you cannot use the rest of the SystemPerformance framework without this class,
     *  if its pattern doesn't meet your needs.
     */
    class Capturer {
    public:
        Capturer ()                = default;
        Capturer (const Capturer&) = delete;
        Capturer& operator= (const Capturer&) = delete;

    public:
        /**
         *  Call this anytime (for example if you don't want to bother with callbacks or if
         *  some other process needs to query the latest values from the instrument measurers.
         */
        nonvirtual MeasurementSet GetMostRecentMeasurements () const;

    public:
        /**
         */
        using NewMeasurementsCallbackType = Function<void(MeasurementSet)>;

    public:
        /**
         */
        nonvirtual Collection<NewMeasurementsCallbackType> GetMeasurementsCallbacks () const;

    public:
        /**
         */
        nonvirtual void SetMeasurementsCallbacks (const Collection<NewMeasurementsCallbackType>& callbacks);

    public:
        /**
         */
        nonvirtual void AddMeasurementsCallback (const NewMeasurementsCallbackType& cb);

    public:
        /**
         */
        nonvirtual void RemoveMeasurementsCallback (const NewMeasurementsCallbackType& cb);

    public:
        /**
         */
        nonvirtual Collection<CaptureSet> GetCaptureSets () const;

    public:
        /**
         */
        nonvirtual void SetCaptureSets (const Collection<CaptureSet>& captureSets);

    public:
        /**
         */
        nonvirtual void AddCaptureSet (const CaptureSet& cs);

    private:
        nonvirtual void ManageRunner_ (bool on);

    private:
        nonvirtual void Runner_ ();

    private:
        // FOR NOW - just assign/overwrite the latest measurement set, and call
        // callbacks as needed
        nonvirtual void UpdateMeasurementSet_ (const MeasurementSet& ms);

    private:
        Execution::Synchronized<Collection<CaptureSet>>                  fCaptureSets_;
        Execution::Synchronized<Collection<NewMeasurementsCallbackType>> fCallbacks_;
        Execution::Synchronized<MeasurementSet>                          fCurrentMeasurementSet_;
        Execution::ThreadPool                                            fThreadPool_; // Subtle - construct last so auto-destructed first (shuts down threads)
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Capturer.inl"

#endif /*_Stroika_Framework_SystemPerformance_Capturer_h_*/
