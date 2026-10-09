/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DataTypes_h_
#define _Stroika_Frameworks_UPnP_DataTypes_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdint>
#include <optional>
#include <variant>

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/TimeOfDay.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

/**
 *  UPnP's data types (UPnP Device Architecture 1.1, section 2.5): each a state variable's dataType, and so the type of an action's
 *  argument and an event's value - always sent as text, with no mark of its type: the reader knows it from the service's
 *  description, or its code. ToText writes a value as UPnP does, and FromText reads one back, as UPnP says to.
 *
 *  Each data type has its C++ type; each C++ type, its default data type (marked * in the table) - so where code knows its
 *  values' types, ToText (v) and FromText<T> (text) need no data type; where a value is of another data type than its C++ type's
 *  default - a BLOB as bin.hex, a double as fixed.14.4 - or code learns a value's type only as it runs, from a service's
 *  description, say, the data type is given: ToText (v, DataType::eBinHex), FromText (text, dataType) - a Value, then.
 *
 *      data type (DataType)                C++ type                    written as
 *      boolean (eBoolean) *                bool                        1 or 0 (read: also true/false, yes/no - any case)
 *      ui1, ui2, ui4, ui8 (eUI1 ...) *     uint8_t ... uint64_t        decimal (read: a leading + and zeros allowed; out of range: none)
 *      i1, i2, i4, i8 (eI1 ...) *          int8_t ... int64_t          decimal, - if negative
 *      int (eInt)                          int32_t                     as i4
 *      r4 (eR4) *                          float                       decimal - the fewest digits that read back exactly - E before an
 *      r8 *, number, float (eR8 ...)       double                          exponent (no NaN or infinity: UPnP has none)
 *      fixed.14.4 (eFixed14_4)             double                      decimal, no more than 4 digits after the point (nor 14 before)
 *      char (eChar) *                      Characters::Character       itself
 *      string (eString) *                  String                      itself (read: as is - not trimmed)
 *      date (eDate) *                      Time::Date                  ISO 8601: 2026-10-08
 *      dateTime (eDateTime)                Time::DateTime              ISO 8601, without its time zone: 2026-10-08T14:30:05
 *      dateTime.tz (eDateTimeTZ) *         Time::DateTime              ISO 8601, with its time zone if it has one
 *      time (eTime) *                      Time::TimeOfDay             ISO 8601: 14:30:05
 *      time.tz (eTimeTZ)                   Time::TimeOfDay             as time (read: a time zone after it ignored - TimeOfDay has none)
 *      bin.base64 (eBinBase64) *           Memory::BLOB                base64, on one line
 *      bin.hex (eBinHex)                   Memory::BLOB                hexadecimal, lower case
 *      uri (eURI) *                        IO::Network::URI            itself
 *      uuid (eUUID) *                      Common::GUID                8-4-4-4-12 hexadecimal digits, no braces
 *
 *  Each is read with spaces round it ignored - but a string, or a char, as it is.
 */
namespace Stroika::Frameworks::UPnP::DataTypes {

    using Foundation::Characters::String;

    /**
     *  \brief UPnP's data types - each named, as a service's description names it (Common::DefaultNames): ui2, fixed.14.4, ...
     */
    enum class DataType : uint8_t {
        eUI1,
        eUI2,
        eUI4,
        eUI8,
        eI1,
        eI2,
        eI4,
        eI8,
        eInt,
        eR4,
        eR8,
        eNumber,
        eFixed14_4,
        eFloat,
        eChar,
        eString,
        eDate,
        eDateTime,
        eDateTimeTZ,
        eTime,
        eTimeTZ,
        eBoolean,
        eBinBase64,
        eBinHex,
        eURI,
        eUUID,

        Stroika_Define_Enum_Bounds (eUI1, eUUID)
    };

    /**
     *  \brief A value of any of UPnP's data types, as its C++ type (the table above) - for code that learns a value's type as it
     *         runs
     *
     *  \note Not DataExchange::VariantValue: it has no type for a time of day, a URI, a uuid or a char, and just one each for
     *        integers, unsigned integers and floats - so a ui1 and a ui4, an r4 and an r8, would be the same to it. Here each has
     *        the C++ type of its own size and kind. (Data types sharing a C++ type - r8 and fixed.14.4, say - are told apart by the
     *        DataType given with the value.)
     */
    using Value = variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double,
                          Foundation::Characters::Character, String, Foundation::Time::Date, Foundation::Time::DateTime,
                          Foundation::Time::TimeOfDay, Foundation::Memory::BLOB, Foundation::IO::Network::URI, Foundation::Common::GUID>;

    /**
     *  \brief The C++ types of UPnP's data types: Value's
     */
    template <typename T>
    concept IValue =
        Foundation::Common::IAnyOf<T, bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double,
                                   Foundation::Characters::Character, String, Foundation::Time::Date, Foundation::Time::DateTime,
                                   Foundation::Time::TimeOfDay, Foundation::Memory::BLOB, Foundation::IO::Network::URI, Foundation::Common::GUID>;

    /**
     *  \brief The data type a T is written as, given none: its default (marked * in the table above)
     */
    template <IValue T>
    constexpr DataType DefaultDataType ();

    /**
     *  \brief Whether T is t's C++ type (the table above)
     */
    template <IValue T>
    constexpr bool IsTypeOf (DataType t);

    /**
     *  \brief v, as UPnP writes it: as t, if given, else as its C++ type's default data type (the table above)
     *
     *  \pre t, if given, is a data type whose C++ type is v's (IsTypeOf)
     *  \pre a float or double: finite - UPnP has no NaN or infinity; as fixed.14.4, less than 10^14 from 0
     */
    template <IValue T>
    String ToText (const T& v);
    String ToText (const Value& v);             ///< \brief v, as UPnP writes it: as t, if given, else as its C++ type's default data type
    String ToText (const Value& v, DataType t); ///< \brief v, as UPnP writes it: as t, if given, else as its C++ type's default data type

    /**
     *  \brief text read as UPnP reads it: as t, if given, else as T's default data type (the table above) - nullopt if it is not
     *         one: not a number, say, or a number out of its type's range. Given only t, a Value of t's C++ type.
     *
     *  \pre t, if given with T, is a data type whose C++ type is T (IsTypeOf)
     */
    template <IValue T>
    optional<T> FromText (const String& text);
    template <IValue T>
    optional<T> FromText (const String& text, DataType t); ///< \brief text read as UPnP reads it: as t, if given, else as T's default data type
    optional<Value> FromText (const String& text, DataType t); ///< \brief text read as UPnP reads it: as t, if given, else as T's default data type

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DataTypes.inl"

#endif /*_Stroika_Frameworks_UPnP_DataTypes_h_*/
