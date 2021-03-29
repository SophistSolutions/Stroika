/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Instrument.h"

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ******************** SystemPerformance::Instrument *****************************
 ********************************************************************************
 */
Instrument::Instrument (InstrumentNameType instrumentName, unique_ptr<ICapturer>&& capturer, const Set<MeasurementType>& capturedMeasurements, const Mapping<type_index, MeasurementType>& typeToMeasurementTypeMap, const DataExchange::ObjectVariantMapper& objectVariantMapper)
    : fInstrumentName{instrumentName}
    , fCapFun_{move (capturer)}
    , fType2MeasurementTypes{typeToMeasurementTypeMap}
    , fCapturedMeasurementTypes{capturedMeasurements}
    , fObjectVariantMapper{objectVariantMapper}
{
}
