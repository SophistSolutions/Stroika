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
              return fInstrumentName_;
          }}
    , pCapturedMeasurementTypes{[this] ([[maybe_unused]] const auto* property) {
        return fCapturedMeasurementTypes_;
    }}
    , pObjectVariantMapper{[this] ([[maybe_unused]] const auto* property) {
        return fObjectVariantMapper_;
    }}
    , pType2MeasurementTypes{[this] ([[maybe_unused]] const auto* property) {
        return fType2MeasurementTypes_;
    }}
    , fInstrumentName_{instrumentName}
    , fType2MeasurementTypes_{typeToMeasurementTypeMap}
    , fCapturedMeasurementTypes_{capturedMeasurements}
    , fObjectVariantMapper_{objectVariantMapper}
    , fCapFun_{move (capturer)}
{
}

Instrument& Instrument::operator= (const Instrument& rhs)
{
    fInstrumentName_           = rhs.fInstrumentName_;
    fCapFun_                  = rhs.fCapFun_;
    fType2MeasurementTypes_    = rhs.fType2MeasurementTypes_;
    fCapturedMeasurementTypes_ = rhs.fCapturedMeasurementTypes_;
    fObjectVariantMapper_      = rhs.fObjectVariantMapper_;
    return *this;
}
