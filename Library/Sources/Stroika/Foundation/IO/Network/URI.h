/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URI_h_
#define _Stroika_Foundation_IO_Network_URI_h_ 1

#include "../../StroikaPreComp.h"

#include <string>

#include "../../Characters/String.h"
#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Mapping.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "InternetAddress.h"
#include "UniformResourceIdentification.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   support Compare() and oeprator<, etc.
 *
 *      @todo   Review http://tools.ietf.org/html/rfc1808.html and see if we want params to parser to say if
 *              we allow relative urls, etc.
 *
 *              We currently produce differnt answers than https://docs.python.org/2/library/urlparse.html for the case
 *              of the url urlparse('help/Python.html'), where I treat help as a hostname, and python
 *              as part of the path.
 *
 *              Maybe params to parser to say "strict", or "can-be-relative-url", or "best guess full url"
 *
 *      @todo   Need LOTS of reg-tests tests!.
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    class URL;

    /**
     *  \par RFC Reference
     *       \note https://tools.ietf.org/html/rfc3986 - Uniform Resource Identifier (URI): Generic Syntax - 2005
     *             Updates: 1738; Obsoletes 2732, 2396, 1808
     *             (so combines relative and absolute)
     *
     *  A URI is EITHER a URL, or a relative reference to a URL. This definition is VERY MUCH LESS THAN CLEAR,
     *  but the closest I can infer from:
     *      https://tools.ietf.org/html/rfc3986
     *
     *          A URI can be further classified as a locator, a name, or both.  The
     *          term "Uniform Resource Locator" (URL) refers to the subset of URIs
     *          that, in addition to identifying a resource, provide a means of
     *          locating the resource by describing its primary access mechanism
     *          (e.g., its network "location").
     *
     *  From:
     *      https://tools.ietf.org/html/rfc3986#section-3.2
     *
     *          The generic URI syntax consists of a hierarchical sequence of
     *          components referred to as the scheme, authority, path, query, and
     *          fragment
     *          ...
     *         The following are two example URIs and their component parts:
     *
     *               foo://example.com:8042/over/there?name=ferret#nose
     *               \_/   \______________/\_________/ \_________/ \__/
     *                |           |            |            |        |
     *             scheme     authority       path        query   fragment
     *                |   _____________________|__
     *               / \ /                        \
     *               urn:example:animal:ferret:nose
     *
     *  \note This code does not currently (as of v2.1d23) address https://tools.ietf.org/html/rfc3986#appendix-C - URI delimiting (finding the boundaries of the URI from
     *        surrounding text).
     *
     *  \note   One subtlty with the URI syntax is that:
     *          https://tools.ietf.org/html/rfc3986#section-3.3
     *              If a URI contains an authority component, then the path component
     *              must either be empty or begin with a slash ("/") character
     *          If you look at the syntax/BNF, this makes sense. But logically, it makes no sense. The specification of an authority
     *          then PROHIBITS the specification of a relative path.
     *
     *          But - I don't get to write the specs ;-).
     *
     *          So this class triggers throws if you ever attempt to specify a non-empty path that doesn't start with /
     *          on a URI that has an authority.
     *
     *          This poses some difficulties for code that wants to update BOTH the authority and the path of a URI (which do you do first - tricky).
     *          But its easy enough to avoid by re-constructing the URI from scratch using the URI (individiaul components) constructor.
     *      
     */
    class URI : private Debug::AssertExternallySynchronizedLock {
    public:
        using Authority = UniformResourceIdentification::Authority;

    public:
        using Host = UniformResourceIdentification::Host;

    public:
        using PortType = UniformResourceIdentification::PortType;

    public:
        using SchemeType = UniformResourceIdentification::SchemeType;

    public:
        using Query = UniformResourceIdentification::Query;

    public:
        /**
         *  This checks and throws if arguments are invalid our out of range (e.g. a scheme with a colon in it will trigger a throw).
         *
         *  These will raise exceptions if anything illegal in the URL specification.
         *
         *  Constructor from String(or string) and no other arguments, is equivilent to calling URI::Parse ()
         */
        URI () = default;
        URI (const optional<SchemeType>& scheme, const optional<Authority>& authority, const String& path = String{}, const optional<String>& query = nullopt, const optional<String>& fragment = nullopt);
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        URI (const URL& url);
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        URI (const string& encodedURI);
        URI (const String& encodedURI);

    public:
        /**
         *  This takes argument string url, with possibly % encoded characters, accorind to https://tools.ietf.org/html/rfc3986
         *  The input characterset is always ASCII (but may encode UCS after %PCT substitions).
         *  If not handed ASCII text, an exception will be thrown.
         */
        static URI Parse (const string& rawURL);
        static URI Parse (const String& rawURL);

    public:
        /**
         *  This returns true if this is relative URI (either network relative or host-relative), and false if
         *  it contains a scheme.
         *
         *      From https://tools.ietf.org/html/rfc3986#section-4.1
         *
         *          URI-reference = URI / relative-ref
         *
         *          A URI-reference is either a URI or a relative reference.  If the
         *          URI-reference's prefix does not match the syntax of a scheme followed
         *          by its colon separator, then the URI-reference is a relative
         *          reference.
         *          ...
         *          A relative reference that begins with two slash characters is termed
         *          a network-path reference; such references are rarely used.  A
         *          relative reference that begins with a single slash character is
         *          termed an absolute-path reference.  A relative reference that does
         *          not begin with a slash character is termed a relative-path reference
         *
         *  \note Even if something is NOT a relative reference, it may not have a host/authority:
         *          e.g.
         *              mailto:John.Doe@example.com
         */
        nonvirtual bool IsRelativeReference () const;

    public:
        /**
         *  Always returns a valid (or empty) protocol/URL scheme - according to http://www.ietf.org/rfc/rfc1738.txt
         */
        nonvirtual optional<SchemeType> GetScheme () const;

    public:
        /**
         */
        nonvirtual void SetScheme (const optional<SchemeType>& scheme);
        nonvirtual void SetScheme (const SchemeType& scheme);

    public:
        /**
         *  The authority of a URI is basically the hostname (+ optional port and user info)
         */
        nonvirtual optional<Authority> GetAuthority () const;

    public:
        /**
         */
        nonvirtual void SetAuthority (const optional<Authority>& authority);

    public:
        /**
         *  Get the best guess possible for the port#, based on the given port, and given the scheme. Will return a bogus
         *  port number (like 80) if not enough information given.
         */
        nonvirtual PortType GetPortValue () const;

    public:
        /*
         *  The path MAY or MAY NOT start with a /, and it may be empty.
         *
         *  \note - the path is already decoded (% decoding and character set decoded)
         */
        nonvirtual String GetPath () const;

    public:
        /**
         *  \note - the path is a UNICODE string, and should not be url-encoded.
         */
        nonvirtual void SetPath (const String& path);

    public:
        /**
         *  Return the (PCT etc encoded) string AFTER the authority, but not including the fragment
         *
         *  \note Alias - this used to be called GetHostRelativePathPlusQuery
         *
         *  This returns the path + the query (omitting authory, scheme, and fragment).
         *
         *  RETURN_TYPE may be:
         *      o   String (default)
         *      o   string (because its all ASCII return since ENCODED)
         */
        template <typename RETURN_TYPE = String>
        nonvirtual RETURN_TYPE GetAuthorityRelativeResource () const;

    public:
        /**
         *  Return the path component, excluding any text after the final /.
         *
         *  \note Alias - this used to be called GetHostRelPathDir
         *
         *  This value maybe a full UNICODE String, and is NOT PCT encoded.
         */
        nonvirtual String GetAuthorityRelativeResourceDir () const;

    public:
        /**
         *  Return the GetPath () value, but assuring its an absolute path.
         *
         *  Return type maybe:
         *      String
         *      optional<String>
         *
         *  If return type is optional<String>, it will return nullopt when the path is NOT an absolute path.
         *  If return type is String, it will THROW when the path is not an absolute path.
         *
         *  In either case, the special case of GetPath ().empty () will be treated as '/'.
         *
         */
        template <typename RETURN_VALUE = String>
        nonvirtual RETURN_VALUE GetAbsPath () const;

    public:
        /*
         *  Return the query part of the URI as the given RETURN_TYPE. Note this this value maybe missing.
         *
         *  Supported RETURN_TYPE values are:
         *      o   String
         *      o   Query (a parsed query string - much akin to a Mapping<String,String>)
         *
         */
        template <typename RETURN_TYPE = Query>
        nonvirtual optional<RETURN_TYPE> GetQuery () const;

    public:
        /**
         */
        nonvirtual void SetQuery (const optional<String>& query);
        nonvirtual void SetQuery (const optional<Query>& query);

    public:
        /**
         */
        nonvirtual optional<String> GetFragment () const;

    public:
        /**
         */
        nonvirtual void SetFragment (const optional<String>& query);

    public:
        /**
         *  Produce a normalized represnetation of the URI.
         *
         *  Since constructing the URI object already does alot of this, some parts are not needed (like 
         *  Percent-Encoding Normalization, and character set conversion. But other parts are still useful/impactful (like tolower).
         *
         *  @see https://tools.ietf.org/html/rfc3986#section-6
         */
        nonvirtual URI Normalize () const;

    public:
        /**
         *  Supported conversion-targets (T):
         *      String - converts to the raw URI format (as it would appear in a web-browser or html link); note raw form is ASCII
         *      string - converts to the raw URI format (as it would appear in a web-browser or html link); note raw form is ASCII
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  Combine a full URI with a (possibly) relative URI, to produce a new URI. Note - its completely legal for hte argument uri
         *  to be a full url, in which case this returns its argument (taking no properties from 'this')
         *
         *      @see https://tools.ietf.org/html/rfc3986#section-5.2
         */
        nonvirtual URI Combine (const URI& uri) const;

    public:
        /**
         *  For debugging purposes: don't count on the format.
         */
        nonvirtual String ToString () const;

    private:
        static void CheckValidPathForAuthority_ (const optional<Authority>& authority, const String& path);

    private:
        optional<SchemeType> fScheme_;    // aka protocol
        optional<Authority>  fAuthority_; // aka host+port+username
        String               fPath_;      // Can be empty string, but documented as always 'present' even as empty so model that way
        optional<String>     fQuery_;     // ditto
        optional<String>     fFragment_;  // ditto
    };

    template <>
    optional<String> URI::GetQuery () const;
    template <>
    optional<URI::Query> URI::GetQuery () const;

    template <>
    String URI::As () const;
    template <>
    string URI::As () const;

    template <>
    String URI::GetAuthorityRelativeResource () const;
    template <>
    string URI::GetAuthorityRelativeResource () const;

    template <>
    String URI::GetAbsPath () const;
    template <>
    optional<String> URI::GetAbsPath () const;

    /**
     */
    bool operator== (const URI& lhs, const URI& rhs);
    bool operator!= (const URI& lhs, const URI& rhs);

}

namespace Stroika::Foundation::Common {

    template <>
    struct ThreeWayCompare<Stroika::Foundation::IO::Network::URI> {
        constexpr ThreeWayCompare () = default;
        int operator() (const Stroika::Foundation::IO::Network::URI& lhs, const Stroika::Foundation::IO::Network::URI& rhs) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "URI.inl"

#endif /*_Stroika_Foundation_IO_Network_URI_h_*/
