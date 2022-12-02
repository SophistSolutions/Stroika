/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        : URI{Parse (encodedURI)}
    {
    }
    inline URI::URI (const String& encodedURI)
        : URI{Parse (encodedURI)}
    {
    }
    inline bool URI::IsRelativeReference () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fScheme_.has_value ();
    }
    inline optional<URI::SchemeType> URI::GetScheme () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fScheme_;
    }
    inline void URI::SetScheme (const optional<SchemeType>& scheme)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        if (scheme) {
            scheme->Validate ();
        }
        fScheme_ = scheme;
    }
    inline void URI::SetScheme (const SchemeType& scheme)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        scheme.Validate ();
        fScheme_ = scheme;
    }
    inline optional<URI::Authority> URI::GetAuthority () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fAuthority_;
    }
    inline void URI::SetAuthority (const optional<Authority>& authority)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        CheckValidPathForAuthority_ (authority, fPath_);
        fAuthority_ = authority;
    }
    inline PortType URI::GetPortValue () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        optional<PortType>                          op = fAuthority_ ? fAuthority_->GetPort () : optional<PortType>{};
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
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fPath_;
    }
    inline void URI::SetPath (const String& path)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        CheckValidPathForAuthority_ (fAuthority_, path);
        fPath_ = path;
    }
    inline URI URI::GetSchemeAndAuthority () const
    {
        return URI{GetScheme (), GetAuthority ()};
    }
    template <>
    inline string URI::GetAuthorityRelativeResource () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return GetAuthorityRelativeResource<String> ().AsASCII ();
    }
    template <>
    inline URI URI::GetAuthorityRelativeResource () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return URI{nullopt, nullopt, GetPath (), GetQuery<String> ()};
    }
    template <>
    inline optional<String> URI::GetAbsPath () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        if (fPath_.empty ()) {
            return L"/"sv;
        }
        if (fPath_.StartsWith (L"/")) {
            return fPath_;
        }
        return nullopt;
    }
    template <>
    inline String URI::GetAbsPath () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        if (auto op = GetAbsPath<optional<String>> ()) {
            return *op;
        }
        Execution::Throw (Execution::RuntimeErrorException{L"This URI does not have an absolute path"sv});
    }
    template <>
    inline auto URI::GetQuery () const -> optional<String>
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fQuery_;
    }
    template <>
    inline auto URI::GetQuery () const -> optional<Query>
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        if (fQuery_) {
            return Query{*fQuery_};
        }
        return nullopt;
    }
    inline void URI::SetQuery (const optional<String>& query)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        fQuery_ = query;
    }
    inline void URI::SetQuery (const optional<Query>& query)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        fQuery_ = query ? query->ComputeQueryString () : optional<String>{};
    }
    inline optional<String> URI::GetFragment () const
    {
        AssertExternallySynchronizedMutex::ReadLock critSec{*this};
        return fFragment_;
    }
    inline void URI::SetFragment (const optional<String>& fragment)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        fFragment_ = fragment;
    }
    inline strong_ordering URI::operator<=> (const URI& rhs) const
    {
        return URI::TWC_ (*this, rhs);
    }
    inline bool URI::operator== (const URI& rhs) const
    {
        return URI::TWC_ (*this, rhs) == 0;
    }

}

#endif /*_Stroika_Foundation_IO_Network_URI_inl_*/
