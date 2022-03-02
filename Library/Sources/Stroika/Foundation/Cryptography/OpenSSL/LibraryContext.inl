/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qHasFeature_OpenSSL
    /*
     ********************************************************************************
     *************** OpenSSL::LibraryContext::TemporarilyAddProvider ****************
     ********************************************************************************
     */
    inline LibraryContext::TemporarilyAddProvider::TemporarilyAddProvider (LibraryContext* context, const String& providerName)
        : fContext_{context}
        , fProviderName_{providerName}
    {
        RequireNotNull (context);
        context->LoadProvider (providerName);
    }
    inline LibraryContext::TemporarilyAddProvider::~TemporarilyAddProvider ()
    {
        AssertNotNull (fContext_);
        fContext_->UnLoadProvider (fProviderName_);
    }
#endif

}

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_*/
