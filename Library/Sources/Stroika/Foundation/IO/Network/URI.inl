/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URI_inl_
#define _Stroika_Foundation_IO_Network_URI_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     *************************************** URI ************************************
     ********************************************************************************
     */
    inline URI::URI (const optional<SchemeType>& scheme, const optional<Authority>& authority, const String& path, const optional<String>& query, const optional<String>& fragment)
        : fScheme_{scheme}
        , fAuthority_{authority}
        , fPath_{path}
        , fQuery_{query}
        , fFragment_{fragment}
    {
        // @todo probably validate query and fragment args?? Maybe not needed (if not document why)
        if (scheme) {
            UniformResourceIdentification::ValidateScheme (*scheme);
        }
    }
    inline bool URI::IsURL () const
    {
        // @todo review/revise/document reference
        return fScheme_ and fAuthority_;
    }
    inline optional<URI::SchemeType> URI::GetScheme () const
    {
        return fScheme_;
    }
    inline optional<URI::Authority> URI::GetAuthority () const
    {
        return fAuthority_;
    }
    inline String URI::GetPath () const
    {
        return fPath_;
    }
    inline optional<String> URI::GetQueryString () const
    {
        return fQuery_;
    }
    inline auto URI::GetQuery () const -> optional<Query>
    {
        if (fQuery_) {
            return Query{*fQuery_};
        }
        return nullopt;
    }
    inline optional<String> URI::GetFragment () const
    {
        return fFragment_;
    }

}

#endif /*_Stroika_Foundation_IO_Network_URI_inl_*/
