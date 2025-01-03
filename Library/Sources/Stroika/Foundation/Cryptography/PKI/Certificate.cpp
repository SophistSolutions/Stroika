/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif

#include "Stroika/Foundation/Characters/ToString.h"

#include "Certificate.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 *********************** Cryptography::Certificate::Ptr *************************
 ********************************************************************************
 */
String Cryptography::PKI::Certificate::SubjectInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "country: "sv << fCountry;
    sb << ", organization: "sv << fOrganization;
    sb << ", commonName: "sv << fCommonName;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************** Cryptography::Certificate::Ptr *************************
 ********************************************************************************
 */
auto Cryptography::PKI::Certificate::Ptr::ToString () const -> String
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "subject: "sv << this->GetSubject ();
    sb << ", valid-dates: "sv << this->GetValidDates ();
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************** Cryptography::Certificate::NewSelfSigned ********************
 ********************************************************************************
 */
auto Cryptography::PKI::Certificate::NewSelfSigned (const SelfSignedCertParams& params) -> tuple<Cryptography::PKI::PrivateKey::Ptr, Ptr>
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::OpenSSL::Certificate::NewSelfSigned (params);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}