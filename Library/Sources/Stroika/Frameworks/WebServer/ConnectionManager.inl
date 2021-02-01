/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    inline ConnectionManager::Options ConnectionManager::GetOptions () const
    {
        return fEffectiveOptions_;
    }
    inline Sequence<Interceptor> ConnectionManager::GetAfterInterceptors () const
    {
        return fAfterInterceptors_;
    }

}

#endif /*_Stroika_Frameworks_WebServer_ConnectionManager_inl_*/
