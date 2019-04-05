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
     *************************************** URL ************************************
     ********************************************************************************
     */
    inline URL::PortType URL::GetPortValue (PortType defaultValue) const
    {
        if (fAuthority_.fPort) {
            return *fAuthority_.fPort;
        }
        else {
            return UniformResourceIdentification::GetDefaultPortForScheme (GetScheme ()).value_or (defaultValue);
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
    inline UniformResourceIdentification::Query URL::GetQuery () const
    {
        return Query (fQuery_);
    }
    inline void URL::SetQuery (const UniformResourceIdentification::Query& query)
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
