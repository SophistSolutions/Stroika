/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CaptureSet_h_
#define _Stroika_Framework_SystemPerformance_CaptureSet_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Time/Duration.h"

#include "InstrumentSet.h"

/*
 * TODO:
 *      @todo
 */

namespace Stroika {
    namespace Frameworks {
        namespace SystemPerformance {

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
                CaptureSet (const Duration& period = Duration (30.0), const InstrumentSet& instruments = InstrumentSet ());

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
                nonvirtual InstrumentSet GetInstrumentSet () const;

            public:
                /**
                 */
                nonvirtual void SetInstrumentSet (const InstrumentSet& i);

            public:
                nonvirtual void AddInstrument (const Instrument i);

            private:
                InstrumentSet fInstruments_;
                Duration      fPeriod_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CaptureSet.inl"

#endif /*_Stroika_Framework_SystemPerformance_CaptureSet_h_*/
