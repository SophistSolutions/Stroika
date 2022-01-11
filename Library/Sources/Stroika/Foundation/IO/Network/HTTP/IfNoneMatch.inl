/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_inl_
#define _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ***************************** HTTP::IfNoneMatch ********************************
     ********************************************************************************
     */
    inline IfNoneMatch::IfNoneMatch (const Containers::Sequence<ETag>& etags)
        : fETags{etags}
    {
    }
    inline bool IfNoneMatch::IsAsterisk () const
    {
        return fETags.empty ();
    }
    inline optional<IfNoneMatch> IfNoneMatch::Parse (const String& wireFormat)
    {
        // @see https://tools.ietf.org/html/rfc7232#section-3.2
        //      If-None-Match = "*" / 1#entity-tag
        // @see https://tools.ietf.org/html/rfc7232#appendix-C
        //      If-Match = "*" / ( *( "," OWS ) entity-tag *( OWS "," [ OWS entity-tag ] ) )
        //      entity-tag = [ weak ] opaque-tag
        //      opaque-tag = DQUOTE *etagc DQUOTE
        //      etagc = "!" / %x23-7E ; '#'-'~' / obs - text
        //      weak = %x57.2F ; W/
        // @see https://tools.ietf.org/html/rfc7230#section-3.2.6
        //      obs-text       = %x80-FF
        //
        // So space/comma separated. But etags always start with W or ", and always end with next "
        Containers::Sequence<ETag> result;
        wstring                    t = wireFormat.As<wstring> ();
        wstring                    curETagToken;
        bool                       readingETag = false;
        for (auto i = t.begin (); i != t.end (); ++i) {
            if (readingETag) {
                curETagToken += *i;
                if (*i == '\"') {
                    if (auto oe = ETag::Parse (curETagToken)) {
                        result += *oe;
                        readingETag = false;
                        curETagToken.clear ();
                    }
                    else {
                        // Treat a bad etag as a bad if-none-else line, and just silently fail to parse
                        return nullopt;
                    }
                }
            }
            else {
                if (*i == 'W' or *i == '\"') {
                    curETagToken += *i;
                    readingETag = true;
                }
            }
        }
        if (readingETag) {
            // then parse didn't parse correctly, so treat as failure
            return nullopt;
        }
        else {
            return IfNoneMatch{result};
        }
    }
    template <>
    inline Characters::String IfNoneMatch::As () const
    {
        if (IsAsterisk ()) {
            return L"*"sv;
        }
        return String::Join (fETags.Select<String> ([] (const ETag& e) { return e.As<String> (); }));
    }
    inline Characters::String IfNoneMatch::ToString () const
    {
        return As<String> ();
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_inl_*/
