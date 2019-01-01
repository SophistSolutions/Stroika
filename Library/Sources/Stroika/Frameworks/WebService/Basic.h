/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Basic_h_
#define _Stroika_Framework_WebService_Basic_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Sequence.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"

/*
 */

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
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
