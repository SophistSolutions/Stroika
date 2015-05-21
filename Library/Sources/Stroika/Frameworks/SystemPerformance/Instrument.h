/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instrument_h_
#define _Stroika_Framework_SystemPerformance_Instrument_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"
#include    "../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../Foundation/Execution/Function.h"

#include    "Measurement.h"
#include    "MeasurementSet.h"


/*
 * TODO:
 *      @todo   Try to do specialization of CaptureOneMeasurement in each instrument that avoids the
 *              serialize/deserialize part - and jsut returns the raw value.
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Set;


            /**
             *  @todo - consider using independent atom registry
             */
            using   InstrumentNameType =  DataExchange::Atom<>;


            /**
             */
            using   CapturerCallback = Execution::Function<MeasurementSet()>;

            class   ICapturer {
            public:
                virtual ~ICapturer () {};
                virtual MeasurementSet  Capture () = 0;
                virtual unique_ptr<ICapturer>   Clone () const = 0;
            };

            class   xICapturer : public ICapturer {
            public:
                xICapturer (const CapturerCallback&  capturerCallback)
                    : fCapturerCallback (capturerCallback)
                {
                }
                virtual MeasurementSet  Capture ()
                {
                    return fCapturerCallback ();
                }
                virtual unique_ptr<ICapturer>   Clone () const override
                {
                    // make_unique<> only in C++14 - add bug defines for this
#if 1
                    return unique_ptr<ICapturer> (new xICapturer (fCapturerCallback));
#else
                    return make_unique<xICapturer> (fCapturerCallback);
#endif
                }
                CapturerCallback    fCapturerCallback;
            };

            /**
             *  \note   Design Note
             *          Each instrument instance MAY maintain 'state' - that affects subsequent calls. For example, for instruments
             *          that measure data over a period of time (like average CPU usage over a time interval) - they may maintain
             *          state, and return an average over the time since the last call to this instrument instance.
             */
            struct  Instrument {
                InstrumentNameType                  fInstrumentName;
                //CapturerCallback                    fCaptureFunction;
                Set<MeasurementType>                fCapturedMeasurements;
                DataExchange::ObjectVariantMapper   fObjectVariantMapper;


                struct SharedByValueCaptureRepType {
                    unique_ptr<ICapturer>   fCap_;
                    ICapturer*  get ()
                    {
                        return fCap_.get ();
                    }
                    const ICapturer*    get () const
                    {
                        return fCap_.get ();
                    }
                    SharedByValueCaptureRepType (unique_ptr<ICapturer>&& cap)
                        : fCap_ (move (cap))
                    {
                    }
                    SharedByValueCaptureRepType (const SharedByValueCaptureRepType& cap)
                        : fCap_ (move (cap.get ()->Clone ()))
                    {
                    }
                    SharedByValueCaptureRepType& operator= (const SharedByValueCaptureRepType& cap)
                    {
                        fCap_ = move (cap.get ()->Clone ());
                        return *this;
                    }
                };

                Instrument (InstrumentNameType instrumentName, const CapturerCallback& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper);
                Instrument (InstrumentNameType instrumentName, const SharedByValueCaptureRepType& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper);


                SharedByValueCaptureRepType    fCapFun_;

                /**
                 */
                nonvirtual  MeasurementSet  Capture ();


                /**
                 *  Require just one measurmenet
                 */
                template    <typename T>
                nonvirtual    T CaptureOneMeasurement (DateTimeRange* measurementTimeOut = nullptr);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Instrument.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Instrument_h_*/
