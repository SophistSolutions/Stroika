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
            scheme->Validate ();
        }
    }
    inline bool URI::IsRelativeReference () const
    {
        return fScheme_.has_value ();
    }
    inline optional<URI::SchemeType> URI::GetScheme () const
    {
        return fScheme_;
    }
    inline void URI::SetScheme (const optional<SchemeType>& scheme)
    {
        if (scheme) {
            scheme->Validate ();
        }
        fScheme_ = scheme;
    }
    inline void URI::SetScheme (const SchemeType& scheme)
    {
        scheme.Validate ();
        fScheme_ = scheme;
    }
    inline optional<URI::Authority> URI::GetAuthority () const
    {
        return fAuthority_;
    }
    inline void URI::SetAuthority (const optional<Authority>& authority)
    {
        fAuthority_ = authority;
    }
    inline String URI::GetPath () const
    {
        return fPath_;
    }
    inline void URI::SetPath (const String& path)
    {
        fPath_ = path;
    }
    template <>
    inline auto URI::GetQuery () const -> optional<String>
    {
        return fQuery_;
    }
    template <>
    inline auto URI::GetQuery () const -> optional<Query>
    {
        if (fQuery_) {
            return Query{*fQuery_};
        }
        return nullopt;
    }
    inline void URI::SetQuery (const optional<String>& query)
    {
        fQuery_ = query;
    }
    inline void URI::SetQuery (const optional<Query>& query)
    {
        fQuery_ = query ? query->ComputeQueryString () : optional<String>{};
    }
    inline optional<String> URI::GetFragment () const
    {
        return fFragment_;
    }
    inline void URI::SetFragment (const optional<String>& fragment)
    {
        fFragment_ = fragment;
    }

}

#endif /*_Stroika_Foundation_IO_Network_URI_inl_*/
