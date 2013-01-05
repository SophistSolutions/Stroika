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


/*
 * TODO:
 *      o   Redo using Stroika String class
 *      o   Add overload for URL with map<> for query.
 *      o   Add PREDEFINED namespace and list predefined protocols
 *              HTTP
 *              HTTPS
 *              FTP
 *              FILE
 *          (only do with new stroika string class so we can use low-cost constants)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                const   int kDefaultPort    =   -1;

                using   Characters::String;

                // probably should define standard protos here - with named constnats - like http/ftp/https etc
                int     GetDefaultPortForProtocol (const String& proto);


                class   URL {
                public:
                    URL ();
                    explicit URL (const String& fullURL);
                    explicit URL (const String& protocol, const String& host, int portNumber = kDefaultPort, const String& relPath = String (), const String& query = String (), const String& fragment = String ());
                    explicit URL (const String& protocol, const String& host, const String& relPath, const String& query = String (), const String& fragment = String ());

                public:
                    // For example - the arg to GET in HTTP request - a regular URL but the part after the hostname
                    static  URL ParseHostRelativeURL (const String& w);

                public:
                    nonvirtual  bool    IsSecure () const;
                    nonvirtual  String  GetURL () const;
                    nonvirtual  String  GetHostRelPathDir () const;
                    nonvirtual  int     GetEffectivePortNumber () const;    // if port# not specified, returns detault given the protocol
                    nonvirtual  int     GetPortNumber () const;
                    nonvirtual  void    SetPortNumber (int portNum = kDefaultPort);

                public:
                    nonvirtual  void    clear ();
                    nonvirtual  bool    empty () const;

                public:
                    String fProtocol;
                    String fHost;
                    String fRelPath;
                    String fQuery;
                    String fFragment;
                private:
                    int     fPort;
                };
                bool    operator== (const URL& lhs, const URL& rhs);
                bool    operator!= (const URL& lhs, const URL& rhs);


                // See http://tools.ietf.org/html/rfc3986
                // This doesn't encode an entire URL, just a particular field
                string  EncodeURLQueryStringField (const String& s);

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



                class   LabeledURL {
                public:
                    LabeledURL (const String& url, const String& label = String ());

                public:
                    String fURL;
                    String fLabel;
                };

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_URL_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "URL.inl"
