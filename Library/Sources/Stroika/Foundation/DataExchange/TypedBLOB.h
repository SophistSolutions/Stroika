/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_TypedBLOB_h_
#define _Stroika_Foundation_DataExchange_TypedBLOB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    /**
     *  \brief TypedBLOB is a named tuple<Memory::BLOB, optional<InternetMediaType>> - with friendlier names, and
     *         serialization properties.
     * 
     *  \par Example Usage
     *      \code
     *          return TypedBLOB{
     *              .fData = GetOpenAPISpecification ().As (Frameworks::WebService::OpenAPI::kMediaType),
     *              .fType = Frameworks::WebService::OpenAPI::kMediaType,
     *          };
     *      \endcode
     *  
     *  \par Example Usage
     *      \code
     *          TypedBLOB{someBLOBData, DataExchange::InternetMediaTypes::kOctetStream}
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          TypedBLOB{someBLOBData}
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          return TypedBLOB{.fData = GetData (), .fType = GetContentType ()};
     *      \endcode
     */
    class [[nodiscard]] TypedBLOB {
    public:
        Memory::BLOB                fData;
        optional<InternetMediaType> fType;

    public:
        nonvirtual String ToString () const;

    public:
        bool operator== (const TypedBLOB&) const = default;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TypedBLOB.inl"

#endif /*_Stroika_Foundation_DataExchange_TypedBLOB_h_*/
