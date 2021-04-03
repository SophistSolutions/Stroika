/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instrument_h_
#define _Stroika_Framework_SystemPerformance_Instrument_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include <memory>

#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/DataExchange/Atom.h"
#include "../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../Foundation/Execution/Function.h"

#include "Measurement.h"
#include "MeasurementSet.h"

/*
 * TODO:
 *      @todo   Figure out how to get const working for data members (or hide data members and replace with cosnt properties or accssor functions)
 *
 *      @todo   enforce thread safety polciy (assert externally locked?).
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Mapping;
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
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Instrument {
    public:
        /**
         */
        class ICapturer {
        public:
            virtual ~ICapturer ()                        = default;
            virtual MeasurementSet        Capture ()     = 0;
            virtual unique_ptr<ICapturer> Clone () const = 0;
        };

    public:
        /**
         */
        Instrument ()                      = delete;
        Instrument (const Instrument& src) = default;
        Instrument (InstrumentNameType instrumentName, unique_ptr<ICapturer>&& capturer, const Set<MeasurementType>& capturedMeasurements, const Mapping<type_index, MeasurementType>& typeToMeasurementTypeMap, const DataExchange::ObjectVariantMapper& objectVariantMapper);

    public:
        nonvirtual Instrument& operator= (const Instrument& rhs) = default;

    public:
        /**
         *  This can return multiple measurements, of different types, but typically will return a single measurement (with multiple name/value pairs).
         */
        nonvirtual MeasurementSet Capture ();

    public:
        /**
         *  Require just one measurement? This can be simpler, but generally better to use Capturer class, at least if
         *  doing a sequence of measurements (so they come in regularly spaced time intervals).
         */
        template <typename T>
        nonvirtual T CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut = nullptr);

    public:
        /**
         *  The particular types you can convert a measurement to are defined by each subtype of instrument. Typically they are
         *  that instruments "Info" field, but sometimes a given Instrument will generate multiple MeasurementTypes, and will allow
         *  additional conversions.
         * 
         *  If the required MeasurementType is missing from m, nullopt is returned.
         */
        template <typename T>
        nonvirtual T MeasurementAs (const Measurement& m) const;
        template <typename T>
        nonvirtual optional<T> MeasurementAs (const MeasurementSet& m) const;

    public:
        /*const*/ InstrumentNameType fInstrumentName;

    public:
        /*const*/ Mapping<type_index, MeasurementType> fType2MeasurementTypes;

    public:
        /*const*/ Set<MeasurementType> fCapturedMeasurementTypes;

    public:
        /*const*/ DataExchange::ObjectVariantMapper fObjectVariantMapper;

    public:
        /**
         */
        nonvirtual bool operator== (const Instrument& rhs) const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Instrument& rhs) const
        {
            return fInstrumentName <=> rhs.fInstrumentName;
        }

#endif
#if __cpp_impl_three_way_comparison < 201907
    public:
        bool operator< (const Instrument& rhs) const
        {
            return fInstrumentName < rhs.fInstrumentName;
        }
#endif

    private:
        struct SharedByValueCaptureRepType_ {
            unique_ptr<ICapturer> fCap_;
            ICapturer*            rwget ()
            {
                return fCap_.get ();
            }
            SharedByValueCaptureRepType_ (unique_ptr<ICapturer>&& cap)
                : fCap_{move (cap)}
            {
            }
            SharedByValueCaptureRepType_ (const SharedByValueCaptureRepType_& cap)
                : fCap_{cap.fCap_.get ()->Clone ()}
            {
            }
            SharedByValueCaptureRepType_& operator= (const SharedByValueCaptureRepType_& cap)
            {
                fCap_ = cap.fCap_.get ()->Clone ();
                return *this;
            }
        };
        /*const*/ SharedByValueCaptureRepType_ fCapFun_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Instrument.inl"

#endif /*_Stroika_Framework_SystemPerformance_Instrument_h_*/
