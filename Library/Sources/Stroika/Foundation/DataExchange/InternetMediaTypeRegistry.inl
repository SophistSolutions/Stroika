/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ************************** InternetMediaTypeRegistry ***************************
     ********************************************************************************
     */
    inline auto InternetMediaTypeRegistry::MakeSharedFrontendRep_ (const IFrontendRep_& t) -> shared_ptr<IFrontendRep_>
    {
        return MakeSharedPtr<FrontendRep_> (t.GetBackendRep (), t.GetOverrides ());
    }
    inline optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const filesystem::path& fileSuffix) const
    {
        return GetAssociatedContentType (String{fileSuffix});
    }
    template <typename EXCEPTION>
    inline void InternetMediaTypeRegistry::CheckIsA (const InternetMediaType& moreGeneralType, const InternetMediaType& moreSpecificType,
                                                     const EXCEPTION& throwIfNot) const
    {
        if (not IsA (moreGeneralType, moreSpecificType)) {
            Execution::Throw (throwIfNot);
        }
    }

}
