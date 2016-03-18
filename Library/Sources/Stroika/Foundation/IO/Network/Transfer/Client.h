/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_h_   1

#include    "../../../StroikaPreComp.h"

#include    <string>

#include    "../../../Characters/String.h"
#include    "../../../Characters/String_Constant.h"
#include    "../../../Configuration/Common.h"
#include    "../../../Containers/Mapping.h"
#include    "../../../DataExchange/InternetMediaType.h"
#include    "../../../Memory/BLOB.h"
#include    "../../../Memory/Optional.h"
#include    "../../../Streams/InputStream.h"
#include    "../../../Time/Realtime.h"

#include    "../URL.h"
#include    "../HTTP/Exception.h"
#include    "../HTTP/Status.h"


/**
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *
 *      @todo   FIXUP Docs and code so on thrown response, and open stream in the response is replaced by
 *              a FULLY REALIZED STREAM (MEMORY STREAM)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-443 - cleanup exception handling and docs a little.
 *
 *      @todo   Probably should redo Request so it can optionally use a BLOB or
 *              Stream (like Response). DO NOW the header / class changes - making
 *              CTOR/accessors, so easiser to change funcitonality later!!!
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
 *              CTOR on response object taking a stream, and hten reasonably (tbd) how to handle
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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {


//avoid windows header clash...
#if     qPlatform_Windows
#undef DELETE
#endif


                    using   Characters::String;
                    using   Containers::Mapping;
                    using   DataExchange::InternetMediaType;
                    using   Memory::BLOB;
                    using   Memory::Byte;
                    using   Memory::Optional;
                    using   Streams::InputStream;
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
                     *  DESIGN NOTES:
                     *      The Response can be constructed with a BLOB of data, or a binary stream. If constructed
                     *      with a blob of data, the binary stream returned will be a reference to this data, and can be used
                     *      to page out the data.
                     *
                     *      If constructed with a BinaryStream (NYI) - then the caller (Client::Send) can return before all the data
                     *      is available. Any future calls on teh Client may block (TBD???) or fail???
                     *
                     *      But then the data can be paged out without ever being accumulated into a single large buffer.
                     *
                     *      Note - if the use of a Response calls GetBLOB () - even if constructed with a stream - this will
                     *      block reading in the stream.
                     *
                     *      If the caller gets GetResponseStream() - then calls to GetBLOB() will fail. Note - we COULD have
                     *      kept the bytes around from the response stream as it came in, but that could in principle be large,
                     *      and your probably using the Stream API to avoid having the entire thing in emmroy so we dont want
                     *      to needlessly thwart that.
                     *
                     *      NOTE - as of 2014-10-08 - the only case thats implemented is the case of construction with a prefetched
                     *      BLOB, and then repsenting THAT as a binary stream if requested.
                     *
                     *  @todo    NOTE IMPLICATIONS ABOUT COIPYING.
                     *          HAVE FLAG CALLED "STREAMED_RESPONSE".
                     *
                     *          CTOR specifies this value and it cannot change and generally comes from request options.
                     *          if streamed response, ILLEGAL to call GETBLOB() (throws IsStreamedResponse).
                     *          If streamed response, copy of Response object copies Stream itself, whcih is (cuz all streams are
                     *          logcially smart poitners to data) - so multiple readers interfere with one another. threadsafe byt
                     *          possibly not the expected behavior).
                     */
                    class  Response {
                    public:
                        /**
                         *  This info is returned only for secure connections - and is an indicator of whether or
                         *  not the SSL connection was valid
                         *
                         *  This system allows invalid SSL certs as the target - by default - and returns that info, so its up
                         *  to the caller to decide whether or not to accept the data from an invalid SSL cert
                         */
                        struct  SSLResultInfo {
                            String  fSubjectCommonName;     // hostname declared
                            String  fSubjectCompanyName;
                            String  fStyleOfValidation;     // a string saying how the cert was valided - for example 'Domain Controll Validated'
                            String  fIssuer;
                            enum    class ValidationStatus : uint8_t {
                                eNoSSL,
                                eSSLOK,
                                eCertNotYetValid,   // startB date too soon
                                eCertExpired,
                                eHostnameMismatch,
                                eSSLFailure,        // generic - typically bad CERT - or bad trust

                                Stroika_Define_Enum_Bounds(eNoSSL, eSSLFailure)
                            };
                            ValidationStatus    fValidationStatus { ValidationStatus::eNoSSL };
                        };

                    public:
                        Response (const BLOB& data, HTTP::Status status, const Mapping<String, String>& headers, const Optional<SSLResultInfo>& sslInfo = Optional<SSLResultInfo> ());
                        Response (BLOB&& data, HTTP::Status status, Mapping<String, String>&& headers);
                        Response (BLOB&& data, HTTP::Status status, Mapping<String, String>&& headers, const Optional<SSLResultInfo>&& sslInfo);

                    public:
                        /**
                         */
                        nonvirtual  bool                GetSucceeded () const;

                    public:
                        /**
                         */
                        nonvirtual  BLOB                GetData () const;

                    public:
                        /**
                         *  TBD how this will work - use with caution. Unclear if you call twice you get same stream. Want to evanutaly
                         *  support delayed read (so return before all data read.
                         *
                         *  \par Example Usage
                         *      \code
                         *      Response r = clientConn.GET ();
                         *      VariantValue v = JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                         *      \endcode
                         */
                        nonvirtual  InputStream<Byte>     GetDataBinaryInputStream () const;

                    public:
                        /**
                         *  This is layered on top of GetDataBinaryInputStream(), but uses any Response headers, such as
                         *  content type, to help decode the text stream as best as possible.
                         *      (as of 2014-10-08 - this is PLANED decoding, and not necesarily fully implemented).
                         *      @todo FIX to take into account codepage/etc from MIME content type!
                         *
                         *  \par Example Usage
                         *      \code
                         *      Response r = clientConn.GET ();
                         *      String answer = r.GetDataTextInputStream ().ReadAll ();
                         *      \endcode
                         */
                        nonvirtual  InputStream<Characters::Character>       GetDataTextInputStream () const;

                    public:
                        /**
                         */
                        nonvirtual  HTTP::Status        GetStatus () const;

                    public:
                        /**
                         */
                        nonvirtual  Optional<SSLResultInfo>     GetSSLResultInfo () const;

                    public:
                        /**
                         */
                        nonvirtual  Mapping<String, String>     GetHeaders () const;

                    public:
                        /**
                         */
                        nonvirtual  InternetMediaType   GetContentType () const;    // scans headers

#if 0
                        // now done automatically
                    public:
                        /**
                         *  Throws HTTP::Exception if not 'GetSucceeded'
                         */
                        nonvirtual  void    ThrowIfFailed () const;
#endif

                    private:
                        BLOB                                                    fData_;                     // usually empty, but provided for some methods like POST
                        mutable Optional<InputStream<Byte>>                     fDataBinaryInputStream_;    // store so subsequent calls to GetBinaryStream() returns same offset/pointer
                        mutable Optional<InputStream<Characters::Character>>    fDataTextInputStream_;
                        Mapping<String, String>                                 fHeaders_;
                        HTTP::Status                                            fStatus_ {};
                        Optional<SSLResultInfo>                                 fServerEndpointSSLInfo_;
                    };


                    /**
                     */
                    struct  Exception : HTTP::Exception {
                    public:
                        Exception (const Response& response);

                    public:
                        nonvirtual  Response    GetResponse () const;

                    private:
                        Response    fResponse_;
                    };


                    /**
                     * TODO:
                     *     @todo    Unclear about copyability - maybe if its a smartptr OK to copy - but would be copy-by-reference?
                     *              Could be confusing! CONSIDER
                     *
                     *  \par Example Usage
                     *      \code
                     *          Connection  c   =   IO::Network::Transfer::CreateConnection ();
                     *          c.SetURL (URL::Parse (L"http://www.google.com"));
                     *          Response    r   =   c.GET ();
                     *          VerifyTestResult (r.GetSucceeded ());
                     *          VerifyTestResult (r.GetData ().size () > 1);
                     *      \endcode
                     *
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
                        /*
                         *  This returns a response object, which possibly contains an http error.
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.Send (Request (...));
                         *      ...
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    Send (const Request& r);

                    public:
                        /*
                         *  Simple wrapper on Send () for method GET
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.GET ();
                         *      ...
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    GET (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrapper on Send () for method POST
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.POST (data);
                         *      ...
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    POST (const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrapper on Send () for method DELETE
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.Delete ();
                         *      ...
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    DELETE (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrapper on Send () for method PUT
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.PUT (data);
                         *      ...
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see an error HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    PUT (const BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    public:
                        /*
                         *  Simple wrappers, with hardwired methods
                         *
                         *  \par Example Usage
                         *      \code
                         *      Request r = conn.OPTIONS ();
                         *      \endcode
                         *
                         *  \note   This function only returns a Response on success. To see HTTP status response, catch (Exception e), and look
                         *          at e.GetResponse ()
                         */
                        nonvirtual  Response    OPTIONS (const Mapping<String, String>& extraHeaders = Mapping<String, String> ());

                    private:
                        shared_ptr<_IRep>    fRep_;
                    };


                    /**
                     */
                    struct  Connection::Options {
                        /**
                         */
                        bool    fReturnSSLInfo { false };

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
                        Optional<bool>  fFailConnectionIfSSLCertificateInvalid;

                        /**
                         */
                        bool    fAssumeLowestCommonDenominatorHTTPServer    { false };

                        /**
                         */
                        bool    fSupportSessionCookies                      { true };

                        /**
                         */
                        String  fUserAgent                                  { Characters::String_Constant { L"Stroika/2.0" } };
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
