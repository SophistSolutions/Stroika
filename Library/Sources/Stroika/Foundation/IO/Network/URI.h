/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URI_h_
#define _Stroika_Foundation_IO_Network_URI_h_ 1

#include "../../StroikaPreComp.h"

#include <string>

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Mapping.h"
#include "InternetAddress.h"
#include "UniformResourceIdentification.h"

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

    class URL;

    /**
     *  A URI is EITHER a URL, or a relative reference to a URL. This definition is VERY MUCH LESS THAN CLEAR,
     *  but the closest I can infer from:
     *      https://tools.ietf.org/html/rfc3986
     *
     *          A URI can be further classified as a locator, a name, or both.  The
     *          term "Uniform Resource Locator" (URL) refers to the subset of URIs
     *          that, in addition to identifying a resource, provide a means of
     *          locating the resource by describing its primary access mechanism
     *          (e.g., its network "location").
     */
    class URI {
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
         *  This Requires() its arguments are valid and in range (where provided)
         *
         *  These will raise exceptions if anything illegal in the URL specification.
         */
        URI (const optional<SchemeType>& scheme = nullopt, const optional<Authority>& authority = nullopt, const String& path = String{}, const optional<String>& query = nullopt, const optional<String>& fragment = nullopt);
        URI (const URL& url);

    public:
        /**
         *  This takes argument string url, with possibly % encoded characters, accorind to https://tools.ietf.org/html/rfc3986
         */
        static URI Parse (const String& rawURL);

    public:
        /**
         *  This returns true if this is an absolute URL, and false if it is a relative URL
         *
         *  \par Example Usage
         *      \code
         *          if (u.IsURL ()) {
         *              URL url = u.As<URL> ();
         *          }
         *      \endcode
         */
        nonvirtual bool IsURL () const;

    public:
        /**
         *  Always returns a valid (or empty) protocol/URL scheme - according to http://www.ietf.org/rfc/rfc1738.txt
         */
        nonvirtual optional<SchemeType> GetScheme () const;

    public:
        /**
         *  The authority of a URI is basically the hostname (+ optional port and user info)
         */
        nonvirtual optional<Authority> GetAuthority () const;

    public:
        /*
         *  The path MAY or MAY NOT start with a /, and it may be empty.
         */
        nonvirtual String GetPath () const;

    public:
        /*
         */
        nonvirtual optional<String> GetQueryString () const;

    public:
        /**
         *  Supported conversion-targets (T):
         *      String - converts to the raw URI format (as it would appear in a web-browser or html link)
         *      URL - requires IsURL ()  - but then returns URL
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  For debugging purposes: don't count on the format.
         */
        nonvirtual String ToString () const;

    private:
        optional<String>    fScheme_;    // aka protocol
        optional<Authority> fAuthority_; // aka host+port+username
        String              fPath_;      // must read docs on combinng urls - but this maybe required (though can be empty)
        optional<String>    fQuery_;     // ditto
        optional<String>    fFragment_;  // ditto
    };

    template <>
    nonvirtual String URI::As () const;
    template <>
    nonvirtual URL URI::As () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "URI.inl"

#endif /*_Stroika_Foundation_IO_Network_URI_h_*/
