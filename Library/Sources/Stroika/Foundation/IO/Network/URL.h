/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_h_
#define _Stroika_Foundation_IO_Network_URL_h_   1

#include    "../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   major cleanup of URL class needed - started process of making
 *              some stuff private - but a long way to go
 *      @todo   Redo using Stroika String class and Mapping<>
 *      @todo   Add overload for URL with map<> for query.
 *      @todo   Add PREDEFINED namespace and list predefined protocols
 *              o   HTTP
 *              o   HTTPS
 *              o   FTP
 *              o   FILE
 *          (only do with new stroika string class so we can use low-cost constants)
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
                ***TODO: RENAME URLQuery
                 */
                class   URLQueryDecoder {
                public:
                    URLQueryDecoder (const string& query);
                    URLQueryDecoder (const String& query);

                public:
                    nonvirtual  const map<String, String>& GetMap () const;

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
                    map<String, String>    fMap;
                };


                /**
                 *  Class to help encode/decode the logical parts of an internet URL
                 *
                 *  @see http://www.ietf.org/rfc/rfc1738.txt
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
                     */
                    nonvirtual  int     GetPortNumber () const;

                public:
                    /**
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
                    String  GetProtocol () const {
                        return fProtocol;
                    }
                public:
                    void    SetProtocol (const String& protocol) {
                        fProtocol = protocol;
                    }

                public:
                    String  GetHost () const {
                        return fHost;
                    }
                public:
                    void    SetHost (const String& host) {
                        fHost = host;
                    }

                public:
                    String  GetHostRelativePath () const {
                        return fRelPath;
                    }
                public:
                    void    SetHostRelativePath (const String& hostRelativePath) {
                        fRelPath = hostRelativePath;
                    }

                public:
                    String  GetQueryString () const {
                        return fQuery;
                    }
                public:
                    void    SetQueryString (const String& queryString) {
                        fQuery = queryString;
                    }

                public:
                    String  GetFragment () const {
                        return fFragment;
                    }
                public:
                    void    SetFragment (const String& frag) {
                        fFragment = frag;
                    }

                public:
                private:
                    /// TODO
                    //  @todo -make private and proivide accessors
                    // for Query - consider haivng URLQueryDecoder as value/...
                    String fProtocol;
                    String fHost;
                    String fRelPath;
                    String fQuery;
                    String fFragment;

                public:
                    nonvirtual  bool    operator== (const URL& rhs) const;
                    nonvirtual  bool    operator!= (const URL& rhs) const;

                private:
                    int     fPort;      // PERHPAS SHOULD USE OPTIONAL FOR THIS!!!
                };


                // See http://tools.ietf.org/html/rfc3986
                // This doesn't encode an entire URL, just a particular field
                string  EncodeURLQueryStringField (const String& s);


                class   LabeledURL {
                public:
                    LabeledURL (const URL& url, const String& label = String ());

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
