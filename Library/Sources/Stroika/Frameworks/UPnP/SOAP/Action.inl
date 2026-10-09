/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP::SOAP {

    /*
     ********************************************************************************
     ***************************** UPnP::SOAP::Argument *****************************
     ********************************************************************************
     */
    template <DataTypes::IValue T>
    inline Argument::Argument (const String& name, const T& value)
        : Foundation::Common::KeyValuePair<String, String>{name, DataTypes::ToText (value)}
    {
    }
    inline Argument::Argument (const String& name, const DataTypes::Value& value, DataTypes::DataType dataType)
        : Foundation::Common::KeyValuePair<String, String>{name, DataTypes::ToText (value, dataType)}
    {
    }
    inline Argument::Argument (const String& name, const String& text)
        : Foundation::Common::KeyValuePair<String, String>{name, text}
    {
    }
    inline Argument::Argument (const Foundation::Common::KeyValuePair<String, String>& nameAndText)
        : Foundation::Common::KeyValuePair<String, String>{nameAndText}
    {
    }

    namespace Private_ {
        // the text of the argument called name; nullopt if none is
        inline optional<String> LookupText_ (const Arguments& arguments, const String& name)
        {
            for (const Argument& a : arguments) {
                if (a.fKey == name) {
                    return a.fValue;
                }
            }
            return nullopt;
        }
    }

    /*
     ********************************************************************************
     ************************** UPnP::SOAP::ActionRequest ***************************
     ********************************************************************************
     */
    template <DataTypes::IValue T>
    inline optional<T> ActionRequest::LookupArgument (const String& name) const
    {
        return LookupArgument<T> (name, DataTypes::DefaultDataType<T> ());
    }
    template <DataTypes::IValue T>
    inline optional<T> ActionRequest::LookupArgument (const String& name, DataTypes::DataType dataType) const
    {
        optional<String> text = Private_::LookupText_ (fArguments, name);
        return text ? DataTypes::FromText<T> (*text, dataType) : nullopt;
    }

    /*
     ********************************************************************************
     ************************* UPnP::SOAP::ActionResponse ***************************
     ********************************************************************************
     */
    template <DataTypes::IValue T>
    inline optional<T> ActionResponse::LookupArgument (const String& name) const
    {
        return LookupArgument<T> (name, DataTypes::DefaultDataType<T> ());
    }
    template <DataTypes::IValue T>
    inline optional<T> ActionResponse::LookupArgument (const String& name, DataTypes::DataType dataType) const
    {
        optional<String> text = Private_::LookupText_ (fArguments, name);
        return text ? DataTypes::FromText<T> (*text, dataType) : nullopt;
    }

}
