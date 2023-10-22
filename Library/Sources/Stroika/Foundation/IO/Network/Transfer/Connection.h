/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_h_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Mapping.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Memory/BLOB.h"
#include "../../../Time/Realtime.h"

#include "../HTTP/Status.h"

#include "../ConnectionOrientedStreamSocket.h"
#include "../URI.h"

#include "Cache.h"
#include "Exception.h"
#include "Request.h"
#include "Response.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 * TODO:
 *      @todo   FIXUP Docs and code so on thrown response, an open stream in the response is replaced by
 *              a FULLY REALIZED STREAM (MEMORY STREAM)
 *
 *      @todo   Probably should redo Request so it can optionally use a BLOB or
 *              Stream (like Response). DO NOW the header / class changes - making
 *              CTOR/accessors, so easiser to change functionality later!!!
 *
 *      @todo   Add thread safety (locks/semaphores)
 *
 *                  Decide on and DOCUMENT threading policy. For example - do we need
 *                  locks internally in the connection object or DEFINE that its
 *                  the callers resposabiltiy. PROBABLY best to do in the Connection object itself?
 *
 *                  ADD CRITICAL SECTIONS!!! - or DOCUMENT CALLERS REPSONABILTY
 *
 *      @todo   Add (optionally callable) Connect() method. Send etc connect on-demand as needed
 *              but sometimes its useful to pre-create connections (to reduce latnecy).
 *
 *      @todo   Add factory for 'CreateConnection'  - so you can do 'dependnecy injection' or other
 *              way to configure http client support library (winhttp versus libcurl or other).
 *
 *      @todo   Redo Response to fully/properly support incremental read through streams. Must do
 *              CTOR on response object taking a stream, and then reasonably (tbd) how to handle
 *              calls to getResponseBLOB? (probably assert or except?)
 *
 *      @todo   Progress Callbacks?
 *
 *      @todo   Add Client side certs
 *
 *      @todo   Refine server-side-cert checking mechanism (review LIBCURL to see what makes sense)
 *              PROBABLY just a callback mechanism - with a few default callabcks you can plugin (like reject bad certs)
 *              MAYBE just add FLAG saying whether to VALIDATE_HOST? Maybe callback API"?
 *
 *                        //  This COULD be expanded to include either a BOOL to CHECK remote SSL info, or a callback (which can throw)
 *                        //  if invalid SSL INFO from server side. Thats probably best. callback whcih throws, and set it to such a callback by default!
 *                        //      -- LGP 2012-06-26
 */

namespace Stroika::Foundation::IO::Network::Transfer {

//avoid windows header clash...
#if qPlatform_Windows
#undef DELETE
#endif

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Time::DurationSecondsType;

    /**
     *  \par Example Usage
     *      \code
     *          Connection::Ptr c   =   IO::Network::Transfer::CreateConnection ();
     *          Response        r   =   c.GET (URI {L"http://www.google.com"});
     *          Assert (r.GetSucceeded ());
     *          VerifyTestResult (r.GetData ().size () > 1);
     *      \endcode
     *
     */
    namespace Connection {

        /**
         */
        struct Options {
            /**
             */
            bool fReturnSSLInfo{false};

            /**
             *  fFailConnectionIfSSLCertificateInvalid could be because of expired CERT, or because of non-matching
             *  host.
             *
             *      \note - we may want to do something more sophisticated, like a callback with stats about the remote side
             *              and allow that to throw, indicating rejection. BUt this is good enuf for now.
             *
             *      \note - fFailConnectionIfSSLCertificateInvalid, because it may default on or off depending on the
             *              the underlying http client software. For example, with embedded apps, and openssl, we may not
             *              have access to a reliable certs file.
             *
             *              Specify it explicitly if you care.
             */
            optional<bool> fFailConnectionIfSSLCertificateInvalid;

            /**
             */
            bool fAssumeLowestCommonDenominatorHTTPServer{false};

            /**
             */
            bool fSupportSessionCookies{true};

            /**
             *  If true, the Execution::DeclareActivity API will be used during some selected operations which will produce
             *  more detailed exception messages (mostly include url) when exceptions happen.
             */
            optional<bool> fDeclareActivities{};

            /**
             *  Set to 0 to disable automatic redirects. 10 is a reasonable number if you allow auto-redirects (windows default).
             *  
             *  Set to 1 by default (a change from 0 before Stroika 2.1a5) because a single redirect can
             *  make sense, and a typical fetcher would want to see the redirection. Most malicious or
             *  broken cases involve more redirects.
             */
            unsigned int fMaxAutomaticRedirects{1};

            /**
             */
            String fUserAgent{"Stroika/3.0"sv};

