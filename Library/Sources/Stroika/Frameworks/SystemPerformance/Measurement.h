/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Measurement_h_
#define _Stroika_Framework_SystemPerformance_Measurement_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/Atom.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"

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
     *  A measurement refers to a single UNIT of related fields from an Instrument. It often contains tens or hundreds of fields.
     *  the fType field is an atom, which identifies the object type you can convert the fValue to.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    struct Measurement {
        MeasurementType fType;
        VariantValue    fValue;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Measurement.inl"

#endif /*_Stroika_Framework_SystemPerformance_Measurement_h_*/
