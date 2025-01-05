/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PrivateKey.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "Stroika/Foundation/Characters/ToString.h"

#include "PrivateKey.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 ******************* Cryptography::PKI::PrivateKey::Ptr *************************
 ********************************************************************************
 */
auto Cryptography::PKI::PrivateKey::Ptr::ToString () const -> String
{
    if (*this == nullptr) {
        return "nullptr"sv;
    }
    StringBuilder sb;
    sb << "{"sv;
    sb << "type: "sv << this->GetType ();
    sb << ", bits: "sv << this->GetBits ();
    sb << ", summary: "sv << this->GetPrintSummary ();
    sb << "}"sv;
    return sb;
}