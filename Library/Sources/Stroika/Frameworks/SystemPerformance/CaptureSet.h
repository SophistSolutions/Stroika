/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CaptureSet_h_
#define _Stroika_Framework_SystemPerformance_CaptureSet_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Time/Duration.h"

#include "Instrument.h"

/*
 * TODO:
 *      @todo
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Set;
    using Time::Duration;

    /**
     *  This is a collection of instruments (which are in turn sets of measureables) all collected on a common
     *  time period.
     */
    class CaptureSet {
    public:
        CaptureSet (const Duration& period = Duration (30.0), const Set<Instrument>& instruments = {});

    public:
        /**
         */
        nonvirtual Duration GetRunPeriod () const;

    public:
        /**
         */
        nonvirtual void SetRunPeriod (const Duration& p);

    public:
        /**
         */
        nonvirtual Set<Instrument> GetInstrumentSet () const;

    public:
        /**
         */
        nonvirtual void SetInstrumentSet (const Set<Instrument>& i);

    public:
        nonvirtual void AddInstrument (const Instrument i);

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
