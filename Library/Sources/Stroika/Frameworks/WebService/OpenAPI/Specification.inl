/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_OpenAPI_Basic_inl_
#define _Stroika_Framework_WebService_OpenAPI_Basic_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebService::OpenAPI {

    inline Specification::Specification (const VariantValue& v)
        : fValue_{v}
    {
    }
    template <typename T>
    inline T Specification::As () const
        requires (same_as<T, VariantValue>)
    {
        if constexpr (same_as<T, VariantValue>) {
            return fValue_;
        }
    }

}
#endif /*_Stroika_Framework_WebService_OpenAPI_Basic_inl_*/
