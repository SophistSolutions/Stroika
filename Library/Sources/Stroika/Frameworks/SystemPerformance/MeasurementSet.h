/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_MeasurementSet_h_
#define _Stroika_Framework_SystemPerformance_MeasurementSet_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Time/Realtime.h"
#include "../../Foundation/Traversal/Range.h"

#include "Measurement.h"

/*
 * TODO:
 *
 *      @todo   Consider adding some sort of MeasurementType registry to capture meta inforamtion - descriptions
 *              etc - about measurements. Don't do yet, cuz I'm not sure how to manage it - when to add things.
 *
 *              The challenge is how do to this modularly.
 *
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Collection;
    using Time::DurationSecondsType;
    using Traversal::Range;

    /**
     *  A MeasurementSet is typically produced by a call to Instrument::Capture (). It represents the data
     *  from a single run of that instrument. It can be point in time, or potentially span a range of times.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    struct MeasurementSet {
        Range<DurationSecondsType> fMeasuredAt;

        // @todo - this should use KeyedCollection, as the Measurement::fType is a KEY to this collection
        Collection<Measurement> fMeasurements;

        /**
         *  Combine m with this measurement set. any overlapping measurements replace their cooresponding values.
         */
        nonvirtual void MergeAdditions (const MeasurementSet& m);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MeasurementSet.inl"

#endif /*_Stroika_Framework_SystemPerformance_MeasurementSet_h_*/
