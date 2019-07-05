/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../Characters/ToString.h"

namespace Stroika::Foundation::IO::Network::Transfer {

    /*
     ********************************************************************************
     *********************** Connection::Options::Authentication ********************
     ********************************************************************************
     */
    inline Connection::Options::Authentication::Authentication (const String& authToken)
        : fOptions_ (Options::eProactivelySendAuthentication)
        , fExplicitAuthToken_ (authToken)
    {
    }
    inline Connection::Options::Authentication::Authentication (const String& username, const String& password, Options options)
        : fOptions_ (options)
        , fUsernamePassword_ (pair<String, String>{username, password})
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
        : fRep_ (rep)
    {
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
    inline DurationSecondsType Connection::Ptr::GetTimeout () const
    {
        return fRep_->GetTimeout ();
    }
    inline void Connection::Ptr::SetTimeout (DurationSecondsType timeout)
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
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {

#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> DefaultNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::k{
        EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::BasicArrayInitializer{{
            {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eProactivelySendAuthentication, L"Proactively-Send-Authenitcation"},
            {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eRespondToWWWAuthenticate, L"Respond-To-WWW-Authenticate"},
        }}};

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_inl_*/
