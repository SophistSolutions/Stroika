/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_UniformResourceIdentification_h_
#define _Stroika_Foundation_IO_Network_UniformResourceIdentification_h_ 1

#include "../../StroikaPreComp.h"

#include <string>

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Mapping.h"
#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Use "Value" style hack to deal with optional stuff, and use more of optional class, at least in API.
 *              That way - we capture just what the user said, and only when he askes for info, do we use the Value() variant
 *              that plugs in defaults.
 *
 *              (partly/maybe done?)
 *
 *      @todo   support Compare() and oeprator<, etc.
 *
 *      @todo   CURRENT code sometimes (like for scheme) normalizes on assignemnt. But should preserve what caller
 *              says. Just 'normalize' on compare, or have extra field which is normalized string (tolower).
 *
 *      @todo   MABYE "SCHEMEREGISTER"
 *              { string, isSecure, isHttpIsh } -na dif httpish req certina methdos get GethOst etc for httpish schemes
 *
 *      @todo   Add PREDEFINED namespace and list predefined protocols
 *              o   HTTP
 *              o   HTTPS
 *              o   FTP
 *              o   FILE
 *          (only do with new stroika string class so we can use low-cost constants)
 *
 *      @todo   Add Compare() method, and operator<, so we can easily add URL to
 *              sorted types.
 *
 *      @todo   Need more regression tests
 */

namespace Stroika::Foundation::IO::Network::UniformResourceIdentification {

    using Characters::String;

    /**
     *  \par Base RFC References
     *       \note http://www.ietf.org/rfc/rfc1738.txt - Uniform Resource Locators (URL) - 1994
     *             original FULL URL spec
     *
     *       \note http://tools.ietf.org/html/rfc1808 - Relative Uniform Resource Locators - 1995
     *             original FULL URL spec
     *
     *       \note https://tools.ietf.org/html/rfc3986 - Uniform Resource Identifier (URI): Generic Syntax - 2005
     *             Updates: 1738; Obsoletes 2732, 2396, 1808
     *             (so combines relative and absolute)
     *
     *  Each RFC uses its own terminology for the parts of a URL. We the most recent of these RFCs for our primary terminology:
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
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     *
     *  Stroika's URL class comprises the concept URI Reference (https://tools.ietf.org/html/rfc3986#section-4.1):
     *      URI-reference = URI / relative-ref
     *
     *  So a given URL object maybe a FullURI, or a RelativeURI () - 
        &&& TBD but probabkly add
         GetType () => URLType { eFullURI, eRelativeURI } based on ??? if it has authority?
         NO - because many different subtypes. 
         Just bool IsFullURL () (all parts all needed parts specified - scheme principally)
         READ https://tools.ietf.org/html/rfc3986#section-4.2 more carefully

         AND NEED API TO GLUE FULL_URI with RELATIVE_URI () to produce a new FULL_URI
     *
     *  \par Empty String versus optional 'missing' !has_value
     *
     *      For the various optional peices of a URL (or URI), we could represent this as
     *      an empty string, or call the feature optional, and return nullopt.
     *
     *      We COULD allow for EITHER, but that would be clearly needlessly confusing.
     *      PRO USE of optional
     *          o   its the normal way (since C++17) to represent that something is there, or not (string empty is a throwback
     *              sentinal value approach)
     *          o   Some objects (like port#, or authority) have formatting constraints and using optional makes
     *              clear that IF it exists its of the right form, and else it just doesn't exist (small point)
     *      CONS USE of optional
     *          o   Backward compatability with earlier versions of the API are a bit of a pain, as the old API used
     *              string and string.empty () in many places.
     *      Performance:
     *          o   optional more often avoids allocating memory, but is larger, so probably a wash
     *      Choice:
     *          o   API will use optional<> for sub-elements of URL and URI that may or may not be present, and
     *              if a string value is provoded (not nullopt) - then it MUST be a legal value (not empty typically).
     *              So, for example, Authority.Get/SetUserInfo () - MUST be either nullopt or a non-empty string.
     *          o   Exception - because the https://tools.ietf.org/html/rfc3986#page-11 explicitly says the path is not optional
     *              
     *              "A path is always defined for a URI, though the defined path may be empty (zero length)"
     *
     *              So the Path in Stroika does not use optional.
     *
     *
     * TODO:
     *      @todo   DOCUMENT and ENFORCE restrictions on values of query string, hostname, and protocol (eg. no : in protocol, etc)
     */

    /**
     *  From https://tools.ietf.org/html/rfc3986#section-3.1
     *      Scheme names consist of a sequence of characters beginning with a
     *      letter and followed by any combination of letters, digits, plus
     *      ("+"), period ("."), or hyphen ("-").  Although schemes are case-
     *      insensitive, the canonical form is lowercase ...
     *
     *  \note schemes cannot include a ':' character
     *
     *  \note Schemes should generally be treated as case insensitive, so compare with String::EqualToCI{}
     */
    using SchemeType = String;

