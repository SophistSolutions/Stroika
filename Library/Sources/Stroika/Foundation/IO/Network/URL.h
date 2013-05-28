/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_h_
#define _Stroika_Foundation_IO_Network_URL_h_   1

#include    "../../StroikaPreComp.h"

#include    <string>

#include    "../../Characters/String.h"
#include    "../../Containers/Mapping.h"
#include    "../../Configuration/Common.h"



/**
 *  \file
 *
 * TODO:
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
 *      @todo   Current implementation contains Windows code for assertion checking.
 *              Instead - move that to a regression test suite and pepper it with lots
 *              of tests. That cleanups up our portable implemenation (but requires lots
 *              of reg-tests tests!).
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                // TODO - probably use OPTIOINAL instead of a sentinal value!!!
                // OR - BETTER - USE SETINAL INTERNALLY - as IMPL DETIAL - but change public API to use Optional!!!! Better API, and
                // then basically the same perforamncnce...
                const   int kDefaultPort    =   -1;


                using   Characters::String;


                /**
                 * probably should define standard protos here - with named constnats - like http/ftp/https etc
                 */
                int     GetDefaultPortForProtocol (const String& proto);


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

                    nonvirtual  bool    HasField (const string& idx) const;
                    nonvirtual  bool    HasField (const String& idx) const;

                    nonvirtual  void    AddField (const String& idx, const String& value);

                    nonvirtual  void    RemoveFieldIfAny (const string& idx);
                    nonvirtual  void    RemoveFieldIfAny (const String& idx);

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
                    URL ();
                    explicit URL (const String& fullURL);
                    explicit URL (const String& protocol, const String& host, int portNumber = kDefaultPort, const String& relPath = String (), const String& query = String (), const String& fragment = String ());
                    explicit URL (const String& protocol, const String& host, const String& relPath, const String& query = String (), const String& fragment = String ());

                public:
                    /**
                    *   TODO - DOCUMENT MUCH BETTER - I DONT UNDERSNTADN THIS!!!
                    *
                    // For example - the arg to GET in HTTP request - a regular URL but the part after the hostname
                    */
                    static  URL ParseHostRelativeURL (const String& w);

                public:
                    /**
                     *  Returns if the URL protocol is secure (SSL-based).
                     */
                    nonvirtual  bool    IsSecure () const;

                public:
                    /**
                     *  Returns a fully String representation of the fully qualified URL.
                     */
                    nonvirtual  String  GetFullURL () const;

                public:
                    /**
                     */
                    nonvirtual  String  GetHostRelPathDir () const;

                public:
                    /**
                     */
                    nonvirtual  int     GetEffectivePortNumber () const;    // if port# not specified, returns detault given the protocol

                public:
                    /**
                    *** CHANGE TO USE OPTIONAL API
                     */
                    nonvirtual  int     GetPortNumber () const;

                public:
                    /**
                    *** CHANGE TO USE OPTIONAL API
                     */
                    nonvirtual  void    SetPortNumber (int portNum = kDefaultPort);

                public:
                    /**
                     *  NYI
                     *
                     *  Presume the given url is the owning webpage. What is the print string for this relative url.
                     */
                    nonvirtual  String  GetRelativeURL (const URL& baseURL) const;

                public:
                    /**
                     */
                    nonvirtual  bool    Equals (const URL& rhs) const;

                public:
                    /**
                     */
                    nonvirtual  void    clear ();

                public:
                    /**
                     */
                    nonvirtual  bool    empty () const;

                public:
                    /**
                     */
                    String  GetProtocol () const;

                public:
                    /**
                     */
                    void    SetProtocol (const String& protocol);

                public:
                    /**
                     */
                    String  GetHost () const;

                public:
                    /**
                     */
                    void    SetHost (const String& host);

                public:
                    /**
                     */
                    String  GetHostRelativePath () const;

                public:
                    /**
                     */
                    void    SetHostRelativePath (const String& hostRelativePath);

                public:
                    /**
                     */
                    URLQuery  GetQuery () const;

                public:
                    /**
                     */
                    void    SetQuery (const URLQuery& query);

                public:
                    /**
                     */
                    String  GetQueryString () const;

                public:
                    /**
                     */
                    void    SetQueryString (const String& queryString);

                public:
                    /**
                     */
                    String  GetFragment () const;
                public:
                    /**
                     */
                    void    SetFragment (const String& frag);

                public:
                    nonvirtual  bool    operator== (const URL& rhs) const;
                    nonvirtual  bool    operator!= (const URL& rhs) const;

                private:
                    String fProtocol_;
                    String fHost_;
                    String fRelPath_;
                    String fQuery_;
                    String fFragment_;

                private:
                    int     fPort_;      // PERHPAS SHOULD USE OPTIONAL FOR THIS!!!
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
