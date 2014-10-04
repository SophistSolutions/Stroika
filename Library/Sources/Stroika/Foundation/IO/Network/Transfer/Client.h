/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_h_   1

#include    "../../../StroikaPreComp.h"

#include    <string>

#include    "../../../Characters/String.h"
#include    "../../../Configuration/Common.h"
#include    "../../../Containers/Mapping.h"
#include    "../../../DataExchange/InternetMediaType.h"
#include    "../../../Memory/BLOB.h"
#include    "../../../Memory/Optional.h"
#include    "../URL.h"
#include    "../HTTP/Status.h"
#include    "../../../Time/Realtime.h"



/**
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 *
 * TODO:
 *
 *      @todo   tests with curl http://httpbin.org/ip
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
 *      @todo   Progress Callbacks?
 *
 *      @todo   Redo response / src API using streams?
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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {


//avoid windows header clash...
#undef DELETE


                    using   Characters::String;
                    using   Containers::Mapping;
                    using   DataExchange::InternetMediaType;
                    using   Memory::BLOB;
                    using   Memory::Byte;
                    using   Memory::Optional;
                    using   Time::DurationSecondsType;


                    /**
                     *  \note  DESIGN-NOTE:
                     *      Chose not move request/repsonse stuff to HTTP module- so re-usable in framework
                     *      code which does webserver, because though the abstract API is identical, we
                     *      have very differnt needs about to what to specify/retrieve in one case or the other.
                     *
                     *      @todo maybe reconsider?
                     */
                    struct  Request {
                        String                  fMethod;
                        Mapping<String, String> fOverrideHeaders;
                        BLOB                    fData;  // usually empty, but provided for some methods like POST

                        /**
                         *  Scans fOverrideHeaders
                         */
                        nonvirtual  InternetMediaType   GetContentType () const;

                        /**
                         * updates fOverrideHeaders
                         */
                        nonvirtual  void                SetContentType (const InternetMediaType& ct);
                    };


                    /**
                     */
                    struct  Response {
                        struct  SSLResultInfo;
                        Response ();

                        BLOB                        fData;  // usually empty, but provided for some methods like POST
                        Mapping<String, String>     fHeaders;
                        HTTP::Status                fStatus;
                        Optional<SSLResultInfo>     fServerEndpointSSLInfo;

                        /**
                         */
                        nonvirtual  bool                GetSucceeded () const;

                        /**
                         */
                        nonvirtual  InternetMediaType   GetContentType () const;    // scans headers

                        /**
                         *  Throws HTTP::Exception if not 'GetSucceeded'
                         */
                        nonvirtual  void    ThrowIfFailed () const;
                    };


                    /**
                     *  This info is returned only for secure connections - and is an indicator of whether or
                     *  not the SSL connection was valid
                     *
                     *  This system allows invalid SSL certs as the target - by default - and returns that info, so its up
                     *  to the caller to decide whether or not to accept the data from an invalid SSL cert
                     */
                    struct  Response::SSLResultInfo {
                        String  fSubjectCommonName;     // hostname declared
                        String  fSubjectCompanyName;
                        String  fStyleOfValidation;     // a string saying how the cert was valided - for example 'Domain Controll Validated'
                        String  fIssuer;
                        enum    class ValidationStatus : uint8_t {
                            eNoSSL,
                            eSSLOK,
                            eCertNotYetValid,   // start date too soon
                            eCertExpired,
                            eHostnameMismatch,
                            eSSLFailure,        // generic - typically bad CERT - or bad trust

                            Stroika_Define_Enum_Bounds(eNoSSL, eSSLFailure)
                        };
                        ValidationStatus    fValidationStatus { ValidationStatus::eNoSSL };
                    };


                    /**
                     * TODO:
                     *      Unclear about copyability - maybe if its a smartptr OK to copy - but would be copy-by-reference?
                     *      Could be confusing! CONSIDER
                     */
                    class   Connection {
                    protected:
                        class   _IRep;

                    public:
                        struct  Options;

                    protected:
                        /**
                         */
                        Connection (const shared_ptr<_IRep>& rep);

                    public:
                        /**
                         * Send should timeout after this amount of time. Note - the initial Send may do
                         * much more work (nslookup and tcp connect) than subsequent ones, and this same timeout is used for the combined time.
                         */
                        nonvirtual  DurationSecondsType     GetTimeout () const;

                    public:
                        /**
                         */
                        nonvirtual  void                    SetTimeout (DurationSecondsType timeout);

                    public:
                        /**
                         */
                        nonvirtual  URL     GetURL () const;

                    public:
                        /**
                         */
                        nonvirtual  void    SetURL (const URL& url);

                    public:
                        /**
                         *  force closed Connection. Can still call Send again, but that autocreates new Connection
                         */
                        nonvirtual  void    Close ();

                    public:
                        _DeprecatedFunction_ (Response    SendAndRequest (const Request& r), "Use Send() - to be removed after v2.0a46");

                    public:
                        /*
                         *  This returns a response object, which possibly contains an http error.
                         *
                         *  Example usage:
                         *      Request r = conn.Send (Request (...));
                         *      r.ThrowIfFailed ();
                         *      ...
                         */
                        nonvirtual  Response    Send (const Request& r);

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         *
                         *  Example usage:
                         *      Request r = conn.Get ();
                         *      r.ThrowIfFailed ();
                         *      ...
                         */
                        nonvirtual  Response    GET (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         *
                         *  Example usage:
                         *      Request r = conn.Post (data);
                         *      r.ThrowIfFailed ();
                         *      ...
                         */
                        nonvirtual  Response    POST (const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         *
                         *  Example usage:
                         *      Request r = conn.Delete ();
                         *      r.ThrowIfFailed ();
                         *      ...
                         */
                        nonvirtual  Response    DELETE (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         *
                         *  Example usage:
                         *      Request r = conn.Put (data);
                         *      r.ThrowIfFailed ();
                         *      ...
                         */
                        nonvirtual  Response    PUT (const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         */
                        nonvirtual  Response    OPTIONS (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    private:
                        shared_ptr<_IRep>    fRep_;
                    };


                    /**
                        */
                    struct  Connection::Options {
                        bool    fReturnSSLInfo { false };
                        bool    fAssumeLowestCommonDenominatorHTTPServer { false };
                    };


                    /**
                     * Simple connection factory object. If you don't care what backend to use for remote connections, use this API
                     * to construct an unconnected object.
                     */
                    Connection  CreateConnection (const Connection::Options& options = Connection::Options ());


                    /**
                     */
                    class   Connection::_IRep {
                    public:
                        _IRep ();
                        _IRep (const _IRep&) = delete;
                        virtual ~_IRep ();

                    public:
                        nonvirtual  _IRep& operator= (const _IRep&) = delete;

                    public:
                        virtual URL                 GetURL () const                             =   0;
                        virtual void                SetURL (const URL& url)                     =   0;
                        virtual DurationSecondsType GetTimeout () const                         =   0;
                        virtual void                SetTimeout (DurationSecondsType timeout)    =   0;
                        virtual void                Close ()                                    =   0;
                        virtual Response            Send (const Request& r)                     =   0;
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Client.inl"

#endif  /*_Stroika_Foundation_IO_Network_Transfer_Client_h_*/