            /**
             */
            struct Authentication {
                /**
                 *      eProactivelySendAuthentication requires fewer round-trips, and less resnding of data, but may not always work
                 *      (e.g. if the auth requires server side information).
                 *
                 *      eRespondToWWWAuthenticate is more secure and widely applicable, but can be slower.
                 *
                 *  \note   Configuration::DefaultNames<> supported
                 */
                enum class Options {
                    eProactivelySendAuthentication,
                    eRespondToWWWAuthenticate,

                    eDEFAULT = eRespondToWWWAuthenticate,

                    Stroika_Define_Enum_Bounds (eProactivelySendAuthentication, eRespondToWWWAuthenticate)
                };

            public:
                /**
                 *  If the constructor with an authToken is specified, we automatically use eProactivelySendAuthentication.
                 *
                 *      \note   digest/basic/etc - normal username/password:
                 *              Authentication{L"Mr-Smith", L"Super-Secret"} is equivalent to curl --user Mr-Smith:Super-Secret URL
                 *
                 *      \note   For OAuth2:
                 *              Authentication{"OAuth <ACCESS_TOKEN>"} is equivalent to curl -H "Authorization: OAuth <ACCESS_TOKEN>" URL
                 *
                 *      \note   For Bearer tokens:
                 *              Authentication{"Bearer <ACCESS_TOKEN>"} is equivalent to curl -H "Authorization: Bearer <ACCESS_TOKEN>" URL
                 */
                Authentication () = delete;
                Authentication (const String& authToken);
                Authentication (const String& username, const String& password, Options options = Options::eDEFAULT);

            public:
                /**
                 */
                nonvirtual Options GetOptions () const;

            public:
                /**
                 *      return engaged optional iff constructed with a username/password.
                 */
                nonvirtual optional<pair<String, String>> GetUsernameAndPassword () const;

            public:
                /**
                 * Return the parameter to the HTTP Authorization header. So for example, if you provided a username/password, this
                 * might return (from https://tools.ietf.org/html/rfc2617#section-2) Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
                 *
                 * This is not generally very useful, except if you've constructed the authorization with an explicit auth token, or when using 
                 * eProactivelySendAuthentication.
                 */
                nonvirtual String GetAuthToken () const;

            public:
                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;

            private:
                Options                        fOptions_;
                optional<String>               fExplicitAuthToken_;
                optional<pair<String, String>> fUsernamePassword_;
            };

            /**
             *  If authentication options are missing, no authentication will be performed/supported, and if the remote HTTP server
             *      requires authentication, and 401 HTTP exception will be thrown.
             */
            optional<Authentication> fAuthentication;

            /*
             * This is NOT to be confused with HTTP keep alives, but refers to the TCP transport layer variety. This only
             * matters for long connections
             */
            optional<IO::Network::ConnectionOrientedStreamSocket::KeepAliveOptions> fTCPKeepAlives;

            /**
             *  The cache object defaults to nullptr, but to have cached HTTP GETs, create a static Options object and have
             *  that object contain the Cache object reference, and then any Connections you create from that options
             *  object will share the same cache.
             */
            Cache::Ptr fCache{nullptr};
        };

        class IRep;

        /**
         *  A Connection::Ptr is a smart 'shared_ptr' to a connection object.
         */
        class Ptr {
        public:
            /**
             *  \req rep != nullptr
             */
            Ptr (const shared_ptr<IRep>& rep);

        public:
            /**
             * Send should timeout after this amount of time. Note - the initial Send may do
             * much more work (nslookup and tcp connect) than subsequent ones, and this same timeout is used for the combined time.
             */
            nonvirtual DurationSecondsType GetTimeout () const;

        public:
            /**
             */
            nonvirtual void SetTimeout (DurationSecondsType timeout);

        public:
            /**
             */
            nonvirtual Options GetOptions () const;

        public:
            /**
             *  Returns a URI with only the scheme/authority part set (the part defining the connection)
             *      \ens (url.GetSchemeAndAuthority () == url);
             */
            nonvirtual URI GetSchemeAndAuthority () const;

        public:
            /**
             *  Set a URI with only the scheme/authority part set (the part defining the connection)
             *      \req (url.GetSchemeAndAuthority () == url);
             */
            nonvirtual void SetSchemeAndAuthority (const URI& url);

        public:
            /**
             *  force closed Connection. Can still call Send again, but that autocreates new Connection
             */
            nonvirtual void Close ();

