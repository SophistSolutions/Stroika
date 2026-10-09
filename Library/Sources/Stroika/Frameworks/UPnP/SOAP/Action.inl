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
    inline Argument::Argument (const String& name, const String& text)
        : Foundation::Common::KeyValuePair<String, String>{name, text}
    {
    }
    inline Argument::Argument (const Foundation::Common::KeyValuePair<String, String>& nameAndText)
        : Foundation::Common::KeyValuePair<String, String>{nameAndText}
    {
    }

    namespace Private_ {
        template <DataTypes::IValue T>
        optional<T> LookupArgument_ (const Arguments& arguments, const String& name)
        {
            for (const Argument& a : arguments) {
                if (a.fKey == name) {
                    return DataTypes::FromText<T> (a.fValue);
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
        return Private_::LookupArgument_<T> (fArguments, name);
    }

    /*
     ********************************************************************************
     ************************* UPnP::SOAP::ActionResponse ***************************
     ********************************************************************************
     */
    template <DataTypes::IValue T>
    inline optional<T> ActionResponse::LookupArgument (const String& name) const
    {
        return Private_::LookupArgument_<T> (fArguments, name);
    }

}
