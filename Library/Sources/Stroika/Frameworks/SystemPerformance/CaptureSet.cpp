/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "CaptureSet.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 *********************** SystemPerformance::CaptureSet **************************
 ********************************************************************************
 */
CaptureSet::CaptureSet (const Duration& period, const Set<Instrument>& instruments)
    : pRunPeriod{
          [this] ([[maybe_unused]] const auto* property) {
              return fPeriod_;
          },
          [this] ([[maybe_unused]] auto* property, const auto& runPeriod) {
              fPeriod_ = runPeriod;
          }}
    , pInstruments{
          [this] ([[maybe_unused]] const auto* property) {
              return fInstruments_;
          },
          [this] ([[maybe_unused]] auto* property, const auto& instruments) {
              fInstruments_ = instruments;
          }}
    , fInstruments_ (instruments)
    , fPeriod_ (period)
{
}
CaptureSet::CaptureSet (const CaptureSet& src)
    : CaptureSet{src.fPeriod_, src.fInstruments_}
{
}

CaptureSet& CaptureSet::operator= (const CaptureSet& rhs)
{
    fInstruments_ = rhs.fInstruments_;
    fPeriod_      = rhs.fPeriod_;
    return *this;
}
