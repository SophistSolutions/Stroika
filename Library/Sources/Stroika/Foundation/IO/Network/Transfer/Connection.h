/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_h_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Characters/String_Constant.h"
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
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *
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
     * TODO:
     *     @todo    Unclear about copyability - maybe if its a smartptr OK to copy - but would be copy-by-reference?
     *              Could be confusing! CONSIDER
     *
     *  \par Example Usage
     *      \code
     *          Connection  c   =   IO::Network::Transfer::CreateConnection ();
     *          c.SetURL (URI::Parse (L"http://www.google.com"));
     *          Response    r   =   c.GET ();
     *          Assert (r.GetSucceeded ());
     *          VerifyTestResult (r.GetData ().size () > 1);
     *      \endcode
     *
     */
    class Connection {
    protected:
        class _IRep;

    public:
        struct Options;

    protected:
        /**
         */
        Connection (const shared_ptr<_IRep>& rep);

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
        [[deprecated ("Since v2.1d27, use GetSchemeAndAuthority")]] URI GetURL () const;

    public:
        [[deprecated ("Since v2.1d27, use SetSchemeAndAuthority, or use the URL argument to GET/POST/etc")]] void SetURL (const URI& url);

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
         *  \req r.fAuthorityRelativeURL.GetAuthorityRelativeResource<URI> () == r.fAuthorityRelativeURL // MUST BE LEGIT authority-relative
         */
        nonvirtual Response Send (const Request& r);

    public:
        /*
         *  Simple wrapper on Send () for method GET
         *
         *  \par Example Usage
         *      \code
         *          Request r = conn.GET (URI{L"https://www.sophists.com/"});
         *          ...
         *      \endcode
         *
         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
         *          at e.GetResponse ()
         */
        nonvirtual Response GET (const URI& l, const Mapping<String, String>& extraHeaders = {});

    public:
        /*
         *  Simple wrapper on Send () for method POST
         *
         *  \par Example Usage
         *      \code
         *          Request r = conn.POST (URI{L"https://www.sophists.com/"}, data);
         *          ...
         *      \endcode
         *
         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
         *          at e.GetResponse ()
         */
        nonvirtual Response POST (const URI& l, const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

    public:
        /*
         *  Simple wrapper on Send () for method DELETE
         *
         *  \par Example Usage
         *      \code
         *          Request r = conn.Delete (URI{L"https://www.sophists.com/"});
         *          ...
         *      \endcode
         *
         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
         *          at e.GetResponse ()
         */
        nonvirtual Response DELETE (const URI& l, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

    public:
        /*
         *  Simple wrapper on Send () for method PUT
         *
         *  \par Example Usage
         *      \code
         *          Request r = conn.PUT (URI{L"https://www.sophists.com/"}, data);
         *          ...
         *      \endcode
         *
         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
         *          at e.GetResponse ()
         */
        nonvirtual Response PUT (const URI& l, const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

    public:
        /*
         *  Simple wrappers, with hardwired methods
         *
         *  \par Example Usage
         *      \code
         *          Request r = conn.OPTIONS (URI{L"https://www.sophists.com/"});
         *      \endcode
         *
         *  \note   This function only returns a Response on success. To see HTTP status response, catch (Exception e), and look
         *          at e.GetResponse ()
         */
        nonvirtual Response OPTIONS (const URI& l, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

    public:
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        [[deprecated ("Since v2.1d27, use GET (url - but can be relative url - relative to scehme/authority)")]] Response GET (const Mapping<String, String>& extraHeaders)
        {
            return GET (GetURL (), extraHeaders);
        }
        [[deprecated ("Since v2.1d27, use PUT (url - but can be relative url - relative to scehme/authority)")]] Response PUT (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
        {
            return PUT (GetURL (), data, contentType, extraHeaders);
        }
        [[deprecated ("Since v2.1d27, use OPTIONS (url - but can be relative url - relative to scehme/authority)")]] Response OPTIONS (const Mapping<String, String>& extraHeaders = Mapping<String, String> ())
        {
            return OPTIONS (GetURL (), extraHeaders);
        }
        [[deprecated ("Since v2.1d27, use POST (url - but can be relative url - relative to scehme/authority)")]] Response POST (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
        {
            return POST (GetURL (), data, contentType, extraHeaders);
        }
        [[deprecated ("Since v2.1d27, use DELETE (url - but can be relative url - relative to scehme/authority)")]] Response DELETE (const Mapping<String, String>& extraHeaders)
        {
            return DELETE (GetURL (), extraHeaders);
        }
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_END (4996);

    private:
        shared_ptr<_IRep> fRep_;
    };

    /**
     */
    struct Connection::Options {
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
         *  Disabled by default because curl and WinHTTP disable by default (they must have thought this out better than me).
         */
        unsigned int fMaxAutomaticRedirects{0};

        /**
         */
        String fUserAgent{L"Stroika/2.1"sv};

        /**
         */
        struct Authentication {
            /**
             *      eProactivelySendAuthentication requires fewer round-trips, and less resnding of data, but may not always work
             *      (e.g. if the auth requires server side information).
             *
             *      eRespondToWWWAuthenticate is more secure and widely applicable, but can be slower.
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
             *              Authentication (L"Mr-Smith", L"Super-Secret") is equivilent to curl --user Mr-Smith:Super-Secret URL
             *
             *      \note   For OAuth2:
             *              Authentication (L"OAuth <ACCESS_TOKEN>") is equivilent to curl -H "Authorization: OAuth <ACCESS_TOKEN>" URL
             *
             *      \note   For Bearer tokens:
             *              Authentication (L"Bearer <ACCESS_TOKEN>") is equivilent to curl -H "Authorization: Bearer <ACCESS_TOKEN>" URL
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

    /**
     */
    class Connection::_IRep {
    public:
        _IRep ()             = default;
        _IRep (const _IRep&) = delete;
        virtual ~_IRep ()    = default;

    public:
        nonvirtual _IRep& operator= (const _IRep&) = delete;

    public:
        virtual Options             GetOptions () const                                   = 0;
        virtual URI                 DeprecatedGetAuthorityRelativeURL () const            = 0;
        virtual void                DeprecatedSetAuthorityRelativeURL (const URI& url)    = 0;
        virtual URI                 GetSchemeAndAuthority () const                        = 0;
        virtual void                SetSchemeAndAuthority (const URI& schemeAndAuthority) = 0;
        virtual DurationSecondsType GetTimeout () const                                   = 0;
        virtual void                SetTimeout (DurationSecondsType timeout)              = 0;
        virtual void                Close ()                                              = 0;
        virtual Response            Send (const Request& r)                               = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_h_*/
