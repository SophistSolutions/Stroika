/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_inl_
#define _Stroika_Foundation_IO_Network_URL_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/Optional.h"

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ************************************ URLQuery **********************************
     ********************************************************************************
     */
    inline const Containers::Mapping<String, String>& URLQuery::GetMap () const
    {
        return fMap_;
    }
    inline String URLQuery::operator() (const String& idx) const
    {
        return Memory::ValueOrDefault (fMap_.Lookup (idx));
    }
    inline String URLQuery::operator() (const string& idx) const
    {
        return operator() (Characters::UTF8StringToWide (idx));
    }
    inline bool URLQuery::HasField (const String& idx) const
    {
        return fMap_.ContainsKey (idx);
    }
    inline bool URLQuery::HasField (const string& idx) const
    {
        return HasField (Characters::UTF8StringToWide (idx));
    }
    inline void URLQuery::AddField (const String& idx, const String& value)
    {
        fMap_.Add (idx, value);
    }
    inline void URLQuery::RemoveFieldIfAny (const string& idx)
    {
        RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
    }

    /*
     ********************************************************************************
     **************************** URL::Authority::Host ******************************
     ********************************************************************************
     */
    inline URL::Authority::Host::Host (const String& registeredName)
        : fEncodedName_{EncodeAsRawURL_ (registeredName)}
        , fRegisteredName_{registeredName}
    {
    }
    inline URL::Authority::Host::Host (const InternetAddress& addr)
        : fEncodedName_{EncodeAsRawURL_ (addr)}
        , fInternetAddress_{addr}
    {
    }
    inline URL::Authority::Host URL::Authority::Host::Parse (const String& rawURLHostnameText)
    {
        pair<optional<String>, optional<InternetAddress>> tmp{ParseRaw_ (rawURLHostnameText)};
        URL::Authority::Host                              h;
        h.fEncodedName_     = rawURLHostnameText;
        h.fRegisteredName_  = tmp.first;
        h.fInternetAddress_ = tmp.second;
        return h;
    }
    inline optional<String> URL::Authority::Host::AsRegisteredName () const
    {
        return fRegisteredName_;
    }
    inline optional<InternetAddress> URL::Authority::Host::AsInternetAddress () const
    {
        return fInternetAddress_;
    }
    inline String URL::Authority::Host::AsEncoded () const
    {
        return fEncodedName_;
    }

    /*
     ********************************************************************************
     **************************** URL::Authority ************************************
     ********************************************************************************
     */
    inline URL::Authority::Authority (const optional<Host>& h, optional<PortType> port, optional<String> userInfo)
        : fHost{h}
        , fPort{port}
        , fUserInfo (userInfo)

    {
    }

    /*
     ********************************************************************************
     *************************************** URL ************************************
     ********************************************************************************
     */
    inline URL::PortType URL::GetPortValue (PortType defaultValue) const
    {
        if (fAuthority_.fPort) {
            return *fAuthority_.fPort;
        }
        else {
            return GetDefaultPortForScheme (GetScheme ()).value_or (defaultValue);
        }
    }
    inline optional<URL::PortType> URL::GetPortNumber () const
    {
        return fAuthority_.fPort;
    }
    inline void URL::SetPortNumber (const optional<PortType>& portNum)
    {
        fAuthority_.fPort = portNum;
    }
    inline optional<URL::SchemeType> URL::GetScheme () const
    {
        return fScheme_;
    }
    inline String URL::GetHost () const
    {
        if (fAuthority_.fHost) {
            return fAuthority_.fHost->AsEncoded ();
        }
        else {
            return String{};
        }
    }
    inline void URL::SetHost (const String& host)
    {
        if (host.empty ()) {
            fAuthority_.fHost = nullopt;
        }
        else {
            fAuthority_.fHost = host;
        }
    }
    inline void URL::SetScheme (const optional<SchemeType>& scheme)
    {
        if (scheme) {
            SetScheme (*scheme); //validates value
        }
        else {
            fScheme_ = nullopt;
        }
    }
    inline String URL::GetHostRelativePath () const
    {
        return fRelPath_;
    }
    inline void URL::SetHostRelativePath (const String& hostRelativePath)
    {
        fRelPath_ = hostRelativePath;
    }
    inline URLQuery URL::GetQuery () const
    {
        return URLQuery (fQuery_);
    }
    inline void URL::SetQuery (const URLQuery& query)
    {
        fQuery_ = query.ComputeQueryString ();
    }
    inline String URL::GetQueryString () const
    {
        Ensure (not fQuery_.StartsWith (L"?"));
        return fQuery_;
    }
    inline void URL::SetQueryString (const String& queryString)
    {
        Require (not queryString.StartsWith (L"?")); // should validate that it parses
        fQuery_ = queryString;
    }
    inline String URL::GetFragment () const
    {
        return fFragment_;
    }
    inline void URL::SetFragment (const String& frag)
    {
        fFragment_ = frag;
    }

    /*
     ********************************************************************************
     *************************************** URI ************************************
     ********************************************************************************
     */
    inline URI::URI (const optional<SchemeType>& scheme, const optional<Authority>& authority, const String& path, const optional<String>& query, const optional<String>& fragment)
        : fScheme_{scheme}
        , fAuthority_{authority}
        , fPath_{path}
        , fQuery_{query}
        , fFragment_{fragment}
    {
    }
    inline URI::URI (const URL& url)
        : fScheme_{url.GetScheme ()}
        , fAuthority_{url.GetAuthority ()}
        , fPath_{url.GetHostRelativePath ()}
        , fQuery_{url.GetQueryString ()}
        , fFragment_{url.GetFragment ()}
    {
    }
    inline bool URI::IsURL () const
    {
        return false; // check key fields present?
    }
    inline optional<URI::SchemeType> URI::GetScheme () const
    {
        return fScheme_;
    }
    inline optional<URI::Authority> URI::GetAuthority () const
    {
        return fAuthority_;
    }
    inline String URI::GetPath () const
    {
        return fPath_;
    }
    inline optional<String> URI::GetQueryString () const
    {
        return fQuery_;
    }

    /*
     ********************************************************************************
     ******************* URL::Authority::Host operators *****************************
     ********************************************************************************
     */
    inline bool operator== (const URL::Authority::Host& lhs, const URL::Authority::Host& rhs)
    {
        return lhs.fEncodedName_.Equals (rhs.fEncodedName_, Characters::CompareOptions::eCaseInsensitive);
    }
    inline bool operator!= (const URL::Authority::Host& lhs, const URL::Authority::Host& rhs)
    {
        return not lhs.fEncodedName_.Equals (rhs.fEncodedName_, Characters::CompareOptions::eCaseInsensitive);
    }

    /*
     ********************************************************************************
     ****************************** URL operators ***********************************
     ********************************************************************************
     */
    inline bool operator< (const URL& lhs, const URL& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    inline bool operator<= (const URL& lhs, const URL& rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    inline bool operator== (const URL& lhs, const URL& rhs)
    {
        return lhs.Equals (rhs);
    }
    inline bool operator!= (const URL& lhs, const URL& rhs)
    {
        return not lhs.Equals (rhs);
    }
    inline bool operator> (const URL& lhs, const URL& rhs)
    {
        return lhs.Compare (rhs) > 0;
    }
    inline bool operator>= (const URL& lhs, const URL& rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }

    /*
     ********************************************************************************
     ******************************** LabeledURL ************************************
     ********************************************************************************
     */
    inline LabeledURL::LabeledURL (const URL& url, const String& label)
        : fURL (url)
        , fLabel (label)
    {
    }

}

#endif /*_Stroika_Foundation_IO_Network_URL_inl_*/
