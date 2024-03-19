/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_OpenAPI_Basic_h_
#define _Stroika_Framework_WebService_OpenAPI_Basic_h_ 1

#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/*
 */

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *       SUPER DUPER ROUGH DRAFT
 */

namespace Stroika::Frameworks::WebService::OpenAPI {

    using namespace Stroika::Foundation;

    using DataExchange::VariantValue;

    // https://stackoverflow.com/questions/52541842/what-is-the-media-type-of-an-openapi-schema
    inline const DataExchange::InternetMediaType kMediaType{"application/openapi+json"sv};

    // very early rough draft - for now - represnet as a VariantValue
    class Specification {

        // @
    public:
        Specification (const VariantValue& v);
        Specification (const Streams::InputStream::Ptr<byte>& b, const optional<DataExchange::InternetMediaType>& mediaType = {});
        Specification (const Specification&) = default;

        // public add accessor (properties) for ServerList??? And other aspects

    public:
        /**
         * So far only kMediaType supported.
         */
        Memory::BLOB As (const DataExchange::InternetMediaType& mediaType);

    private:
    private:
        VariantValue fValue_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Specification.inl"

#endif /*_Stroika_Framework_WebService_OpenAPI_Basic_h_*/
