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
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pContext);
              return thisObj->fCaptureRep_->GetConext ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& context) {
              Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pContext);
              thisObj->fCaptureRep_->SetConext (context);
          }}
    , pInstrumentName{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pInstrumentName);
              return thisObj->fInstrumentName_;
          }}
    , pCapturedMeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pCapturedMeasurementTypes);
        return thisObj->fCapturedMeasurementTypes_;
    }}
    , pObjectVariantMapper{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pObjectVariantMapper);
        return thisObj->fObjectVariantMapper_;
    }}
    , pType2MeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pType2MeasurementTypes);
        return thisObj->fType2MeasurementTypes_;
    }}
    , fInstrumentName_{instrumentName}
    , fType2MeasurementTypes_{typeToMeasurementTypeMap}
    , fCapturedMeasurementTypes_{capturedMeasurements}
    , fObjectVariantMapper_{objectVariantMapper}
    , fCaptureRep_{move (capturer)}
{
}

Instrument& Instrument::operator= (const Instrument& rhs)
{
    fInstrumentName_           = rhs.fInstrumentName_;
    fCaptureRep_               = rhs.fCaptureRep_->Clone ();
    fType2MeasurementTypes_    = rhs.fType2MeasurementTypes_;
    fCapturedMeasurementTypes_ = rhs.fCapturedMeasurementTypes_;
    fObjectVariantMapper_      = rhs.fObjectVariantMapper_;
    return *this;
}
