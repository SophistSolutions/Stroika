/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ************************** InternetMediaTypeRegistry ***************************
     ********************************************************************************
     */
    inline optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const filesystem::path& fileSuffix) const
    {
        return GetAssociatedContentType (IO::FileSystem::FromPath (fileSuffix));
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
