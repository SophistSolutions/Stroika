/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CaptureSet_h_
#define _Stroika_Framework_SystemPerformance_CaptureSet_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Time/Duration.h"

#include "Instrument.h"

/**
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Set;
    using Time::Duration;

    /**
     *  This is a collection of instruments (which are in turn sets of measureables) all collected on a common
     *  time period.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class CaptureSet {
    public:
        /**
         */
        CaptureSet (const Duration& period = 30s, const Set<Instrument>& instruments = {});
        CaptureSet (const CaptureSet& src);

    public:
        nonvirtual CaptureSet& operator= (const CaptureSet& rhs);

    public:
        /**
         * @brief All the Instruments have their data 'captured' automatically every 'run-period' time interval.
         * 
         *  So, in particular, any averages captured will generally, by default, be relative to this interval
         *  (unless the instrument documents a different pattern, such as load-average).
         */
        Common::Property<Duration> pRunPeriod;

    public:
        /**
         * @brief This is the set of instruments which will be captured on the regular interval basis
         */
        Common::Property<Set<Instrument>> pInstruments;

    public:
        /**
         */
        nonvirtual void AddInstrument (const Instrument& i);

    private:
        Set<Instrument> fInstruments_;
        Duration        fPeriod_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CaptureSet.inl"

#endif /*_Stroika_Framework_SystemPerformance_CaptureSet_h_*/
