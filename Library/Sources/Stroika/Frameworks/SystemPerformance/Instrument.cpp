/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
Instrument::Instrument (InstrumentNameType instrumentName, unique_ptr<IRep>&& capturer, const Set<MeasurementType>& capturedMeasurements, const Mapping<type_index, MeasurementType>& typeToMeasurementTypeMap, const DataExchange::ObjectVariantMapper& objectVariantMapper)
    : pContext{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Instrument*                                    thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pContext);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fCaptureRep_->GetContext ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& context) {
              Instrument*                                         thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pContext);
              lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              thisObj->fCaptureRep_->SetContext (context);
          }}
    , pInstrumentName{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Instrument*                                    thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pInstrumentName);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fInstrumentName_;
          }}
    , pCapturedMeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument*                                    thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pCapturedMeasurementTypes);
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
        return thisObj->fCapturedMeasurementTypes_;
    }}
    , pObjectVariantMapper{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument*                                    thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pObjectVariantMapper);
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
        return thisObj->fObjectVariantMapper_;
    }}
    , pType2MeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument*                                    thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::pType2MeasurementTypes);
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
        return thisObj->fType2MeasurementTypes_;
    }}
    , fInstrumentName_{instrumentName}
    , fType2MeasurementTypes_{typeToMeasurementTypeMap}
    , fCapturedMeasurementTypes_{capturedMeasurements}
    , fObjectVariantMapper_{objectVariantMapper}
    , fCaptureRep_{move (capturer)}
{
}

Instrument& Instrument::operator= (Instrument&& rhs) noexcept
{
    lock_guard<const AssertExternallySynchronizedMutex>  critSec1{*this};
    shared_lock<const AssertExternallySynchronizedMutex> critSec2{rhs};
    fInstrumentName_           = move (rhs.fInstrumentName_);
    fCaptureRep_               = move (rhs.fCaptureRep_);
    fType2MeasurementTypes_    = move (rhs.fType2MeasurementTypes_);
    fCapturedMeasurementTypes_ = move (rhs.fCapturedMeasurementTypes_);
    fObjectVariantMapper_      = move (rhs.fObjectVariantMapper_);
    return *this;
}

Instrument& Instrument::operator= (const Instrument& rhs)
{
    lock_guard<const AssertExternallySynchronizedMutex>  critSec1{*this};
    shared_lock<const AssertExternallySynchronizedMutex> critSec2{rhs};
    fInstrumentName_           = rhs.fInstrumentName_;
    fCaptureRep_               = rhs.fCaptureRep_->Clone ();
    fType2MeasurementTypes_    = rhs.fType2MeasurementTypes_;
    fCapturedMeasurementTypes_ = rhs.fCapturedMeasurementTypes_;
    fObjectVariantMapper_      = rhs.fObjectVariantMapper_;
    return *this;
}
