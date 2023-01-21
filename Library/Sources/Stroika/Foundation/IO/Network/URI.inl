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
    inline URI::URI (const optional<SchemeType>& scheme, const optional<Authority>& authority, const String& path,
                     const optional<String>& query, const optional<String>& fragment)
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
    template <Characters::ConvertibleToString STRISH_TYPE>
    inline URI::URI (STRISH_TYPE&& encodedURI)
        : URI{Parse (forward<STRISH_TYPE> (encodedURI))}
    {
    }
    inline bool URI::IsRelativeReference () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fScheme_.has_value ();
    }
    inline optional<URI::SchemeType> URI::GetScheme () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fScheme_;
    }
    inline void URI::SetScheme (const optional<SchemeType>& scheme)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        if (scheme) {
            scheme->Validate ();
        }
        fScheme_ = scheme;
    }
    inline void URI::SetScheme (const SchemeType& scheme)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        scheme.Validate ();
        fScheme_ = scheme;
    }
    inline optional<URI::Authority> URI::GetAuthority () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fAuthority_;
    }
    inline void URI::SetAuthority (const optional<Authority>& authority)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        CheckValidPathForAuthority_ (authority, fPath_);
        fAuthority_ = authority;
    }
    inline PortType URI::GetPortValue () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        optional<PortType>                                    op = fAuthority_ ? fAuthority_->GetPort () : optional<PortType>{};
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fPath_;
    }
    inline void URI::SetPath (const String& path)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        CheckValidPathForAuthority_ (fAuthority_, path);
        fPath_ = path;
    }
    inline URI URI::GetSchemeAndAuthority () const { return URI{GetScheme (), GetAuthority ()}; }
    template <typename RETURN_TYPE>
    RETURN_TYPE URI::GetAuthorityRelativeResource () const
        requires (is_same_v<RETURN_TYPE, String> or is_same_v<RETURN_TYPE, string> or is_same_v<RETURN_TYPE, URI>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        if constexpr (is_same_v<RETURN_TYPE, String>) {
            static constexpr UniformResourceIdentification::PCTEncodeOptions kPathEncodeOptions_{false, false, false, false, true};
            Characters::StringBuilder result = UniformResourceIdentification::PCTEncode2String (fPath_, kPathEncodeOptions_);
            if (fQuery_) {
                static constexpr UniformResourceIdentification::PCTEncodeOptions kQueryEncodeOptions_{false, false, false, true};
                result += "?"sv + UniformResourceIdentification::PCTEncode2String (*fQuery_, kQueryEncodeOptions_);
            }
            return result.str ();
        }
        if constexpr (is_same_v<RETURN_TYPE, string>) {
            return GetAuthorityRelativeResource<String> ().AsASCII ();
        }
        if constexpr (is_same_v<RETURN_TYPE, URI>) {
            return URI{nullopt, nullopt, GetPath (), GetQuery<String> ()};
        }
    }
    template <typename RETURN_VALUE>
    RETURN_VALUE URI::GetAbsPath () const
        requires (is_same_v<RETURN_VALUE, String> or is_same_v<RETURN_VALUE, optional<String>>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        if constexpr (is_same_v<RETURN_VALUE, String>) {
            if (auto op = GetAbsPath<optional<String>> ()) {
                return *op;
            }
            static const auto kException_ = Execution::RuntimeErrorException{"This URI does not have an absolute path"sv};
            Execution::Throw (kException_);
        }
        if constexpr (is_same_v<RETURN_VALUE, optional<String>>) {
            if (fPath_.empty ()) {
                return "/"sv;
            }
            if (fPath_.StartsWith ("/"sv)) {
                return fPath_;
            }
            return nullopt;
        }
    }
    template <typename RETURN_TYPE>
    inline optional<RETURN_TYPE> URI::GetQuery () const
        requires (is_same_v<RETURN_TYPE, String> or is_same_v<RETURN_TYPE, URI::Query>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        if constexpr (is_same_v<RETURN_TYPE, String>) {
            return fQuery_;
        }
        if constexpr (is_same_v<RETURN_TYPE, Query>) {
            if (fQuery_) {
                return Query{*fQuery_};
            }
            return nullopt;
        }
    }
    inline void URI::SetQuery (const optional<String>& query)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fQuery_ = query;
    }
    inline void URI::SetQuery (const optional<Query>& query)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fQuery_ = query ? query->ComputeQueryString () : optional<String>{};
    }
    inline optional<String> URI::GetFragment () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fFragment_;
    }
    inline void URI::SetFragment (const optional<String>& fragment)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fFragment_ = fragment;
    }
    inline strong_ordering URI::operator<=> (const URI& rhs) const { return URI::TWC_ (*this, rhs); }
    inline bool URI::operator== (const URI& rhs) const { return URI::TWC_ (*this, rhs) == 0; }
    template <typename T>
    inline T URI::As () const
        requires (is_same_v<T, String> or is_same_v<T, string>)
    {
        if constexpr (is_same_v<T, String>) {
            return AsString_ ();
        }
        if constexpr (is_same_v<T, string>) {
            return AsString_ ().AsASCII ();
        }
    }

}

#endif /*_Stroika_Foundation_IO_Network_URI_inl_*/
