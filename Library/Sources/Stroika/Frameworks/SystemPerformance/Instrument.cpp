/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Instrument.h"

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Debug::AssertExternallySynchronizedMutex;

/*
 ********************************************************************************
 ******************** SystemPerformance::Instrument *****************************
 ********************************************************************************
 */
Instrument::Instrument (InstrumentNameType instrumentName, unique_ptr<IRep>&& capturer, const Set<MeasurementType>& capturedMeasurements,
                        const Mapping<type_index, MeasurementType>& typeToMeasurementTypeMap, const DataExchange::ObjectVariantMapper& objectVariantMapper)
    : context{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                  const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::context);
                  AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
                  return thisObj->fCaptureRep_->GetContext ();
              },
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& context) {
                  Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::context);
                  AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                  thisObj->fCaptureRep_->SetContext (context);
              }}
    , instrumentName{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::instrumentName);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fInstrumentName_;
    }}
    , capturedMeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::capturedMeasurementTypes);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fCapturedMeasurementTypes_;
    }}
    , objectVariantMapper{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::objectVariantMapper);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fObjectVariantMapper_;
    }}
    , type2MeasurementTypes{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Instrument* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Instrument::type2MeasurementTypes);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    AssertExternallySynchronizedMutex::WriteContext declareWriteContext2{rhs.fThisAssertExternallySynchronized_};
    fInstrumentName_           = move (rhs.fInstrumentName_);
    fCaptureRep_               = move (rhs.fCaptureRep_);
    fType2MeasurementTypes_    = move (rhs.fType2MeasurementTypes_);
    fCapturedMeasurementTypes_ = move (rhs.fCapturedMeasurementTypes_);
    fObjectVariantMapper_      = move (rhs.fObjectVariantMapper_);
    return *this;
}

Instrument& Instrument::operator= (const Instrument& rhs)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    AssertExternallySynchronizedMutex::ReadContext  readLock{rhs.fThisAssertExternallySynchronized_};
    fInstrumentName_           = rhs.fInstrumentName_;
    fCaptureRep_               = rhs.fCaptureRep_->Clone ();
    fType2MeasurementTypes_    = rhs.fType2MeasurementTypes_;
    fCapturedMeasurementTypes_ = rhs.fCapturedMeasurementTypes_;
    fObjectVariantMapper_      = rhs.fObjectVariantMapper_;
    return *this;
}
