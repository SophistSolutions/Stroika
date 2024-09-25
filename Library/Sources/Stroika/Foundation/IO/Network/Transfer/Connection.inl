/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::IO::Network::Transfer {

    /*
     ********************************************************************************
     *********************** Connection::Options::Authentication ********************
     ********************************************************************************
     */
    inline Connection::Options::Authentication::Authentication (const String& authToken)
        : fOptions_{Options::eProactivelySendAuthentication}
        , fExplicitAuthToken_{authToken}
    {
    }
    inline Connection::Options::Authentication::Authentication (const String& username, const String& password, Options options)
        : fOptions_{options}
        , fUsernamePassword_{pair<String, String>{username, password}}
    {
    }
    inline Connection::Options::Authentication::Options Connection::Options::Authentication::GetOptions () const
    {
        return fOptions_;
    }
    inline optional<pair<String, String>> Connection::Options::Authentication::GetUsernameAndPassword () const
    {
        return fUsernamePassword_;
    }

    /*
     ********************************************************************************
     ******************************* Connection::Ptr ********************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
        RequireNotNull (rep);
    }
    inline Connection::Options Connection::Ptr::GetOptions () const
    {
        return fRep_->GetOptions ();
    }
    inline URI Connection::Ptr::GetSchemeAndAuthority () const
    {
        Ensure (fRep_->GetSchemeAndAuthority ().GetSchemeAndAuthority () == fRep_->GetSchemeAndAuthority ());
        return fRep_->GetSchemeAndAuthority ();
    }
    inline void Connection::Ptr::SetSchemeAndAuthority (const URI& url)
    {
        Require (url.GetSchemeAndAuthority () == url);
        fRep_->SetSchemeAndAuthority (url);
    }
    inline void Connection::Ptr::Close ()
    {
        fRep_->Close ();
    }
    inline Time::DurationSeconds Connection::Ptr::GetTimeout () const
    {
        return fRep_->GetTimeout ();
    }
    inline void Connection::Ptr::SetTimeout (Time::DurationSeconds timeout)
    {
        fRep_->SetTimeout (timeout);
    }

    /*
     ********************************************************************************
     ********************************** Connection **********************************
     ********************************************************************************
     */
    inline Connection::Ptr Connection::New ()
    {
        return New ({});
    }

}

/*
 ********************************************************************************
 *********************************** Common::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Common {

    template <>
    constexpr EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>
        DefaultNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::k{{{
            {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eProactivelySendAuthentication,
             L"Proactively-Send-Authenitcation"},
            {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eRespondToWWWAuthenticate,
             L"Respond-To-WWW-Authenticate"},
        }}};

}
