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

#include "../../Foundation/Common/Property.h"
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
         *  This is a base type for capture contexts. Actual contexts will contain more information
         *  relevant to that Instrument (but all private). All users of Instruments can control is, when
         *  copying an instrument, which ones shared context.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
         *          More importantly, this is a requirement on all subclasses of the capture context
         */
        class ICaptureContext {
        public:
            virtual ~ICaptureContext () = default;
        };

    public:
        /**
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         */
        class ICapturer {
        public:
            virtual ~ICapturer ()                                                                      = default;
            virtual MeasurementSet              Capture ()                                             = 0;
            virtual unique_ptr<ICapturer>       Clone () const                                         = 0;
            virtual shared_ptr<ICaptureContext> GetConext () const                                     = 0;
            virtual void                        SetConext (const shared_ptr<ICaptureContext>& context) = 0;
        };

    public:
        /**
         */
        Instrument () = delete;
        Instrument (const Instrument& src);
        Instrument (InstrumentNameType instrumentName, unique_ptr<ICapturer>&& capturer, const Set<MeasurementType>& capturedMeasurements, const Mapping<type_index, MeasurementType>& typeToMeasurementTypeMap, const DataExchange::ObjectVariantMapper& objectVariantMapper);

    public:
        nonvirtual Instrument& operator= (const Instrument& rhs);

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
        /**
         *  When you copy an Instrument, by default it remains linked to its source for some of its shared (averaging) data.
         *  to break that context link, set this context to nullptr, and it automatically creates a new context.
         *  Get() here - always returns non-null. But - its an opaque type, so not terribly useful, except to assign nullptr.
         *  It is ILLEGAL (Required) that any arguments ASSIGNED to the pContext be either nullptr or of the exact dynamic
         *  type used by the given Instrument (so copied from an earlier version of this instrument or something known to be
         *  of the same type).
         */
        Common::Property<shared_ptr<ICaptureContext>> pContext;

    public:
        /**
         * @brief  Instruments all have a displayable name which can be used for reference. Each (type) instrument should have a unique.
         */
        Common::ReadOnlyProperty<InstrumentNameType> pInstrumentName;

    public:
        /**
         */
        Common::ReadOnlyProperty<Set<MeasurementType>> pCapturedMeasurementTypes;

    public:
        /**
         */
        Common::ReadOnlyProperty<DataExchange::ObjectVariantMapper> pObjectVariantMapper;

    public:
        /**
         * @brief Many (all) instruments will have corresponding c++ objects to represent what is captuerd. Typically
         *        this entry will have only one entry, but multiple types could be supported. This MAY not represent all
         *        measurement types supported by this instrument (that is some may not have a C++ struct equivilent)
         */
        Common::ReadOnlyProperty<Mapping<type_index, MeasurementType>> pType2MeasurementTypes;

    private:
        /*const*/ InstrumentNameType fInstrumentName;

    private:
        /*const*/ Mapping<type_index, MeasurementType> fType2MeasurementTypes;

    private:
        /*const*/ Set<MeasurementType> fCapturedMeasurementTypes;

    private:
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
