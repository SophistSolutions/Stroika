/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Basic_h_
#define _Stroika_Framework_WebService_Basic_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::WebService {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;
    using Containers::Set;
    using DataExchange::InternetMediaType;

    /**
     */
    struct WebServiceMethodDescription {
        String                      fOperation;
        optional<Set<String>>       fAllowedMethods; // e.g. GET
        optional<InternetMediaType> fResponseType;
        optional<String>            fOneLineDocs;
        optional<Sequence<String>>  fCurlExample;
        optional<Sequence<String>>  fDetailedDocs;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Basic.inl"

#endif /*_Stroika_Framework_WebService_Basic_h_*/
