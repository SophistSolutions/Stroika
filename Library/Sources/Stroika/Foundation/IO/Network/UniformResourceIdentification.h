/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_UniformResourceIdentification_h_
#define _Stroika_Foundation_IO_Network_UniformResourceIdentification_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>

#include <string>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/Port.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Release">Release</a>
 *
 * TODO:
 *      @todo   Add PREDEFINED namespace and list predefined protocols
 *              o   HTTP
 *              o   HTTPS
 *              o   FTP
 *              o   FILE
 *          (only do with new stroika string class so we can use low-cost constants)
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
     *  \par Empty String versus optional 'missing' !has_value
     *
     *      For the various optional pieces of a URL (or URI), we could represent this as
     *      an empty string, or call the feature optional, and return nullopt.
     *
     *      We COULD allow for EITHER, but that would be clearly needlessly confusing.
     *      PRO USE of optional
     *          o   its the normal way (since C++17) to represent that something is there, or not (string empty is a throwback
     *              sentinel value approach)
     *          o   Some objects (like port#, or authority) have formatting constraints and using optional makes
     *              clear that IF it exists its of the right form, and else it just doesn't exist (small point)
     *      CONS USE of optional
     *          o   Backward compatibility with earlier versions of the API are a bit of a pain, as the old API used
     *              string and string.empty () in many places.
     *      Performance:
     *          o   optional more often avoids allocating memory, but is larger, so probably a wash
     *      Choice:
     *          o   API will use optional<> for sub-elements of URL and URI that may or may not be present, and
     *              if a string value is provided (not nullopt) - then it MUST be a legal value (not empty typically).
     *              So, for example, Authority.Get/SetUserInfo () - MUST be either nullopt or a non-empty string.
     *          o   Exception - because the https://tools.ietf.org/html/rfc3986#page-11 explicitly says the path is not optional
     *              
     *              "A path is always defined for a URI, though the defined path may be empty (zero length)"
     *
     *              So the Path in Stroika does not use optional.
     *
     *  \note   As () versus AsEncoded () versus AsDecoded ()
     *          Some objects (like Host, or UserInfo) make sense to access in either encoded or decoded form. Other objects, like an
     *          Authority, or URI, really don't make sense except in ENCODED form (because you couldn't parse out the pieces to
     *          get back to the original). So - for objects where it makes sense to access either way, we provide AsEncoded/AsDecoded
     *          methods, and for objects that really require the string form to be encoded - we just call that As<>.
     *
     *          One SLIGHT exception is the ToString() method, which is just for debugging, and there we emit what will be easier/better
     *          for debugging, and don't worry about reversibility.
     * 
     *          \see also StringPCTEncodedFlag flag
     */

    /**
     * \brief for some purposes, we may want to render objects PCT-encoded, and sometimes not (plain or decoded). This flag is just used
     *        to specify in some 'As<String>' APIs - which is preferred for output. The default often depends on the type being produced
     *        in the As<> method.
     * 
     *  \note - SEE http://stroika-bugs.sophists.com/browse/STK-1000 -- issue about maybe needed more nuanced approach
     *         PERHAPS add another flag eDecodedUNICODEButEncodeSOMETUFFNeededToUnparseDisambiguate (not sure that ever happens)?
     *         (OK - I do have an example - the # character - and maybe sometimes ? character??? - (# in query for example - comes before fragment).
     */
    enum class StringPCTEncodedFlag {
        eDecoded,
        ePCTEncoded,
    };

    /**
     *  From https://tools.ietf.org/html/rfc3986#section-3.1
     *      Scheme names consist of a sequence of characters beginning with a
     *      letter and followed by any combination of letters, digits, plus
     *      ("+"), period ("."), or hyphen ("-").  Although schemes are case-
     *      insensitive, the canonical form is lowercase ...
     *
     *  \note schemes cannot include a ':' character
     *
     * TODO:
     *      @todo   MABYE add "SCHEME REGISTER"
     *              { string, isSecure, isHttpIsh } -na dif httpish req certain methods get GethOst etc for httpish schemes
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          o   schemes are case-insensitive  @see https://tools.ietf.org/html/rfc3986#section-3.1
     */
    class [[nodiscard]] SchemeType : public String {
        using inherited = String;

    public:
        template <Characters::IConvertibleToString STRINGISH_T>
        SchemeType (STRINGISH_T&& s);
        SchemeType (const SchemeType&) noexcept = default;
        SchemeType (SchemeType&&) noexcept      = default;

    public:
        nonvirtual SchemeType& operator= (const SchemeType&) = default;
        nonvirtual SchemeType& operator= (SchemeType&&)      = default;

    public:
        /**
         */
        nonvirtual SchemeType Normalize () const;

    public:
        /**
         */
        nonvirtual void Validate () const;

    public:
        /**
         *  Returns true iff its a KNOWN secure protocol, like https, ssh, ftps, etc. By secure, this generally means that it is TLS based.
         *  \note for unrecognized schema, this may produce a WAG so just take it as a hint.
         */
        nonvirtual bool IsSecure () const;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const SchemeType& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const SchemeType& rhs) const;

    private:
        static strong_ordering TWC_ (const SchemeType& lhs, const SchemeType& rhs); // utility code share between c++17 and c++20 versions

    public:
        nonvirtual optional<PortType> GetDefaultPort () const;
    };

    /**
     * FROM https://tools.ietf.org/html/rfc3986#section-3.2.2:
     *      The host subcomponent of authority is identified by an IP literal
     *      encapsulated within square brackets, an IPv4 address in dotted-
     *      decimal form, or a registered name.  The host subcomponent is case-
     *      insensitive.
     *
     *  This class is ALWAYS either (mutually exclusive) registerName, or internetAddress.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *              note that when comparing hosts, if they are registered names, they are compared case insensitively.
     *              @see https://tools.ietf.org/html/rfc3986#section-6.2.2.1
     */
    class [[nodiscard]] Host {
    public:
        static constexpr auto eDecoded    = StringPCTEncodedFlag::eDecoded;
        static constexpr auto ePCTEncoded = StringPCTEncodedFlag::ePCTEncoded;

    public:
        /**
         *  Technically accoridng to https://tools.ietf.org/html/rfc3986#section-3.2.2, the registered-name
         *  maybe empty, but for the sake of consistency with the rest of this module, we intead represent
         *  this using optional<Host> and say that the optional host is missing.
         *
         *  So, \req not registeredName.empty ()
         * 
         *  \todo http://stroika-bugs.sophists.com/browse/STK-750
         *        noexcept - unclear why I cannot declare copy constructor and copy assignment operators as noexcept
         *        on GCC. THIS compiles fine, but then later bits of code that use it fail to compile (g++ 9 at least).
         */
        Host (const String& registeredName);
        Host (const InternetAddress& addr);
        Host (const Host&)     = default;
        Host (Host&&) noexcept = default;

    private:
        Host () = default;

    public:
        nonvirtual Host& operator= (const Host&)     = default;
        nonvirtual Host& operator= (Host&&) noexcept = default;

    public:
        /**
         *  This takes argument a possibly %-encoded name, or [] encoded internet addresses etc, and produces a properly parsed host object
         *  This may throw if given an invalid raw URL hostname value.
         *
         *  Require (not rawURLHostnameText.empty ());  // use optional instead, and treat empty text as invalid. NB " " is OK.
         */
        static Host Parse (const String& rawURLHostnameText);

    public:
        /**
         *  See https://tools.ietf.org/html/rfc3986#section-6.2.2
         */
        nonvirtual Host Normalize () const;

    public:
        /*
         *  Returns missing if its not a  registered name (DNS name).
         *
         *  \note always AsRegisteredName () or AsInternetAddress () returns a value;
         */
        nonvirtual optional<String> AsRegisteredName () const;

    public:
        /**
         *  Returns missing if its not an InternetAddress
         *
         *  \note always AsRegisteredName () or AsInternetAddress () returns a value;
         */
        nonvirtual optional<InternetAddress> AsInternetAddress () const;

    public:
        /**
         *  \brief Returns the hostname, either encoded or decoded (PCT encoding) as some form of printed derivitive string. If the result type is 'std::string' - throws if data not ASCII;
         * 
         *  if (pctEncoded == StringPCTEncodedFlag::eDecoded)
         *      Returns decoded (no PCT encoding etc) hostname (registered name, ipv4 or ipv6 address). Doesn't contain the
         *      [] decoration around ip addresses, etc. Suitable for passing to DNS::Default::GetHostAddress ().
         *  if (pctEncoded == StringPCTEncodedFlag::ePCTEncoded)
         *      Returns encoded result (%-encoding host names, and wrapping [] around ipv6 addresses).
         * 
         *  if RESULT_TYPE==String, pctEncoded defaults to eDecoded
         *  if RESULT_TYPE==string, pctEncoded defaults to ePCTEncoded
         *
         *  \par Example Usage
         *      \code
         *          auto locAddrs = IO::Network::DNS::kThe.GetHostAddresses (host.As<String> (StringPCTEncodedFlag::eDecoded));
         *      \endcode
         */
        template <typename RESULT_TYPE = String>
        nonvirtual RESULT_TYPE As (optional<StringPCTEncodedFlag> pctEncode = {}) const
            requires (same_as<RESULT_TYPE, String> or same_as<RESULT_TYPE, string>);

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Host& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const Host& rhs) const;

    private:
        static strong_ordering TWC_ (const Host& lhs, const Host& rhs); // utility code share between c++17 and c++20 versions

    public:
        /**
         *  For debugging purposes: don't count on the format.
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika v3.0d5 - use As<String>(StringPCTEncodedFlag::eDecoded)")]] String AsDecoded () const
        {
            return As<String> (StringPCTEncodedFlag::eDecoded);
        }
        template <typename RESULT_TYPE = String>
        [[deprecated ("Since Stroika v3.0d5 - use As<String>(StringPCTEncodedFlag::ePCTEncoded)")]] RESULT_TYPE AsEncoded () const
        {
            return As<RESULT_TYPE> (StringPCTEncodedFlag::ePCTEncoded);
        }

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
     * FROM https://tools.ietf.org/html/rfc3986#section-3.2.1:
     *      The userinfo subcomponent may consist of a user name and, optionally,
     *      scheme-specific information about how to gain authorization to access
     *      the resource.
     *
     *  No claims are made about case sensativity, so this is treated as case sensitive.
     *
     *  \note UserInfo may not contain an empty string (use optional<UserInfo> and nullopt for that)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          Because https://tools.ietf.org/html/rfc3986 says nothing about case sensativity or comparing userInfo,
     *          These are compared as case-senstive strings.
     */
    class [[nodiscard]] UserInfo {
    public:
        static constexpr auto eDecoded    = StringPCTEncodedFlag::eDecoded;
        static constexpr auto ePCTEncoded = StringPCTEncodedFlag::ePCTEncoded;

    public:
        /**
         *  Note, though https://tools.ietf.org/html/rfc3986#section-3.2.1 allows for an empty UserInfo, we instead
         *  handle that case with uses of UserInfo being optional<UserInfo>. So, we require that the decoded userInfo
         *  is not an empty string in this class.
         *
         *  \req not decodedUserInfo.empty ()
         */
        UserInfo (const String& decodedUserInfo);
        UserInfo (const UserInfo&) noexcept = default;
        UserInfo (UserInfo&&) noexcept      = default;

    private:
        UserInfo () = default;

    public:
        nonvirtual UserInfo& operator= (const UserInfo&) noexcept = default;
        nonvirtual UserInfo& operator= (UserInfo&&) noexcept      = default;

    public:
        /**
         *  This takes argument a possibly %-encoded name, or [] encoded internet addresses etc, and produces a properly parsed host object
         *  This may throw if given an invalid raw URL hostname value.
         *
         *  \req not rawURLUserInfo.empty ()        // use optional<UserInfo> {} instead
         */
        static UserInfo Parse (const String& rawURLUserInfo);

    public:
        /**
         *  \brief Returns the hostname, either encoded or decoded (PCT encoding) as some form of printed derivitive string. If the result type is 'std::string' - throws if data not ASCII;
         * 
         *  if (pctEncoded == StringPCTEncodedFlag::eDecoded)
         *      Returns decoded (no PCT encoding etc) userInfo.
         *  if (pctEncoded == StringPCTEncodedFlag::ePCTEncoded)
         *      Returns encoded result (%-encoding user-info after converting to UTF8).
         * 
         *  if RESULT_TYPE==String, pctEncoded defaults to eDecoded
         *  if RESULT_TYPE==string, pctEncoded defaults to ePCTEncoded
         *
         */
        template <typename RESULT_TYPE = String>
        nonvirtual RESULT_TYPE As (optional<StringPCTEncodedFlag> pctEncoded = {}) const
            requires (same_as<RESULT_TYPE, String> or same_as<RESULT_TYPE, string>);

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const UserInfo& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const UserInfo& rhs) const;

    private:
        static strong_ordering TWC_ (const UserInfo& lhs, const UserInfo& rhs); // utility code share between c++17 and c++20 versions

    public:
        /**
         *  For debugging purposes: don't count on the format.
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika v3.0d5 - use As<String>(StringPCTEncodedFlag::eDecoded)")]] String AsDecoded () const
        {
            return As<String> (StringPCTEncodedFlag::eDecoded);
        }
        template <typename RESULT_TYPE = String>
        [[deprecated ("Since Stroika v3.0d5 - use As<String>(StringPCTEncodedFlag::ePCTEncoded)")]] RESULT_TYPE AsEncoded () const
        {
            return As<RESULT_TYPE> (StringPCTEncodedFlag::ePCTEncoded);
        }

    private:
        // Throws if cannot parse/illegal
        static String ParseRaw_ (const String& raw);

    private:
        static String EncodeAsRawURL_ (const String& decodedName);

    private:
        String fEncodedUserInfo_;
        String fUserInfo_;
    };

    template <>
    String UserInfo::AsEncoded () const;
    template <>
    string UserInfo::AsEncoded () const;

    /**
     *  \brief Authority is roughly the part of a URL where you say the hostname (and portnumber etc) - part just after //
     *
     *  Based on https://tools.ietf.org/html/rfc3986#section-3.2
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    class [[nodiscard]] Authority {
    public:
        static constexpr auto eDecoded    = StringPCTEncodedFlag::eDecoded;
        static constexpr auto ePCTEncoded = StringPCTEncodedFlag::ePCTEncoded;

    public:
        /**
         *  \todo http://stroika-bugs.sophists.com/browse/STK-750
         *        noexcept - unclear why I cannot declare copy constructor and copy assignment operators as noexect
         *        on GCC. THIS compiles fine, but then later bits of code that use it fail to compile (g++ 9 at least).
         */
        Authority (const optional<Host>& h = nullopt, const optional<PortType>& port = nullopt, const optional<UserInfo>& userInfo = nullopt);
        Authority (const Authority&)     = default;
        Authority (Authority&&) noexcept = default;

    public:
        nonvirtual Authority& operator= (const Authority&)     = default;
        nonvirtual Authority& operator= (Authority&&) noexcept = default;

    public:
        /**
         *  This takes argument a possibly %-encoded name, or [] encoded internet addresses etc, and produces a properly parsed host object
         *  This may throw if given an invalid raw URL hostname value. However, a 'missing' hostname is not an error, and will just
         *  return an Authority with HostName == nullopt.
         *
         *  \note - the argument rawURLAuthorityText must be valid ASCII, or this will throw
         *
         *  If the argument string is fully empty, this will return an empty optional authority. If its invalid/illegal, it will throw.
         */
        static optional<Authority> Parse (const String& rawURLAuthorityText);

    public:
        /**
         *  See https://tools.ietf.org/html/rfc3986#section-6.2.2
         */
        nonvirtual Authority Normalize () const;

    public:
        /**
         *  Supported conversion-targets (T):
         *      String - converts to the raw URI format (as it would appear in a web-browser or html link)
         *
         *  if RESULT_TYPE==String, pctEncoded defaults to eDecoded
         *  if RESULT_TYPE==string, pctEncoded defaults to ePCTEncoded
         */
        template <typename T>
        nonvirtual T As (optional<StringPCTEncodedFlag> pctEncode = {}) const;

    public:
        /**
         *  note that https://tools.ietf.org/html/rfc3986#appendix-A sort if indicates that the host is NOT optional, but maybe empty
         *  Because of how combining works with base urls and full URLs, I think its clearer to represent the empty case of an empty 
         *  host as a missing host specificiation.
         */
        nonvirtual optional<Host> GetHost () const;

    public:
        /**
         */
        nonvirtual void SetHost (const optional<Host>& host);

    public:
        /**
         */
        nonvirtual optional<PortType> GetPort () const;

    public:
        /**
         */
        nonvirtual void SetPort (const optional<PortType>& port);

    public:
        /**
         * FROM https://tools.ietf.org/html/rfc3986#section-3.2.1:
         *      The userinfo subcomponent may consist of a user name and, optionally,
         *      scheme-specific information about how to gain authorization to access
         *      the resource
         *
         *  \note This value is typically missing (nullopt)
         */
        nonvirtual optional<UserInfo> GetUserInfo () const;

    public:
        /**
         *  @see GetUserInfo ()
         */
        nonvirtual void SetUserInfo (const optional<UserInfo>& userInfo);

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Authority& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const Authority& rhs) const;

    private:
        static strong_ordering TWC_ (const Authority& lhs, const Authority& rhs); // utility code share between c++17 and c++20 versions

    public:
        /**
         *  For debugging purposes: don't count on the format.
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        optional<Host>     fHost_;
        optional<PortType> fPort_;
        optional<UserInfo> fUserInfo_;
    };

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          Nothing in https://tools.ietf.org/html/rfc3986#section-3.4 appears to indicate case insensative so treat as case sensitive
     *
     *          comparing for equals makes full sense. But comparing < really doesn't, because there is no obvious preferred order for query strings
     *          So pick a preferred ordering (alphabetical) - and compare one after the other
     *          @todo see http://stroika-bugs.sophists.com/browse/STK-144 and fix when that is fixed
     *
     * According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before doing % etc substitution, so support u8string overload.
     */
    class [[nodiscard]] Query {
    public:
        /**
         */
        Query (const u8string& query);
        Query (const String& query);
        Query (const Query&) noexcept = default;
        Query (Query&&) noexcept      = default;

    public:
        nonvirtual Query& operator= (const Query&) = default;
        nonvirtual Query& operator= (Query&&)      = default;

    public:
        nonvirtual const Containers::Mapping<String, String>& GetMap () const;

    public:
        nonvirtual String operator() (const u8string& idx) const;
        nonvirtual String operator() (const String& idx) const;

    public:
        nonvirtual bool HasField (const u8string& idx) const;
        nonvirtual bool HasField (const String& idx) const;

    public:
        nonvirtual void AddField (const String& idx, const String& value);

    public:
        nonvirtual void RemoveFieldIfAny (const u8string& idx);
        nonvirtual void RemoveFieldIfAny (const String& idx);

    public:
        // Return wide string, but all ascii characters
        // http://tools.ietf.org/html/rfc3986
        nonvirtual String ComputeQueryString () const;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Query& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const Query& rhs) const;

    private:
        static strong_ordering TWC_ (const Query& lhs, const Query& rhs); // utility code share between c++17 and c++20 versions

    public:
        /**
         *  For debugging purposes: don't count on the format.
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        Containers::Mapping<String, String> fMap_;
    };

    /*
     *  See http://tools.ietf.org/html/rfc3986
     *  This doesn't encode an entire URL, just a particular field
     */
    u8string EncodeURLQueryStringField (const String& s);

    /**
     *  See https://tools.ietf.org/html/rfc3986#appendix-A for the meaning of encodeGenDelims/encodeSubDelims
     *
     *  When called with a String, the string is first UTF8 encoded.
     *
     *  This is mostly used INTERNALLY in parsing URLs, and probably should not be used otherwise.
     */
    struct [[nodiscard]] PCTEncodeOptions {
        // From https://tools.ietf.org/html/rfc3986#appendix-A
        // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
        // gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
        // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
        // we always allow 'unreserved'
        bool allowSubDelims        = false;
        bool allowGenDelims        = false;
        bool allowPChar            = false; // sub-delims + ":" / "@"
        bool allowFragOrQueryChars = false; // pchar / "/" / "?"
        bool allowPathCharacters = false; // COMPLICATED - I THINK this means sub-delims + '/' (@ and : maybe sometimes allowed, but I think always safe to encode)
    };
    u8string PCTEncode (const u8string& s, const PCTEncodeOptions& options);
    u8string PCTEncode (const String& s, const PCTEncodeOptions& options);
    String   PCTEncode2String (const String& s, const PCTEncodeOptions& options);

    /**
     *  PCTDecode2String () takes the result of PCTDecode, and treats it as UTF8 text, and converts a String from that.
     */
    u8string PCTDecode (const u8string& s);
    String   PCTDecode2String (const u8string& s);
    String   PCTDecode2String (const String& s);

}

namespace std {
    template <>
    class hash<Stroika::Foundation::IO::Network::UniformResourceIdentification::Host> {
    public:
        size_t operator() (const Stroika::Foundation::IO::Network::UniformResourceIdentification::Host& arg) const;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UniformResourceIdentification.inl"

#endif /*_Stroika_Foundation_IO_Network_UniformResourceIdentification_h_*/
