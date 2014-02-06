/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Capturer_h_
#define _Stroika_Framework_SystemPerformance_Capturer_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/Time/Duration.h"

#include    "CaptureSet.h"
#include    "Measurement.h"


/*
 * TODO:
 *      @todo   Use new (NYI as of 2014-02-06) Callback (or Function??) helper class so that Callbacks can be
 *              unregistered.
 *
 *      @todo   Consider if this should use threadpool (and if so how exactly?) Maybe push ALL measurements into
 *              threadpool at just the right time, and then paralell process as much as the threadpool size allows.
 *              That seems the best appraoch! Default to theadpool size of one! One recurring task in threadpool
 *              would be to re-schedule rest of measurements...
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Set;
            using   Time::Duration;


            /**
             *  A Capturer is a utility you MAY wish to use the the SystemPerformance framework. It provides some
             *  fairly handy default mechanisms to manage (possibly multiple) CaptureSets, and either store the last result
             *  for each, or to run callbacks on those results.
             *
             *  This also runs its capturing on a (single) background thread. This has implications for how much its able to
             *  keep up with and maintain all the measurements in question.
             */
            class   Capturer {
            public:
                Capturer ();

            public:
                /**
                *WRONG RETVAL
                 */
                nonvirtual  MeasurementSet    GetTrackLastMeasurementSet () const;

            public:
                /**
                *WRONG ARG
                 */
                nonvirtual  void        SetGetTrackLastMeasurementSet (const MeasurementSet& p);

            public:
                /**
                 */
                nonvirtual  MeasurementSet    GetLastMeasurementSet () const;

            public:
                /**
                *WRONG RETVAL
                 */
                nonvirtual  Duration    GetMeasurementsCallbacks () const;

            public:
                /**
                *WRONG ARG
                 */
                nonvirtual  void        SetMeasurementsCallbacks (const Duration& p);

            public:
                /**
                *WRONG ARG
                 */
                nonvirtual  void        AddMeasurementsCallback (const Duration& p);

            public:
                /**
                 */
                nonvirtual  Set<CaptureSet>   GetCaptureSets () const;

            public:
                /**
                 */
                nonvirtual  void            SetCaptureSets (const Set<CaptureSet>& captureSets);

            private:
                InstrumentSet   fInstruments_;
                Duration        fPeriod_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Capturer.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Capturer_h_*/
