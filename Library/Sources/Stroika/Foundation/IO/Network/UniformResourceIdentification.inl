/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_
#define _Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/Optional.h"

namespace Stroika::Foundation::IO::Network::UniformResourceIdentification {

    /*
     ********************************************************************************
     **************************************** Host **********************************
     ********************************************************************************
     */
    inline Host::Host (const String& registeredName)
        : fEncodedName_{EncodeAsRawURL_ (registeredName)}
        , fRegisteredName_{registeredName}
    {
    }
    inline Host::Host (const InternetAddress& addr)
        : fEncodedName_{EncodeAsRawURL_ (addr)}
        , fInternetAddress_{addr}
    {
    }
    inline Host Host::Parse (const String& rawURLHostnameText)
    {
        pair<optional<String>, optional<InternetAddress>> tmp{ParseRaw_ (rawURLHostnameText)};
        Host                                              h;
        h.fEncodedName_     = rawURLHostnameText;
        h.fRegisteredName_  = tmp.first;
        h.fInternetAddress_ = tmp.second;
        return h;
    }
    inline optional<String> Host::AsRegisteredName () const
    {
        return fRegisteredName_;
    }
    inline optional<InternetAddress> Host::AsInternetAddress () const
    {
        return fInternetAddress_;
    }
    inline String Host::AsEncoded () const
    {
        return fEncodedName_;
    }

    /*
     ********************************************************************************
     *************************************** Host operators *************************
     ********************************************************************************
     */
    inline bool operator== (const Host& lhs, const Host& rhs)
    {
        return lhs.AsEncoded ().Equals (rhs.AsEncoded (), Characters::CompareOptions::eCaseInsensitive);
    }
    inline bool operator!= (const Host& lhs, const Host& rhs)
    {
        return not lhs.AsEncoded ().Equals (rhs.AsEncoded (), Characters::CompareOptions::eCaseInsensitive);
    }

    /*
     ********************************************************************************
     ********************************* Authority ************************************
     ********************************************************************************
     */
    inline Authority::Authority (const optional<Host>& h, optional<PortType> port, optional<String> userInfo)
        : fHost{h}
        , fPort{port}
        , fUserInfo (userInfo)
    {
    }

    /*
     ********************************************************************************
     ************************************ Query *************************************
     ********************************************************************************
     */
    inline const Containers::Mapping<String, String>& Query::GetMap () const
    {
        return fMap_;
    }
    inline String Query::operator() (const String& idx) const
    {
        return Memory::ValueOrDefault (fMap_.Lookup (idx));
    }
    inline String Query::operator() (const string& idx) const
    {
        return operator() (Characters::UTF8StringToWide (idx));
    }
    inline bool Query::HasField (const String& idx) const
    {
        return fMap_.ContainsKey (idx);
    }
    inline bool Query::HasField (const string& idx) const
    {
        return HasField (Characters::UTF8StringToWide (idx));
    }
    inline void Query::AddField (const String& idx, const String& value)
    {
        fMap_.Add (idx, value);
    }
    inline void Query::RemoveFieldIfAny (const string& idx)
    {
        RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
    }

}

#endif /*_Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_*/
