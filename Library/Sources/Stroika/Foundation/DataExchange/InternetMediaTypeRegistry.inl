/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../IO/FileSystem/PathName.h"

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
    inline Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct) const
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
    inline const InternetMediaTypeRegistry& InternetMediaTypeRegistry::Get ()
    {
        static const InternetMediaTypeRegistry sThe_;
        return sThe_;
    }

}

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_inl_*/
