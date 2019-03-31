/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_h_
#define _Stroika_Foundation_IO_Network_URL_h_ 1

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
 *      @todo   Read http://tools.ietf.org/html/rfc1808.html section "Resolving Relative URLs"
 *              Maybe add method (resolverelative)? - that performns this logic.
 *
 *      @todo   MABYE "SCHEMEREGISTER"
 *              { string, isSecure, isHttpIsh } -na dif httpish req certina methdos get GethOst etc for httpish schemes
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
 *      @todo   CLEANUP PROTOCOL AND HOSTNAME METHODS - ADD CLEAR REQUIREMENTS AND ENFORCE
 *              ESP FOR STUFF LIKE COLONS!!!
 *
 *      @todo   Add overload for URL with map<> for query.
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
 *      @todo   Need LOTS of reg-tests tests!.
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    class URLQuery;

    /**
     *  Class to help encode/decode the logical parts of an internet URL
     *
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
     * TODO:
     *      @todo   DOCUMENT and ENFORCE restrictions on values of query string, hostname, and protocol (eg. no : in protocol, etc)
     *
     *      @todo   Bake in as member the URLQuery object
     */
    class URL {
    public:
        /**
         *  From https://tools.ietf.org/html/rfc3986#section-3.1
         *      Scheme names consist of a sequence of characters beginning with a
         *      letter and followed by any combination of letters, digits, plus
         *      ("+"), period ("."), or hyphen ("-").  Although schemes are case-
         *      insensitive, the canonical form is lowercase ...
         *
         *  \note schemes cannot include a ':' character
         */
        using SchemeType = String;

    public:
        /**
         */
        using PortType = uint16_t;

    public:
        /**
         *  Based on https://tools.ietf.org/html/rfc3986#section-3.2
         */
        struct Authority {

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
                /*
                 *  Returns missing if its not an InternetAddress
                 */
                nonvirtual optional<InternetAddress> AsInternetAddress () const;

            public:
                /*
                 *  Returns encoded result (%-encoding host names, and wrapping [] around ipv6 addresses).
                 */
                nonvirtual String AsEncodedHostName () const;

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

            private:
                friend bool operator== (const Host& lhs, const Host& rhs);
            };

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

            Authority (const optional<Host>& h = nullopt, optional<PortType> port = nullopt, optional<String> userInfo = nullopt);

            /**
             *  This takes argument a possibly %-encoded name, or [] encoded internet addresse etc, and produces a properly parsed host object
             *  This may throw if given an invalid raw URL hostname value.
             *
             *  If the argument string is fully empty, this will return an empty optional authority. If its invalid/illegal, it will throw.
             */
            static optional<Authority> Parse (const String& rawURLAuthorityText);
        };

        ///// &&&&& @TODO
        /// use https://tools.ietf.org/html/rfc3986#page-50 - Parsing a URI Reference with a Regular Expression
        /// to redo the parsing logic

    public:
        /**
         *      @todo PRELIM!!!!
         *              PROABBLY NEED "FORCE_AS_GOOD_VALID_FULL_URL
         *              MAYBE FULL_OR_RELATIVE (as in webpage urls)
         *
         *      Maybe URL has method "IsRelative"?
         &          &&^&but first decide how to handle URL TYPE
          * MABYBE URLTYPE { enum  - eRelative, eFull, eEmpty }; PITA to lose empty ability. But can you set stuff on a URL that is empty? like port? Is it still empty?


          Need API to COMBINE URLS (LHS must be not relative, and RHS must be RELATIVE)
         */
        enum ParseOptions {
            /*
             *  Check for strict url conformance (URI not URI-Reference)
             *
             *  Throws if not an absolute URI (meaning it must have a scheme, and an authority (host).
             *
             *  Same as parsing (eNormal) - but throw if IsRelativeURL ()
             *
             *  probably should REANME to eThrowIfNotFullURL
             */
            eAsFullURL,

            /*
             *  ???http://tools.ietf.org/html/rfc1808
             *
             *  (MAYBE USE https://tools.ietf.org/html/rfc3986 and see about whcih part - full url)
             *
             *  @todo DEPRECATE THIS!!!!
              * APPEARS SAFE TO DEPREACTE BUT DONT DO TIL I REDO ITS REGTESTS USING eNormal) and Verify (IsRelative ())
             */
            eAsRelativeURL,

            /**
             *  @todo RENAME THIS TO
             *      eNormally           - As rfc3986
             *      THIS IS WHAT IS CALLED 'URI-reference' in https://tools.ietf.org/html/rfc3986#section-4.3
             *      CAN produce URL which is relative or absoluete (new IsAbsolute () method checks for presence of schema, and authority/host)
             &&& AND 
             *  AND make it the DEFAULT when constructing URL from String
             */
            eFlexiblyAsUI,

            /**
             *  @todo DEPRECATE THIS!!!!
             *
             *  @todo CLEANUP/FIX
             *
             *  I THINK The only quirk of this node is that
             *      dyn:/foo.html gets parsed as HOST=EMPTY; relativeURL= foo.html; That makes sense
             *      but doesn't apper to conform to spec for urls (rfs) above. Research/analyze...
             *
             *  \note - this is is STILL used alot in HealthFrame - as of 2019-03-27
             */
            eStroikaPre20a50BackCompatMode,
        };

    public:
        /**
         *  See SetScheme () for handling of the 'scheme' parameter.
         *  See SetQuery() for setting the query parameter.
         *  See SetHostRelativePath for the 'relPath' restrictions.
         *  This Requires() its arguments are valid and in range. use
         *
         *  (except for the default constuctor) - these will raise exceptions if anything illegal in the URL specification.
         *  \todo - maybe we shouldn't have an empty url constructor?"
         */
        URL ();
        URL (const SchemeType& scheme, const String& host, const optional<PortType>& portNumber = nullopt, const String& relPath = String (), const String& query = String (), const String& fragment = String ());
        URL (const SchemeType& scheme, const String& host, const String& relPath, const String& query = String (), const String& fragment = String ());
        URL (const String& urlText, ParseOptions po);

    public:
        /**
         */
        static URL Parse (const String& urlText, ParseOptions po = eAsFullURL);

    private:
        static URL ParseHosteStroikaPre20a50BackCompatMode_ (const String& w);
        static URL ParseHostRelativeURL_ (const String& w);

    public:
        /**
         *  Returns if the URL protocol is secure (SSL-based). If the URL scheme is not recognized, this returns false.
         */
        nonvirtual bool IsSecure () const;

    public:
        nonvirtual optional<Authority> GetAuthority () const;

    public:
        /**
         *  Returns a String representation of the fully qualified URL.
         */
        nonvirtual String GetFullURL () const;

    public:
        /**
         *      If port# not specified, returns detault given the protocol.
         */
        nonvirtual PortType GetPortValue (PortType defaultValue = 80) const;

    public:
        /**
         *  This value overrides the port# used by the protocol. This can be empty, implying the URL refers to the default
         *  port for the given URL scheme.
         *
         *  @see GetPortValue
         *  @see SetPortNumber
         */
        nonvirtual optional<PortType> GetPortNumber () const;

    public:
        /**
         *  @see GetPortNumber ();
         */
        nonvirtual void SetPortNumber (const optional<PortType>& portNum = nullopt);

    public:
        /**
         *  NYI
         *
         *  Presume the given url is the owning webpage. What is the print string for this relative url.
         */
        nonvirtual String GetRelativeURL (const URL& baseURL) const;

    public:
        /**
         *  \brief Compare if two URLs are identical.
         *
         *  \note This is NOT the same as this->GetFullURL () == rhs.GetFullURL (), because default values may be
         *        applied in generating the FullURL () - and those might be the source of differences in the field by
         *        field comparison.
         *
         *  @todo   (modulo what should be compared canse instaitivive???) - hostname?
         *          PROTOCOL SB case INsensitiatve, and if there is a hostame, that should be comapred case insenswative. But rest is case sensative.
         */
        nonvirtual bool Equals (const URL& rhs) const;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         */
        nonvirtual int Compare (const URL& rhs) const;

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         *  This is the value created by clear or the no-arg CTOR.
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Always returns a valid (or empty) protocol/URL scheme - according to http://www.ietf.org/rfc/rfc1738.txt
         */
        nonvirtual optional<SchemeType> GetScheme () const;

    public:
        /**
         *  @see GetScheme, but defaults to (typically) to http. Roughly equivilent to GetScheme ().Value (L"https") - except that we might take other info into acocunt to select another default scheme
         */
        nonvirtual SchemeType GetSchemeValue () const;

    public:
        /**
         *  Since From http://www.ietf.org/rfc/rfc1738.txt suggests mapping upper case to lower case, this function does that.
         *  But other violations in the format of a protocol generate exceptions.
         */
        nonvirtual void SetScheme (const optional<SchemeType>& scheme);
        nonvirtual void SetScheme (const SchemeType& scheme);

    public:
        /**
         */
        nonvirtual String GetHost () const;

    public:
        /**
         @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
            */
        nonvirtual void SetHost (const String& host);

    public:
        /**
         *  Note this does NOT contain the '/' separating the hostname from the relative path,
         *  so it typically doesn't start with a '/', but frequently contains them. It MAY legally
         *  start with a '/' because http://www.ietf.org/rfc/rfc1738.txt doesn't specifically prohibit this.
         *
         *  \note   This does not include the query string, if any.
         *
         *  @see GetHostRelURLString ()
         */
        nonvirtual String GetHostRelativePath () const;

    public:
        /**
         *  @see GetHostRelativePath for format restrictions.
         */
        nonvirtual void SetHostRelativePath (const String& hostRelativePath);

    public:
        /**
         *   @see GetHostRelativePath for format restrictions. This can be empty.
         */
        nonvirtual String GetHostRelPathDir () const;

    public:
        /**
         */
        nonvirtual URLQuery GetQuery () const;

    public:
        /**
         */
        nonvirtual void SetQuery (const URLQuery& query);

    public:
        /**
         *  \note   This returns an empty string if no query.
         */
        nonvirtual String GetQueryString () const;

    public:
        /**
         @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
        */
        nonvirtual void SetQueryString (const String& queryString);

    public:
        /**
         */
        nonvirtual String GetFragment () const;

    public:
        /**
         *  @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
         */
        nonvirtual void SetFragment (const String& frag);

    public:
        /**
         *   This is the host relative path, plus any optional query(string) plus any optional fragment
         *      path[?query][#fragment]
         *  This path does NOT include the leading / seperator
         *
         *  \note   terrible name but I can think of no better
         *
         *  @see GetHostRelativePath ()
         *  @see GetHostRelativePathPlusQuery ()
         */
        nonvirtual String GetHostRelURLString () const;

    public:
        /**
         *   This is the host relative path, plus any optional query(string) plus any optional fragment
         *      path[?query]
         *  This path does NOT include the leading / seperator
         *
         *  \note   terrible name but I can think of no better
         *
         *  @see GetHostRelURLString ()
         *  @see GetHostRelativePath ()
         */
        nonvirtual String GetHostRelativePathPlusQuery () const;

    public:
        /**
         *  For debugging purposes: don't count on the format.
         */
        nonvirtual String ToString () const;

    private:
        optional<String>   fScheme_; // aka protocol
        String             fHost_;
        optional<PortType> fPort_;
        String             fRelPath_;
        String             fQuery_;
        String             fFragment_;
    };

    /**
     */
    bool operator== (const URL::Authority::Host& lhs, const URL::Authority::Host& rhs);

    /**
     *  operator indirects to URL::Compare()
     */
    bool operator< (const URL& lhs, const URL& rhs);

    /**
     *  operator indirects to URL::Compare()
     */
    bool operator<= (const URL& lhs, const URL& rhs);

    /**
     *  operator indirects to URL::Equals ()
     */
    bool operator== (const URL& lhs, const URL& rhs);

    /**
     *  operator indirects to URL::Equals ()
     */
    bool operator!= (const URL& lhs, const URL& rhs);

    /**
     *  operator indirects to URL::Compare()
     */
    bool operator> (const URL& lhs, const URL& rhs);

    /**
     *  operator indirects to URL::Compare()
     */
    bool operator>= (const URL& lhs, const URL& rhs);

    /**
     *  Probably should define standard protos here - with named constants - like http/ftp/https etc
     */
    optional<uint16_t> GetDefaultPortForScheme (const String& proto);

    /**
     */
    class URLQuery {
    public:
        URLQuery (const string& query);
        URLQuery (const String& query);

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

    /**
     */
    class LabeledURL {
    public:
        LabeledURL (const URL& url = URL (), const String& label = String ());

    public:
        URL    fURL;
        String fLabel;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "URL.inl"

#endif /*_Stroika_Foundation_IO_Network_URL_h_*/