        public:
            /*
             *  This returns a response object, which possibly contains an http error.
             *
             *  Call this->SetSchemeAndAuthority() first, or use a wrapper let GET/POST that does this automatically.
             *
             *  \par Example Usage
             *      \code
             *          if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
             *              SetSchemeAndAuthority (schemeAndAuthority);
             *          }
             *          Response r = conn.Send (Request (...));
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \note   We considered having 1xx and 3xx responses not throw. However, they are generally fairly rare, and mostly
             *          need to be treated like errors anyhow (cannot just read response) - so best to have this as the behavior, and catch
             *          if you want to handle 300. The ONLY exception to this might be in caching, when you get a NOT-MODIFIED. We MAY
             *          want to somehow reconsider that. But its simpler - at least for now - to treat these all uniformly.
             *
             *  \ensure (r.GetSucceeded());
             *
             *  \req r.fAuthorityRelativeURL.GetAuthorityRelativeResource<URI> () == r.fAuthorityRelativeURL // MUST BE LEGIT authority-relative
             */
            nonvirtual Response Send (const Request& r);

        public:
            /*
             *  Simple wrapper on Send () for method GET
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.GET (URI{"https://www.sophists.com/"});
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ().
             *
             *  \ensure (r.GetSucceeded());
             */
            nonvirtual Response GET (const URI& l, const Mapping<String, String>& extraHeaders = {});

        public:
            /*
             *  Simple wrapper on Send () for method POST
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.POST (URI{"https://www.sophists.com/obj/"}, data);
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \ensure (r.GetSucceeded());
             */
            nonvirtual Response POST (const URI& l, const BLOB& data, const InternetMediaType& contentType,
                                      const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

        public:
            /*
             *  Simple wrapper on Send () for method PATCH
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.PATCH (URI{"https://www.sophists.com/obj/"}, data);
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \ensure (r.GetSucceeded());
             *
             *  \note see https://tools.ietf.org/html/rfc5789 for docs on Patch
             */
            nonvirtual Response PATCH (const URI& l, const BLOB& data, const InternetMediaType& contentType,
                                       const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

        public:
            /*
             *  Simple wrapper on Send () for method DELETE
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.Delete (URI{"https://www.sophists.com/obj/{id}"});
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \ensure (r.GetSucceeded());
             */
            nonvirtual Response DELETE (const URI& l, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

        public:
            /*
             *  Simple wrapper on Send () for method PUT
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.PUT (URI{"https://www.sophists.com/obj/{id}"}, data);
             *          ...
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \ensure (r.GetSucceeded());
             */
            nonvirtual Response PUT (const URI& l, const BLOB& data, const InternetMediaType& contentType,
                                     const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

        public:
            /*
             *  Simple wrapper on Send() for the HTTP OPTIONS message
             *
             *  \par Example Usage
             *      \code
             *          Request r = conn.OPTIONS (URI{"https://www.sophists.com/"});
             *      \endcode
             *
             *  \note   This function only returns a Response on success. To see HTTP status response, catch (Exception e), and look
             *          at e.GetResponse ()
             *
             *  \ensure (r.GetSucceeded());
             */
            nonvirtual Response OPTIONS (const URI& l, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

        private:
            shared_ptr<IRep> fRep_;
        };

        /**
         */
        class IRep {
        public:
            IRep ()            = default;
            IRep (const IRep&) = delete;
            virtual ~IRep ()   = default;

        public:
            nonvirtual IRep& operator= (const IRep&) = delete;

        public:
            virtual Options             GetOptions () const                                   = 0;
            virtual URI                 GetSchemeAndAuthority () const                        = 0;
            virtual void                SetSchemeAndAuthority (const URI& schemeAndAuthority) = 0;
            virtual DurationSecondsType GetTimeout () const                                   = 0;
            virtual void                SetTimeout (DurationSecondsType timeout)              = 0;
            virtual void                Close ()                                              = 0;
            virtual Response            Send (const Request& r)                               = 0;
        };

        /**
         * Simple connection factory object. If you don't care what backend to use for remote connections, use this API
         * to construct an unconnected object.
         *
         *  \par Example Usage
         *      \code
         *          Connection  c   =   IO::Network::Transfer::CreateConnection ();
         *          Response    r   =   c.GET (IO::Network::URI{"http://www.google.com"});
         *          Assert (r.GetSucceeded ()); // else it would have thrown
         *          String result = r.GetDataTextInputStream ().ReadAll ();
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          auto&&             connection = IO::Network::Transfer::New ();
         *          auto&&             response = connection.GET (IO::Network::URI{"http://myexternalip.com/raw"});
         *          nw.fExternalIPAddress = IO::Network::InternetAddress{response.GetDataTextInputStream ().ReadAll ()};
         *      \endcode
         */
        Ptr New ();
        Ptr New (const Options& options);

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_h_*/
