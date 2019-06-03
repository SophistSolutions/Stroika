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
     ********************************** Connection **********************************
     ********************************************************************************
     */
    inline Connection::Connection (const shared_ptr<_IRep>& rep)
        : fRep_ (rep)
    {
    }
    inline Connection::Options Connection::GetOptions () const
    {
        return fRep_->GetOptions ();
    }
    inline URI Connection::GetURL () const
    {
        return fRep_->GetURL ();
    }
    inline void Connection::SetURL (const URI& url)
    {
        fRep_->SetURL (url);
    }
    inline void Connection::Close ()
    {
        fRep_->Close ();
    }
    inline DurationSecondsType Connection::GetTimeout () const
    {
        return fRep_->GetTimeout ();
    }
    inline void Connection::SetTimeout (DurationSecondsType timeout)
    {
        fRep_->SetTimeout (timeout);
    }

}

/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {

    template <>
    struct DefaultNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> : EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> {
        static constexpr EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> k{
            EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::BasicArrayInitializer{
                {
                    {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eProactivelySendAuthentication, L"Proactively-Send-Authenitcation"},
                    {Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options::eRespondToWWWAuthenticate, L"Respond-To-WWW-Authenticate"},
                }}};
        constexpr DefaultNames ()
            : EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> (k)
        {
        }
    };

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_inl_*/
