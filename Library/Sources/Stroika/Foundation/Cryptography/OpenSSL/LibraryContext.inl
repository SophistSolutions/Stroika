/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
