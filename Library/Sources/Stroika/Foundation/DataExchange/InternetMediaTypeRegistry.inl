/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ************************** InternetMediaTypeRegistry ***************************
     ********************************************************************************
     */
    inline optional<InternetMediaTypeRegistry::FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetPreferredAssociatedFileSuffix (ct);
    }
    inline Containers::Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetAssociatedFileSuffixes (ct);
    }
    inline optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetAssociatedPrettyName (ct);
    }
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
