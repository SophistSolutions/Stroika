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
    : pContext{
          [this] ([[maybe_unused]] const auto* property) {
              return fCapFun_.fCap_->GetConext ();
          },
          [this] ([[maybe_unused]] auto* property, const auto& context) {
              fCapFun_.fCap_->SetConext (context);
          }}
    , pInstrumentName{
          [this] ([[maybe_unused]] const auto* property) {
              return fInstrumentName;
          }}
    , pCapturedMeasurementTypes{[this] ([[maybe_unused]] const auto* property) {
        return fCapturedMeasurementTypes;
    }}
    , pObjectVariantMapper{[this] ([[maybe_unused]] const auto* property) {
        return fObjectVariantMapper;
    }}
    , pType2MeasurementTypes{[this] ([[maybe_unused]] const auto* property) {
        return fType2MeasurementTypes;
    }}
    , fInstrumentName{instrumentName}
    , fType2MeasurementTypes{typeToMeasurementTypeMap}
    , fCapturedMeasurementTypes{capturedMeasurements}
    , fObjectVariantMapper{objectVariantMapper}
    , fCapFun_{move (capturer)}
{
}

Instrument& Instrument::operator= (const Instrument& rhs)
{
    fInstrumentName           = rhs.fInstrumentName;
    fCapFun_                  = rhs.fCapFun_;
    fType2MeasurementTypes    = rhs.fType2MeasurementTypes;
    fCapturedMeasurementTypes = rhs.fCapturedMeasurementTypes;
    fObjectVariantMapper      = rhs.fObjectVariantMapper;
    return *this;
}
