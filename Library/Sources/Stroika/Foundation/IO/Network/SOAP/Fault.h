/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SOAP_Fault_h_
#define _Stroika_Foundation_IO_Network_SOAP_Fault_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note DEPRECATED since Stroika v3.0d25: SOAP is in Frameworks::WebService::SOAP (an RPC protocol: see Design-Overview.md,
 *        "Where code goes: Foundation and Frameworks").
 */

namespace Stroika::Foundation::IO::Network::SOAP {

    using Characters::String;

    /**
     *  \note DEPRECATED since Stroika v3.0d25: use Frameworks::WebService::SOAP::Fault
     */
    struct Fault {
        String faultcode;
        String faultstring;
    };
    [[deprecated ("Since Stroika v3.0d25 use Frameworks::WebService::SOAP::DeSerialize (BLOB, Fault*)")]] optional<Fault>
    Deserialize_Fault (const Streams::InputStream::Ptr<byte>& from);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Fault.inl"

#endif /*_Stroika_Foundation_IO_Network_SOAP_Fault_h_*/
