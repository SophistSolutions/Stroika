/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DataTypes_h_
#define _Stroika_Frameworks_UPnP_DataTypes_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdint>
#include <optional>

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Concepts.h"
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
 *  description, or its code. ToText writes a C++ value as UPnP does, and FromText reads one back, as UPnP says to.
 *
 *      UPnP type                       C++ type                        written as
 *      boolean                         bool                            1 or 0 (read: also true/false, yes/no - any case)
 *      ui1, ui2, ui4, ui8              uint8_t ... uint64_t            decimal (read: a leading + and zeros allowed; out of range: none)
 *      i1, i2, i4, i8, int             int8_t ... int64_t              decimal, - if negative
 *      r4                              float                           decimal - the fewest digits that read back exactly - E before
 *      r8, number, float, fixed.14.4   double                              an exponent (no NaN or infinity: UPnP has none)
 *      char                            Characters::Character           itself
 *      string                          String                          itself (read: as is - not trimmed)
 *      date                            Time::Date                      ISO 8601: 2026-10-08
 *      dateTime, dateTime.tz           Time::DateTime                  ISO 8601, with its time zone if it has one
 *      time, time.tz                   Time::TimeOfDay                 ISO 8601: 14:30:00 (a time.tz's zone is not kept)
 *      bin.base64                      Memory::BLOB                    base64, on one line
 *      bin.hex                         BinHex                          hexadecimal
 *      uri                             IO::Network::URI                itself
 *      uuid                            Common::GUID                    8-4-4-4-12 hexadecimal digits, no braces
 *
 *  Each is read with spaces round it ignored - but a string, or a char, as it is.
 */
namespace Stroika::Frameworks::UPnP::DataTypes {

    using Foundation::Characters::String;

    /**
     *  \brief A bin.hex value: its bytes, written in hexadecimal - where a BLOB alone is bin.base64
     */
    struct BinHex {
        Foundation::Memory::BLOB fBytes;

        bool operator== (const BinHex&) const = default;
    };

    /**
     *  \brief The C++ types of UPnP's data types: what ToText writes, and FromText reads
     */
    template <typename T>
    concept IValue =
        Foundation::Common::IAnyOf<T, bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double,
                                   Foundation::Characters::Character, String, Foundation::Time::Date, Foundation::Time::DateTime, Foundation::Time::TimeOfDay,
                                   Foundation::Memory::BLOB, BinHex, Foundation::IO::Network::URI, Foundation::Common::GUID>;

    /**
     *  \brief v, as UPnP writes it (see the table above)
     *
     *  \pre a float or double: finite - UPnP has no NaN or infinity
     */
    template <IValue T>
    String ToText (const T& v);

    /**
     *  \brief text read as a T, as UPnP reads it (see the table above) - nullopt if it is not one: not a number, say, or a number
     *         out of T's range
     */
    template <IValue T>
    optional<T> FromText (const String& text);

}

#endif /*_Stroika_Frameworks_UPnP_DataTypes_h_*/
