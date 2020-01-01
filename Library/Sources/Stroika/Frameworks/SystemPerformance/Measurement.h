/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Measurement_h_
#define _Stroika_Framework_SystemPerformance_Measurement_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/DataExchange/Atom.h"
#include "../../Foundation/DataExchange/VariantValue.h"

/*
 * TODO:
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

    /**
     *
     */
    using MeasurementType = DataExchange::Atom<>;

    /**
     *
     */
    struct Measurement {
        MeasurementType fType;
        VariantValue    fValue;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Measurement.inl"

#endif /*_Stroika_Framework_SystemPerformance_Measurement_h_*/
