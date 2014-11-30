/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_h_
#define _Stroika_Foundation_IO_Network_URL_h_   1

#include    "../../StroikaPreComp.h"

#include    <string>

#include    "../../Characters/String.h"
#include    "../../Containers/Mapping.h"
#include    "../../Configuration/Common.h"
#include    "../../Memory/Optional.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   Use "Value" style hack to deal with optional stuff, and use more of Optional class, at least in API.
 *              That way - we capture just what the user said, and only when he askes for info, do we use the Value() variant
 *              that plugs in defaults.
 *
 *      @todo   RENAME "Protocol" to "Scheme"
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



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                /**
                 *  Probably should define standard protos here - with named constants - like http/ftp/https etc
                 */
                uint16_t     GetDefaultPortForProtocol (const String& proto);


                /**
                 */
                class   URLQuery {
                public:
                    URLQuery (const string& query);
                    URLQuery (const String& query);

                public:
                    nonvirtual  const Containers::Mapping<String, String>& GetMap () const;

                public:
                    nonvirtual  String operator () (const string& idx) const;
                    nonvirtual  String operator () (const String& idx) const;

                public:
                    nonvirtual  bool    HasField (const string& idx) const;
                    nonvirtual  bool    HasField (const String& idx) const;

                public:
                    nonvirtual  void    AddField (const String& idx, const String& value);

                public:
                    nonvirtual  void    RemoveFieldIfAny (const string& idx);
                    nonvirtual  void    RemoveFieldIfAny (const String& idx);

                public:
                    // Return wide string, but all ascii characters
                    // http://tools.ietf.org/html/rfc3986
                    nonvirtual  String ComputeQueryString () const;

                private:
                    Containers::Mapping<String, String>    fMap_;
                };


                /**
                 *  Class to help encode/decode the logical parts of an internet URL
                 *
                 *  @see http://www.ietf.org/rfc/rfc1738.txt
                 *
                 * TODO:
                 *      @todo   DOCUMENT and ENFORCE restrictions on values of query string, hostname, and protocol (eg. no : in protocol, etc)
                 *
                 *      @todo   Bake in as member the URLQuery object
                 */
                class   URL {
                public:
                    /**
                     *  From http://www.ietf.org/rfc/rfc1738.txt (section 2.1. The main parts of URLs)
                     *
                     *  Scheme names consist of a sequence of characters. The lower case
                     *  letters "a"--"z", digits, and the characters plus ("+"), period
                     *  ("."), and hyphen ("-") are allowed. For resiliency, programs
                     *  interpreting URLs should treat upper case letters as equivalent to
                     *  lower case in scheme names (e.g., allow "HTTP" as well as "http").
                     *
                     *  AKA PROTOCOL.
                     */
                    using   SchemeType  =   String;

                public:
                    /**
                     */
                    using   PortType    =   uint16_t;

                public:
                    /**
                     *      @todo PRELIM!!!!
                     *              PROABBLY NEED "FORCE_AS_GOOD_VALID_FULL_URL
                     *              MAYBE FULL_OR_RELATIVE (as in webpage urls)
                     *
                     *      Maybe URL has method "IsRelative"?
                     */
                    enum    ParseOptions {
                        /*
                         *  Check for strict url conformance (scheme : data, or http://host/REL)
                         *  Throws if not http://www.ietf.org/rfc/rfc1738.txt conformant.
                         *
                         *  (OR MAYBE USE https://tools.ietf.org/html/rfc3986
                         *      and see about whcih part - full url)
                         */
                        eAsFullURL,

                        /*
                         *  ???http://tools.ietf.org/html/rfc1808
                         *
                         *  ( MAYBE USE https://tools.ietf.org/html/rfc3986
                         *      and see about whcih part - full url)
                         */
                        eAsRelativeURL,

                        /**
                         */
                        eFlexiblyAsUI,

                        /**
                         *  @todo CLEANUP/FIX
                         *
                         *  I THINK The only quirk of this node is that
                         *      dyn:/foo.html gets parsed as HOST=EMPTY; relativeURL= foo.html; That makes sense
                         *      but doesnt apper to conform to spec for urls (rfs) above. Research/analyze...
                         */
                        eStroikaPre20a50BackCompatMode,
                    };

                public:
                    /**
                     *  See SetScheme () for handling of the 'protocol' parameter.
                     *  See SetQuery() for setting the query parameter.
                     *  See SetHostRelativePath for the 'relPath' restrictions.
                     *  This Requires() its arguments are valid and in range. use
                     *
                     *  EXCEPT for the fullURL CTOR. This will raise exceptions if anything illegal in the URL specification.
                     */
                    URL ();
                    URL (const SchemeType& protocol, const String& host, Memory::Optional<PortType> portNumber = Memory::Optional<PortType> (), const String& relPath = String (), const String& query = String (), const String& fragment = String ());
                    URL (const SchemeType& protocol, const String& host, const String& relPath, const String& query = String (), const String& fragment = String ());
                    URL (const String& urlText, ParseOptions po);

                public:
                    /**
                     */
                    static  URL Parse (const String& urlText, ParseOptions po = eAsFullURL);

                private:
                    static  URL ParseHosteStroikaPre20a50BackCompatMode_ (const String& w);
                    static  URL ParseHostRelativeURL_ (const String& w);

                public:
                    /**
                     *  Returns if the URL protocol is secure (SSL-based).
                     */
                    nonvirtual  bool    IsSecure () const;

                public:
                    /**
                     *  Returns a String representation of the fully qualified URL.
                     */
                    nonvirtual  String  GetFullURL () const;

                public:
                    /**
                     *      If port# not specified, returns detault given the protocol.
                     */
                    nonvirtual  PortType     GetPortValue () const;

                public:
                    _DeprecatedFunction_ (inline  PortType  GetEffectivePortNumber () const, "Instead use GetPortValue() - to be removed after v2.0a55")
                    {
                        return GetPortValue ();
                    }

                public:
                    /**
                     *  This value overrides the port# used by the protocol. This can be empty, implying the URL refers to the default
                     *  port for the given URL scheme.
                     *
                     *  @see GetPortValue
                     *  @see SetPortNumber
                     */
                    nonvirtual  Memory::Optional<PortType>     GetPortNumber () const;

                public:
                    /**
                     *  @see GetPortNumber ();
                     */
                    nonvirtual  void    SetPortNumber (const Memory::Optional<PortType>& portNum = Memory::Optional<PortType> ());

                public:
                    /**
                     *  NYI
                     *
                     *  Presume the given url is the owning webpage. What is the print string for this relative url.
                     */
                    nonvirtual  String  GetRelativeURL (const URL& baseURL) const;

                public:
                    /**
                     *  this does not do a field by field compare. It returns true iff GetFullURL() on lhs / rhs would return the
                     *  same string
                     *
                     *  @todo   (modulo what should be compared canse instaitivive???) - hostname?
                     *          PROTOCOL SB case INsensitiatve, and if there is a hostame, that should be comapred case insenswative. But rest is case sensative.
                     */
                    nonvirtual  bool    Equals (const URL& rhs) const;

                public:
                    /**
                     */
                    nonvirtual  void    clear ();

                public:
                    /**
                     *  This is the value created by clear or the no-arg CTOR.
                     */
                    nonvirtual  bool    empty () const;

                public:
                    /**
                     *  Always returns a valid (or empty) protocol/URL scheme - according to http://www.ietf.org/rfc/rfc1738.txt
                     */
                    nonvirtual  SchemeType  GetScheme () const;

                public:
                    /**
                     *  Since From http://www.ietf.org/rfc/rfc1738.txt suggests mapping upper case to lower case, this function does that.
                     *  But other violations in teh format of a protocol generate exceptions.
                     */
                    nonvirtual  void    SetScheme (const SchemeType& scheme);

                public:
                    _DeprecatedFunction_ (inline  SchemeType  GetProtocol () const, "Instead use GetScheme() - to be removed after v2.0a55")
                    {
                        return GetScheme();
                    }
                    _DeprecatedFunction_ (inline  void    SetProtocol (const SchemeType& protocol), "Instead use SetScheme() - to be removed after v2.0a55")
                    {
                        SetScheme (protocol);
                    }

                public:
                    /**
                     */
                    nonvirtual  String  GetHost () const;

                public:
                    /**
                     @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
                     */
                    nonvirtual  void    SetHost (const String& host);

                public:
                    /**
                     *  Note this does NOT contain the '/' separating the hostname from the relative path,
                     *  so it typically doesnt start with a '/', but frequently contains them. It MAY legally
                     *  start with a '/' because http://www.ietf.org/rfc/rfc1738.txt doesnt specifically prohibit this.
                     */
                    nonvirtual  String  GetHostRelativePath () const;

                public:
                    /**
                     *  @see GetHostRelativePath for format restrictions.
                     */
                    nonvirtual  void    SetHostRelativePath (const String& hostRelativePath);

                public:
                    /**
                    *   @see GetHostRelativePath for format restrictions. This can be empty.
                    */
                    nonvirtual  String  GetHostRelPathDir () const;

                public:
                    /**
                     */
                    nonvirtual  URLQuery  GetQuery () const;

                public:
                    /**
                     */
                    nonvirtual  void    SetQuery (const URLQuery& query);

                public:
                    /**
                     */
                    nonvirtual  String  GetQueryString () const;

                public:
                    /**
                     @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
                     */
                    nonvirtual  void    SetQueryString (const String& queryString);

                public:
                    /**
                     */
                    nonvirtual  String  GetFragment () const;
                public:
                    /**
                     @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
                     */
                    nonvirtual  void    SetFragment (const String& frag);

                public:
                    nonvirtual  bool    operator== (const URL& rhs) const;
                    nonvirtual  bool    operator!= (const URL& rhs) const;

                private:
                    String      fProtocol_;
                    String      fHost_;
                    PortType    fPort_;
                    String      fRelPath_;
                    String      fQuery_;
                    String      fFragment_;

                private:
                    DEFINE_CONSTEXPR_CONSTANT (PortType, kDefaultPortSentinal_, -1);    // sentinal value a more efficient rep than Optional<>
                };


                // See http://tools.ietf.org/html/rfc3986
                // This doesn't encode an entire URL, just a particular field
                string  EncodeURLQueryStringField (const String& s);


                class   LabeledURL {
                public:
                    LabeledURL (const URL& url = URL (), const String& label = String ());

                public:
                    URL     fURL;
                    String  fLabel;
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "URL.inl"

#endif  /*_Stroika_Foundation_IO_Network_URL_h_*/
