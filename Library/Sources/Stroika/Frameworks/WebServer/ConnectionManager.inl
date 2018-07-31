/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_ConnectionManager_inl_
#define _Stroika_Frameworks_WebServer_ConnectionManager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ******************************** ConnectionManager *****************************
     ********************************************************************************
     */
    inline optional<String> ConnectionManager::GetServerHeader () const
    {
        return fServerHeader_;
    }
    inline auto ConnectionManager::GetCORSModeSupport () const -> CORSModeSupport
    {
        return fCORSModeSupport_;
    }
    inline optional<int> ConnectionManager::GetLinger () const
    {
        return fLinger_;
    }
    inline void ConnectionManager::SetLinger (const optional<int>& linger)
    {
        fLinger_ = linger;
    }
    inline optional<Time::DurationSecondsType> ConnectionManager::GetAutomaticTCPDisconnectOnClose ()
    {
        return fAutomaticTCPDisconnectOnClose_;
    }
    inline void ConnectionManager::SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSecondsType>& waitFor)
    {
        fAutomaticTCPDisconnectOnClose_ = waitFor;
    }
    inline optional<Interceptor> ConnectionManager::GetDefaultErrorHandler () const
    {
        return fDefaultErrorHandler_;
    }
    inline Sequence<Interceptor> ConnectionManager::GetEarlyInterceptors () const
    {
        return fEarlyInterceptors_;
    }
    inline Sequence<Interceptor> ConnectionManager::GetBeforeInterceptors () const
    {
        return fBeforeInterceptors_;
    }
    inline Sequence<Interceptor> ConnectionManager::GetAfterInterceptors () const
    {
        return fAfterInterceptors_;
    }

}

#endif /*_Stroika_Frameworks_WebServer_ConnectionManager_inl_*/