    /**
     */
    SchemeType NormalizeScheme (const SchemeType& s);

    /**
     */
    void ValidateScheme (const SchemeType& s);

    /**
     */
    using PortType = uint16_t;

    /**
     *  Probably should define standard protos here - with named constants - like http/ftp/https etc
     */
    optional<PortType> GetDefaultPortForScheme (const optional<String>& scheme);

    /**
     * FROM https://tools.ietf.org/html/rfc3986#section-3.2.2:
     *      The host subcomponent of authority is identified by an IP literal
     *      encapsulated within square brackets, an IPv4 address in dotted-
     *      decimal form, or a registered name.  The host subcomponent is case-
     *      insensitive.
     */
    class Host {
    public:
        /**
         */
        Host (const String& registeredName);
        Host (const InternetAddress& addr);

    private:
        Host () = default;

    public:
        /**
         *  This takes argument a possibly %-encoded name, or [] encoded internet addresse etc, and produces a properly parsed host object
         *  This may throw if given an invalid raw URL hostname value.
         */
        static Host Parse (const String& rawURLHostnameText);

    public:
        /*
         *  Returns missing if its nota  registered name (dnsname).
         */
        nonvirtual optional<String> AsRegisteredName () const;

    public:
        /**
         *  Returns missing if its not an InternetAddress
         */
        nonvirtual optional<InternetAddress> AsInternetAddress () const;

    public:
        /*
         *  Returns encoded result (%-encoding host names, and wrapping [] around ipv6 addresses).
         */
        nonvirtual String AsEncoded () const;

    private:
        // Throws if cannot parse/illegal
        static pair<optional<String>, optional<InternetAddress>> ParseRaw_ (const String& raw);

    private:
        static String EncodeAsRawURL_ (const String& registeredName);
        static String EncodeAsRawURL_ (const InternetAddress& ipAddr);

    private:
        String                    fEncodedName_;
        optional<String>          fRegisteredName_;
        optional<InternetAddress> fInternetAddress_;
    };

    /**
     *  note that when comparing hosts, if they are registered names, they are compared case insensitively.
     *  @see https://tools.ietf.org/html/rfc3986#section-6.2.2.1
     */
    bool operator== (const Host& lhs, const Host& rhs);
    bool operator!= (const Host& lhs, const Host& rhs);

    /**
     *  Based on https://tools.ietf.org/html/rfc3986#section-3.2
     */
    struct Authority {

        /*
         *  note that https://tools.ietf.org/html/rfc3986#appendix-A sort if indicates that the host is NOT optional, but maybe empty
         *  Because of how combining works with base urls and full URLs, I think its clearer to represent the empty case of an empty 
         *  host as a missing host specificiation.
         */
        optional<Host> fHost;

        optional<PortType> fPort;

        /**
         * FROM https://tools.ietf.org/html/rfc3986#section-3.2.1:
         *      The userinfo subcomponent may consist of a user name and, optionally,
         *      scheme-specific information about how to gain authorization to access
         *      the resource
         */
        optional<String> fUserInfo;

        Authority (const optional<Host>& h = nullopt, const optional<PortType>& port = nullopt, const optional<String>& userInfo = nullopt);

        /**
         *  This takes argument a possibly %-encoded name, or [] encoded internet addresse etc, and produces a properly parsed host object
         *  This may throw if given an invalid raw URL hostname value.
         *
         *  If the argument string is fully empty, this will return an empty optional authority. If its invalid/illegal, it will throw.
         */
        static optional<Authority> Parse (const String& rawURLAuthorityText);

    public:
        /**
         *  Supported conversion-targets (T):
         *      String - converts to the raw URI format (as it would appear in a web-browser or html link)
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  For debugging purposes: don't count on the format.
         */
        nonvirtual String ToString () const;
    };

    /**
     */
    class Query {
    public:
        Query (const string& query);
        Query (const String& query);

    public:
        nonvirtual const Containers::Mapping<String, String>& GetMap () const;

    public:
        nonvirtual String operator() (const string& idx) const;
        nonvirtual String operator() (const String& idx) const;

    public:
        nonvirtual bool HasField (const string& idx) const;
        nonvirtual bool HasField (const String& idx) const;

    public:
        nonvirtual void AddField (const String& idx, const String& value);

    public:
        nonvirtual void RemoveFieldIfAny (const string& idx);
        nonvirtual void RemoveFieldIfAny (const String& idx);

    public:
        // Return wide string, but all ascii characters
        // http://tools.ietf.org/html/rfc3986
        nonvirtual String ComputeQueryString () const;

    private:
        Containers::Mapping<String, String> fMap_;
    };

    /*
     *  See http://tools.ietf.org/html/rfc3986
     *  This doesn't encode an entire URL, just a particular field
     */
    string EncodeURLQueryStringField (const String& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UniformResourceIdentification.inl"

#endif /*_Stroika_Foundation_IO_Network_UniformResourceIdentification_h_*/
