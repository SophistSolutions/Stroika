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
        CheckValidPathForAuthority_ (authority, path);
    }
    inline URI::URI (const string& encodedURI)
        : URI (Parse (encodedURI))
    {
    }
    inline URI::URI (const String& encodedURI)
        : URI (Parse (encodedURI))
    {
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
        CheckValidPathForAuthority_ (authority, fPath_);
        fAuthority_ = authority;
    }
    inline URI::PortType URI::GetPortValue () const
    {
        optional<PortType> op = fAuthority_ ? fAuthority_->GetPort () : optional<PortType>{};
        if (op) {
            return *op;
        }
        static constexpr PortType kDefault_{0}; // should return 80????
        if (fScheme_) {
            return fScheme_->GetDefaultPort ().value_or (kDefault_);
        }
        return kDefault_;
    }
    inline String URI::GetPath () const
    {
        return fPath_;
    }
    inline void URI::SetPath (const String& path)
    {
        CheckValidPathForAuthority_ (fAuthority_, path);
        fPath_ = path;
    }
    template <>
    inline string URI::GetAuthorityRelativeResource () const
    {
        return GetAuthorityRelativeResource<String> ().AsASCII ();
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

    /*
     ********************************************************************************
     ******************************** URI operators *********************************
     ********************************************************************************
     */
    inline bool operator!= (const URI& lhs, const URI& rhs)
    {
        return not(lhs == rhs);
    }

}

#endif /*_Stroika_Foundation_IO_Network_URI_inl_*/
