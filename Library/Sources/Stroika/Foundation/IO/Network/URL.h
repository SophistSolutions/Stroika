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
#include "UniformResourceIdentification.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    class URI;

    /**
     *  Class to help encode/decode the logical parts of an internet URL
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
     *
     * TODO:
     *      @todo   DOCUMENT and ENFORCE restrictions on values of query string, hostname, and protocol (eg. no : in protocol, etc)
     *
     *      @todo   Bake in as member the URLQuery object
     */
    class URL {
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
             &&& NO - INSTEAD - MAKE THIS DEPRECATED AND ADD NEW VALUE FOR eNormallyAsRFC...
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
        URL () = default;
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
        //[[deprecated(REPLACE WITH GetScheme () and GetScheme ()->IsSecure ()]]
        nonvirtual bool IsSecure () const;

    public:
        nonvirtual optional<Authority> GetAuthority () const;

    public:
        /**
         *  Returns a String representation of the fully qualified URL.
         */
        //[[deprecated(REPLACE WITH URI::As<String> ())]]
        nonvirtual String GetFullURL () const;

    public:
        /**
         *      If port# not specified, returns detault given the protocol.
         */
        //[[deprecated(REPLACE WITH URI::GetAuthority () && GetAuthority ().GetPort ().value_or (80))]]
        nonvirtual PortType GetPortValue (PortType defaultValue = 80) const;

    public:
        /**
         *  This value overrides the port# used by the protocol. This can be empty, implying the URL refers to the default
         *  port for the given URL scheme.
         *
         *  @see GetPortValue
         *  @see SetPortNumber
         */
        //[[deprecated(REPLACE WITH URI::GetAuthority () && GetAuthority ().GetPort ().value_or (80))]]
        nonvirtual optional<PortType> GetPortNumber () const;

    public:
        /**
         *  @see GetPortNumber ();
         */
        nonvirtual void SetPortNumber (const optional<PortType>& portNum = nullopt);

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
        //[[deprecated(REPLACE WITH use of optional)]]
        nonvirtual void clear ();

    public:
        /**
         *  This is the value created by clear or the no-arg CTOR.
         */
        //[[deprecated(REPLACE WITH use of optional)]]
        nonvirtual bool empty () const;

    public:
        /**
         *  Always returns a valid (or empty) protocol/URL scheme - according to http://www.ietf.org/rfc/rfc1738.txt
         */
        nonvirtual optional<SchemeType> GetScheme () const;

    public:
        /**
         */
        [[deprecated ("Use GetScheme instead (no value version cuz no good default)- as of 2.1d23")]] nonvirtual SchemeType GetSchemeValue () const;

    public:
        /**
         *  Since From http://www.ietf.org/rfc/rfc1738.txt suggests mapping upper case to lower case, this function does that.
         *  But other violations in the format of a protocol generate exceptions.
         */
        nonvirtual void SetScheme (const optional<SchemeType>& scheme);
        nonvirtual void SetScheme (const SchemeType& scheme);

    public:
        /**
            // @todo DEPRECATE THIS METHIOD - cuz unclear if returns encoded or unencoded host
         */
        //[[deprecated(REPLACE WITH URI GetAuthority () && GetAuthority ()->GetHost ().value_or (String{}))]]
        nonvirtual String GetHost () const;

    public:
        /**
            // @todo DEPRECATE THIS METHIOD - cuz unclear if returns encoded or unencoded host
         @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
            */
        //[[deprecated(REPLACE WITH URI TBD)]]
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
        //[[deprecated(REPLACE WITH URI TBD - close to GetPath)]]
        nonvirtual String GetHostRelativePath () const;

    public:
        /**
         *  @see GetHostRelativePath for format restrictions.
         */
        //[[deprecated(REPLACE WITH URI TBD - close to SetPath)]]
        nonvirtual void SetHostRelativePath (const String& hostRelativePath);

    public:
        /**
         *   @see GetHostRelativePath for format restrictions. This can be empty.
         */
        //[[deprecated("deprecated in v2.1d23 - use URI : GetAuthorityRelativeResourceDir instead")]]
        nonvirtual String GetHostRelPathDir () const;

    public:
        /**
         */
        nonvirtual Query GetQuery () const;

    public:
        /**
         */
        nonvirtual void SetQuery (const Query& query);

    public:
        /**
         *  \note   This returns an empty string if no query.
         */
        //[[deprecated(REPLACE WITH URI TBD - close to GetQyery<String>???)]]
        nonvirtual String GetQueryString () const;

    public:
        /**
         @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
        */
        //[[deprecated(REPLACE WITH URI SetQuery)]]
        nonvirtual void SetQueryString (const String& queryString);

    public:
        /**
         */
        //[[deprecated(same as class URI, exeption returns optional<String> - so diff for the case of empty string versus empty optional)]]
        nonvirtual String GetFragment () const;

    public:
        /**
         *  @todo - smae thing we did for protocol/scjema - add type and documetn restrictions on that tyep and then enforce here! (exceptions)
         */
        //[[deprecated(same as class URI, exeption returns optional<String> - so diff for the case of empty string versus empty optional)]]
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
        //[[deprecated(USE URI - METHOD TBD)]]
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
        //[[deprecated("USE URI - METHOD GetAuthorityRelativeResource")]]
        nonvirtual String GetHostRelativePathPlusQuery () const;

    public:
        /**
         *  For debugging purposes: don't count on the format.
         */
        nonvirtual String ToString () const;

    private:
        optional<SchemeType> fScheme_; // aka protocol
        Authority            fAuthority_;
        String               fRelPath_;
        String               fQuery_;
        String               fFragment_;
    };

    /**
     *  operators indirects to URL::Compare()
     */
    bool operator< (const URL& lhs, const URL& rhs);
    bool operator<= (const URL& lhs, const URL& rhs);
    bool operator== (const URL& lhs, const URL& rhs);
    bool operator!= (const URL& lhs, const URL& rhs);
    bool operator> (const URL& lhs, const URL& rhs);
    bool operator>= (const URL& lhs, const URL& rhs);

    using URLQuery [[deprecated ("Since 2.1d23, use UniformResourceIdentification::Query)")]] = UniformResourceIdentification::Query;

    /**
     */
    class [[deprecated ("Since 2.1d23, too simple (and due to switch to uri not worth maintaining)")]] LabeledURL
    {
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
