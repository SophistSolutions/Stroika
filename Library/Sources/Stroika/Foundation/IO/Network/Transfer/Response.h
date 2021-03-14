/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Response_h_
#define _Stroika_Foundation_IO_Network_Transfer_Response_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Mapping.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Memory/BLOB.h"
#include "../../../Streams/InputStream.h"
#include "../../../Time/Realtime.h"

#include "../HTTP/Exception.h"
#include "../HTTP/Status.h"

#include "Request.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *      @todo   Consider if Response::GetSucceeded () should return true or false for 3xx responses.
 *              I think its correct due to https://developer.mozilla.org/en-US/docs/Web/HTTP/Status 
 *              But maybe add GetFailed () as a return value for 400/500 responses and use that
 *              in Connection::Send()
 * 
 *      @todo   https://stroika.atlassian.net/browse/STK-724 - IO::Network::Transfer::Request/Response should use IO::Network::HTTP::Headers (not mapping)
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    using std::byte;

//avoid windows header clash...
#if qPlatform_Windows
//#undef DELETE
#endif

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Memory::BLOB;
    using Streams::InputStream;
    using Time::DurationSecondsType;

    /**
     *  DESIGN NOTES:
     *      The Response can be constructed with a BLOB of data, or a binary stream. If constructed
     *      with a blob of data, the binary stream returned will be a reference to this data, and can be used
     *      to page out the data.
     *
     *      If constructed with a BinaryStream (NYI) - then the caller (Client::Send) can return before all the data
     *      is available. Any future calls on the Client may block (TBD???) or fail???
     *
     *      But then the data can be paged out without ever being accumulated into a single large buffer.
     *
     *      Note - if the use of a Response calls GetBLOB () - even if constructed with a stream - this will
     *      block reading in the stream.
     *
     *      If the caller gets GetResponseStream() - then calls to GetBLOB() will fail. Note - we COULD have
     *      kept the bytes around from the response stream as it came in, but that could in principle be large,
     *      and your probably using the Stream API to avoid having the entire thing in emmroy so we don't want
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
    class Response {
    public:
        /**
         *  This info is returned only for secure connections - and is an indicator of whether or
         *  not the SSL connection was valid
         *
         *  This system allows invalid SSL certs as the target - by default - and returns that info, so its up
         *  to the caller to decide whether or not to accept the data from an invalid SSL cert
         */
        struct SSLResultInfo {
            String fSubjectCommonName; // hostname declared
            String fSubjectCompanyName;
            String fStyleOfValidation; // a string saying how the cert was valided - for example 'Domain Controll Validated'
            String fIssuer;

            /**
             *
             *  \note   Configuration::DefaultNames<> supported
             */
            enum class ValidationStatus : uint8_t {
                eNoSSL,
                eSSLOK,
                eCertNotYetValid, // startB date too soon
                eCertExpired,
                eHostnameMismatch,
                eSSLFailure, // generic - typically bad CERT - or bad trust

                Stroika_Define_Enum_Bounds (eNoSSL, eSSLFailure)
            };
            ValidationStatus fValidationStatus{ValidationStatus::eNoSSL};

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         */
        Response (const BLOB& data, HTTP::Status status, const Mapping<String, String>& headers, const optional<SSLResultInfo>& sslInfo = optional<SSLResultInfo> ());

    public:
        /**
         *  Returns true iff response is in the 200 range - indicating success. Even 3xx responses return false.
         *
         *  \note - for 300 and 100 responses, both GetSucceeded and GetFailed return false.
         */
        nonvirtual bool GetSucceeded () const;

    public:
        /**
         *  Returns true iff response is in the 4xx-5xxx range - indicating failure.
         *
         *  \note - for 300 and 100 responses, both GetSucceeded and GetFailed return false.
         */
        nonvirtual bool GetFailed () const;

    public:
        /**
         */
        nonvirtual BLOB GetData () const;

    public:
        /**
         *  TBD how this will work - use with caution. Unclear if you call twice you get same stream. Want to evanutaly
         *  support delayed read (so return before all data read.
         *
         *  \par Example Usage
         *      \code
         *          Response r = clientConn.GET ();
         *          VariantValue v = JSON::Reader ().Read (r.GetDataBinaryInputStream ());
         *      \endcode
         */
        nonvirtual InputStream<byte>::Ptr GetDataBinaryInputStream () const;

    public:
        /**
         *  This is layered on top of GetDataBinaryInputStream(), but uses any Response headers, such as
         *  content type, to help decode the text stream as best as possible.
         *      (as of 2014-10-08 - this is PLANED decoding, and not necesarily fully implemented).
         *      @todo FIX to take into account codepage/etc from MIME content type!
         *
         *  \par Example Usage
         *      \code
         *          Response r = clientConn.GET ();
         *          String answer = r.GetDataTextInputStream ().ReadAll ();
         *      \endcode
         */
        nonvirtual InputStream<Characters::Character>::Ptr GetDataTextInputStream () const;

    public:
        /**
         */
        nonvirtual HTTP::Status GetStatus () const;

    public:
        /**
         */
        nonvirtual optional<SSLResultInfo> GetSSLResultInfo () const;

    public:
        /**
         */
        nonvirtual Mapping<String, String> GetHeaders () const;

    public:
        /**
         * scans headers
         */
        nonvirtual optional<InternetMediaType> GetContentType () const; // scans headers

    public:
        /**
         * scans headers (Content-Type: xxx; charset=yyy
         *
         *  Can be any name registered in https://www.iana.org/assignments/character-sets/character-sets.xhtml
         */
        nonvirtual optional<String> GetCharset () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        BLOB                                                      fData_;                  // usually empty, but provided for some methods like POST
        mutable optional<InputStream<byte>::Ptr>                  fDataBinaryInputStream_; // store so subsequent calls to GetBinaryStream() returns same offset/pointer
        mutable optional<InputStream<Characters::Character>::Ptr> fDataTextInputStream_;
        Mapping<String, String>                                   fHeaders_;
        HTTP::Status                                              fStatus_{};
        optional<SSLResultInfo>                                   fServerEndpointSSLInfo_;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Response_h_*/
