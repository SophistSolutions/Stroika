/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instrument_h_
#define _Stroika_Framework_SystemPerformance_Instrument_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include <memory>

#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/DataExchange/Atom.h"
#include "../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../Foundation/Execution/Function.h"

#include "Measurement.h"
#include "MeasurementSet.h"

/*
 * TODO:
 *      @todo   Code cleanups
 *
 *      @todo   Document and enforce (no) thread safety polciy (assert externally locked?).
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Set;

    /**
     *  @todo - consider using independent atom registry
     */
    using InstrumentNameType = DataExchange::Atom<>;

    /**
     *  \brief An Instrument is a stateful object from which you can Capture () a series of measurements about a system.
     *
     *  \note   Design Note
     *          Each instrument instance MAY maintain 'state' - that affects subsequent calls. For example, for instruments
     *          that measure data over a period of time (like average CPU usage over a time interval) - they may maintain
     *          state, and return an average over the time since the last call to this instrument instance.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    struct Instrument {

        /**
         */
        class ICapturer {
        public:
            virtual ~ICapturer ()                        = default;
            virtual MeasurementSet        Capture ()     = 0;
            virtual unique_ptr<ICapturer> Clone () const = 0;
        };

        /**
         *  @todo CLEANUP NAMES AND IMPL
         */
        struct SharedByValueCaptureRepType {
            unique_ptr<ICapturer> fCap_;
            ICapturer*            get ()
            {
                return fCap_.get ();
            }
            const ICapturer* get () const
            {
                return fCap_.get ();
            }
            SharedByValueCaptureRepType (unique_ptr<ICapturer>&& cap)
                : fCap_ (move (cap))
            {
            }
            SharedByValueCaptureRepType (const SharedByValueCaptureRepType& cap)
                : fCap_ (cap.get ()->Clone ())
            {
            }
            SharedByValueCaptureRepType& operator= (const SharedByValueCaptureRepType& cap)
            {
                fCap_ = cap.get ()->Clone ();
                return *this;
            }
        };

        InstrumentNameType                fInstrumentName;
        SharedByValueCaptureRepType       fCapFun_;
        Set<MeasurementType>              fCapturedMeasurements;
        DataExchange::ObjectVariantMapper fObjectVariantMapper;

        /**
         */
        Instrument (InstrumentNameType instrumentName, const SharedByValueCaptureRepType& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper);

        /**
         */
        nonvirtual MeasurementSet Capture ();

        /**
         *  Require just one measurmenet
         */
        template <typename T>
        nonvirtual T CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut = nullptr);

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Instrument& rhs) const
        {
            return fInstrumentName <=> rhs.fInstrumentName;
        }

    public:
        /**
         */
        nonvirtual bool operator== (const Instrument& rhs) const
        {
            return fInstrumentName == rhs.fInstrumentName;
        }
#endif
#if __cpp_impl_three_way_comparison < 201907
        bool operator== (const Instrument& rhs) const
        {
            return fInstrumentName == rhs.fInstrumentName;
        }
        bool operator< (const Instrument& rhs) const
        {
            return fInstrumentName < rhs.fInstrumentName;
        }
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Instrument.inl"

#endif /*_Stroika_Framework_SystemPerformance_Instrument_h_*/
