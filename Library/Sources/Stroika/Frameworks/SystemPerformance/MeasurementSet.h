/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *      @todo   HANDLE MERGE (or COMBINE) feature on MeasurmentSet...
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
    using DataExchange::VariantValue;
    using Time::DurationSecondsType;
    using Traversal::Range;

    /**
     *  A MeasurementSet is typically produced by a call to Instrument::Capture (). It represents the data
     *  from a single run of that instrument. It can be point in time, or potentially span a range of times.
     *
     *  MeasurementSets can be combined (@see MeasurementSet::Merge).
     */
    struct MeasurementSet {
        Range<DurationSecondsType> fMeasuredAt;

        Collection<Measurement> fMeasurements;

        // NEED PARAMS TO SAY HOW TO COMBINE - MAYBE RENAME THIS TO COMBINE?
        // WHAT ABOUT ADDING HEIRARCHY??? HOW TO COMBINE MEASUREDAT???
        // ONE ANSEER IS MERGE AT THIS LEVEN AND EXPAND MEASUREDAT.
        // ANOTHE aNSER IS CREATE  A NERW "INSTURMENT" that jsut adds HIERARCHY (and a label for sub-measured-at).
        // NYI
        //
        nonvirtual MeasurementSet Merge (const MeasurementSet& rhs) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MeasurementSet.inl"

#endif /*_Stroika_Framework_SystemPerformance_MeasurementSet_h_*/
