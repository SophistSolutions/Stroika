/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

    /*
     ********************************************************************************
     *************** OpenSSL::LibraryContext::TemporarilyAddProvider ****************
     ********************************************************************************
     */
    inline LibraryContext::TemporarilyAddProvider::TemporarilyAddProvider (LibraryContext* context, const String& providerName)
        : fContext{context}
        , fProviderName_{providerName}
    {
        RequireNotNull (context);
        context->LoadProvider (providerName);
    }
    inline LibraryContext::TemporarilyAddProvider::~TemporarilyAddProvider ()
    {
        AssertNotNull (fContext);
        fContext->UnLoadProvider (fProviderName_);
    }

}

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_inl_*/
