/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_IWSAPI_h_
#define _StroikaSample_WebServices_IWSAPI_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Stroika/Frameworks/WebService/OpenAPI/Specification.h"

#include "Model.h"

/**
 */

namespace Stroika::Samples::HTMLUI {
    using Stroika::Foundation::Characters::String;
    using Stroika::Foundation::Common::GUID;
    using Stroika::Foundation::Containers::Collection;
    using Stroika::Foundation::DataExchange::InternetMediaType;
    using Stroika::Foundation::Memory::BLOB;

    using namespace Model;

    /**
     *  \brief: IWSAPI defines an abstract version of the web service API provided by this program.
     */
    class IWSAPI {
    protected:
        IWSAPI () = default;

    public:
        IWSAPI (const IWSAPI&) = delete;
        virtual ~IWSAPI ()     = default;

    public:
        /**
         */
        virtual Stroika::Frameworks::WebService::OpenAPI::Specification GetOpenAPISpecification () const = 0;

    public:
        /**
         */
        virtual About about_GET () const = 0;

    public:
        virtual tuple<BLOB, InternetMediaType> resource_GET (const String& name) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IWSAPI.inl"

#endif /*_StroikaSample_WebServices_IWSAPI_h_*/