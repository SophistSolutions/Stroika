/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP::DataTypes {

    /*
     ********************************************************************************
     ********************** UPnP::DataTypes::DefaultDataType ************************
     ********************************************************************************
     */
    template <IValue T>
    constexpr DataType DefaultDataType ()
    {
        using namespace Foundation;
        if constexpr (same_as<T, bool>) {
            return DataType::eBoolean;
        }
        else if constexpr (same_as<T, uint8_t>) {
            return DataType::eUI1;
        }
        else if constexpr (same_as<T, uint16_t>) {
            return DataType::eUI2;
        }
        else if constexpr (same_as<T, uint32_t>) {
            return DataType::eUI4;
        }
        else if constexpr (same_as<T, uint64_t>) {
            return DataType::eUI8;
        }
        else if constexpr (same_as<T, int8_t>) {
            return DataType::eI1;
        }
        else if constexpr (same_as<T, int16_t>) {
            return DataType::eI2;
        }
        else if constexpr (same_as<T, int32_t>) {
            return DataType::eI4;
        }
        else if constexpr (same_as<T, int64_t>) {
            return DataType::eI8;
        }
        else if constexpr (same_as<T, float>) {
            return DataType::eR4;
        }
        else if constexpr (same_as<T, double>) {
            return DataType::eR8;
        }
        else if constexpr (same_as<T, Characters::Character>) {
            return DataType::eChar;
        }
        else if constexpr (same_as<T, String>) {
            return DataType::eString;
        }
        else if constexpr (same_as<T, Time::Date>) {
            return DataType::eDate;
        }
        else if constexpr (same_as<T, Time::DateTime>) {
            return DataType::eDateTimeTZ;
        }
        else if constexpr (same_as<T, Time::TimeOfDay>) {
            return DataType::eTime;
        }
        else if constexpr (same_as<T, Memory::BLOB>) {
            return DataType::eBinBase64;
        }
        else if constexpr (same_as<T, IO::Network::URI>) {
            return DataType::eURI;
        }
        else {
            static_assert (same_as<T, Common::GUID>);
            return DataType::eUUID;
        }
    }

    /*
     ********************************************************************************
     ************************* UPnP::DataTypes::IsTypeOf ****************************
     ********************************************************************************
     */
    template <IValue T>
    constexpr bool IsTypeOf (DataType t)
    {
        using namespace Foundation;
        switch (t) {
            case DataType::eUI1:
                return same_as<T, uint8_t>;
            case DataType::eUI2:
                return same_as<T, uint16_t>;
            case DataType::eUI4:
                return same_as<T, uint32_t>;
            case DataType::eUI8:
                return same_as<T, uint64_t>;
            case DataType::eI1:
                return same_as<T, int8_t>;
            case DataType::eI2:
                return same_as<T, int16_t>;
            case DataType::eI4:
            case DataType::eInt:
                return same_as<T, int32_t>;
            case DataType::eI8:
                return same_as<T, int64_t>;
            case DataType::eR4:
                return same_as<T, float>;
            case DataType::eR8:
            case DataType::eNumber:
            case DataType::eFixed14_4:
            case DataType::eFloat:
                return same_as<T, double>;
            case DataType::eChar:
                return same_as<T, Characters::Character>;
            case DataType::eString:
                return same_as<T, String>;
            case DataType::eDate:
                return same_as<T, Time::Date>;
            case DataType::eDateTime:
            case DataType::eDateTimeTZ:
                return same_as<T, Time::DateTime>;
            case DataType::eTime:
            case DataType::eTimeTZ:
                return same_as<T, Time::TimeOfDay>;
            case DataType::eBoolean:
                return same_as<T, bool>;
            case DataType::eBinBase64:
            case DataType::eBinHex:
                return same_as<T, Memory::BLOB>;
            case DataType::eURI:
                return same_as<T, IO::Network::URI>;
            case DataType::eUUID:
                return same_as<T, Common::GUID>;
        }
        return false;
    }

    /*
     ********************************************************************************
     ************************** UPnP::DataTypes::ToText *****************************
     ********************************************************************************
     */
    template <IValue T>
    inline String ToText (const T& v)
    {
        return ToText (Value{in_place_type<T>, v}, DefaultDataType<T> ());
    }

    /*
     ********************************************************************************
     ************************* UPnP::DataTypes::FromText ****************************
     ********************************************************************************
     */
    template <IValue T>
    inline optional<T> FromText (const String& text, DataType t)
    {
        Require (IsTypeOf<T> (t));
        if (optional<Value> v = FromText (text, t)) {
            return get<T> (*v);
        }
        return nullopt;
    }
    template <IValue T>
    inline optional<T> FromText (const String& text)
    {
        return FromText<T> (text, DefaultDataType<T> ());
    }

}

namespace Stroika::Frameworks::UPnP::DataTypes {
    constexpr Foundation::Common::EnumNameText EnumeratorName (DataType e, Foundation::Common::EnumNameText byRule)
    {
        switch (e) {
            case DataType::eUI1:
                return "ui1";
            case DataType::eUI2:
                return "ui2";
            case DataType::eUI4:
                return "ui4";
            case DataType::eUI8:
                return "ui8";
            case DataType::eI1:
                return "i1";
            case DataType::eI2:
                return "i2";
            case DataType::eI4:
                return "i4";
            case DataType::eI8:
                return "i8";
            case DataType::eInt:
                return "int";
            case DataType::eR4:
                return "r4";
            case DataType::eR8:
                return "r8";
            case DataType::eNumber:
                return "number";
            case DataType::eFixed14_4:
                return "fixed.14.4";
            case DataType::eFloat:
                return "float";
            case DataType::eChar:
                return "char";
            case DataType::eString:
                return "string";
            case DataType::eDate:
                return "date";
            case DataType::eDateTime:
                return "dateTime";
            case DataType::eDateTimeTZ:
                return "dateTime.tz";
            case DataType::eTime:
                return "time";
            case DataType::eTimeTZ:
                return "time.tz";
            case DataType::eBoolean:
                return "boolean";
            case DataType::eBinBase64:
                return "bin.base64";
            case DataType::eBinHex:
                return "bin.hex";
            case DataType::eURI:
                return "uri";
            case DataType::eUUID:
                return "uuid";
            default:
                return byRule;
        }
    }
}
