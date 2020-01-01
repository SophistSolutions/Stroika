/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "Instrument.h"

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ***************** SystemPerformance::Instrument ********************************
 ********************************************************************************
 */
Instrument::Instrument (InstrumentNameType instrumentName, const SharedByValueCaptureRepType& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper)
    : fInstrumentName (instrumentName)
    , fCapFun_ (capturer)
    , fCapturedMeasurements (capturedMeasurements)
    , fObjectVariantMapper (objectVariantMapper)
{
}
