/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Response_h_
#define _Stroika_Framework_WebServer_Response_h_    1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <map>
#include    <vector>

#include    "../../Foundation/Characters/CodePage.h"
#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"
#include    "../../Foundation/IO/Network/Socket.h"
#include    "../../Foundation/IO/Network/HTTP/Status.h"
#include    "../../Foundation/Streams/BinaryOutputStream.h"
#include    "../../Foundation/Streams/BufferedBinaryOutputStream.h"




/*
 * TODO:
 *      @todo   Redo using Association, and maybe BLOB - anyhow - use Stroika types...
 *              (not sure but I think some headers can appear more than once, so Association not Mapping for
 *              headers).
 *
 *      @todo   REDO THE HTTPRESPONSE USING A BINARY OUTPUT STREAM.
 *              INTERNALLY - based on code page - construct a TEXTOUTPUTSTREAM wrapping that binary output stream!!!
 *
 *      @todo   Have output CODEPAGE param - used for all unincode-string writes. Create Stream wrapper than does the downshuft
 *              to right codepage.
 *
 *      @todo   Need a clear policy about threading / thread safety. PROBABLY just PROTECT all our APIs. But if not - detect unsafe
 *              usage.
 *
 *      @todo   eExact is UNTESTED, and should have CHECKING code - so if a user writes a different amount, we detect and assert out.
 *              But that can be deferered because it probably works fine for the the case where its used properly.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {


            using   namespace   Stroika::Foundation;
            using   namespace   Stroika::Foundation::IO::Network::HTTP;

            using   Characters::String;
            using   DataExchange::InternetMediaType;
            using   Memory::Byte;


            /*
             * As of yet to specify FLUSH semantics - when we flush... Probably need options (ctor/config)
             */
            struct  Response {
            public:
                Response () = delete;
                Response (const Response&) = delete;
                Response (const IO::Network::Socket& s, Streams::BinaryOutputStream<> outStream, const InternetMediaType& ct);

                // Reponse must be completed (OK to Abort ()) before being destroyed
                ~Response ();

            public:
                nonvirtual  const Response& operator= (const Response&) = delete;

            public:
                /*
                 * Note - this refers to an HTTP "Content-Type" - which is really potentially more than just a InternetMediaType, often
                 * with the characterset appended.
                 *
                 * SetContentType () requires GetState () == eInProgress
                 */
                nonvirtual  InternetMediaType   GetContentType () const;
                nonvirtual  void                SetContentType (const InternetMediaType& contentType);

            public:
                /*
                 * Note - the code page is only applied to string/text conversions and content-types which are know text-based content types.
                 * For ContentTypes
                 *      o   text/*
                 *      o   application/json
                 *  and any other content type that returns true to InternetMediaType::IsTextFormat () the codepage is added to the content-type as in:
                 *          "text/html; charset=UTF-8"
                 *
                 * SetCodePage ()
                 *      REQUIRES:
                 *          GetState () == eInProgress
                 *          TotalBytesWritten == 0
                 */
                nonvirtual  Characters::CodePage    GetCodePage () const;
                nonvirtual  void                    SetCodePage (Characters::CodePage codePage);

            public:
                enum class State : uint8_t {
                    eInProgress,                // A newly constructed Response starts out InProgress
                    eInProgressHeaderSentState, // It then transitions to 'header sent' state
                    eCompleted,                 // and finally to Completed

                    Stroika_Define_Enum_Bounds(eInProgress, eCompleted)
                };
                nonvirtual  State   GetState () const;

            public:
                enum class ContentSizePolicy : uint8_t {
                    eAutoCompute,
                    eExact,
                    eNone,

                    Stroika_Define_Enum_Bounds(eAutoCompute, eNone)
                };
                nonvirtual  ContentSizePolicy   GetContentSizePolicy () const;
                /*
                 * The 1 arg overload requires csp == NONE or AutoCompute. The 2-arg variant requires
                 * its argument is Exact_CSP.
                 *
                 * Also - SetContentSizePolicy () requires GetState () == eInProgress
                 */
                nonvirtual  void                SetContentSizePolicy (ContentSizePolicy csp);
                nonvirtual  void                SetContentSizePolicy (ContentSizePolicy csp, uint64_t size);

            public:
                /*
                 * This begins sending the parts of the message which have already been accumulated to the client.
                 * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
                 * if we are NOT in automatic-include-Content-Length mode (NYI).
                 *
                 * This does NOT End the repsonse, and it CAN be called arbitrarily many times (even after the response has completed - though
                 * its pointless then).
                 */
                nonvirtual  void    Flush ();

                /*
                 * This signifies that the given request has been handled. Its illegal to write to this request object again, or modify
                 * any aspect of it. The state must be eInProgress or eInProgressHeaderSentState and it sets the state to eCompleted.
                 */
                nonvirtual  void    End ();

                /*
                 * This can be called anytime, but has no effect if the status = eCompleted. It has the effect of throwing away all
                 * unsent data, and closing the associated socket.
                 */
                nonvirtual  void    Abort ();

                /*
                 * Only legal to call if state is eInProgress. It sets the state to eCompleted.
                 */
                nonvirtual  void    Redirect (const String& url);

            public:
                nonvirtual  void    write (const Byte* start, const Byte* end);
                nonvirtual  void    write (const wchar_t* e);
                nonvirtual  void    write (const wchar_t* s, const wchar_t* e);
                nonvirtual  void    write (const wstring& e);
                nonvirtual  void    printf (const wchar_t* format, ...);
                nonvirtual  void    writeln (const wchar_t* e);
                nonvirtual  void    writeln (const wstring& e);

            public:
                virtual     void    clear ();
                nonvirtual  bool    empty () const;

// REDO USING BINARY STREAM (CTOR SHOULD TAKE BINARY STREAM CTOR)
            public:
                nonvirtual  const vector<Byte>& GetBytes () const;

            public:
                /*
                 * The Default Status is 200 IO::Network::HTTP::StatusCodes::kOK.
                 */
                nonvirtual  Status  GetStatus () const;
                /*
                 * It is only legal to call SetStatus with state == eInProgress.
                 *
                 * The overrideReason - if specified (not empty) will be used associated with the given status in the HTTP response, and otherwise one will
                 * be automatically generated based on the status.
                 */
                nonvirtual  void    SetStatus (Status newStatus, const String& overrideReason = wstring ());


            public:
                //
                /*
                 *  Add the given 'non-special' header to the list of headers to be associated with this reponse.
                 *  Certain SPECIAL headers are handled differently, via other attributes of the request. The special headers
                 *  that cannot be specified here include:
                 *      o   IO::Network::HTTP::HeaderName::kContentLength
                 *
                 * It is legal to call anytime before FLush. Illegal to call after flush. Can call to replace existing headers values -
                 */
                nonvirtual  void    AddHeader (String headerName, String value);
                nonvirtual  void    ClearHeader ();
                nonvirtual  void    ClearHeader (String headerName);
                nonvirtual  map<String, String>  GetSpecialHeaders () const;
                /*
                 * This includes the user-set headers (AddHeader) and any special infered headers from other options, like
                 * Connection: close, Content-Type, etc.
                 */
                nonvirtual  map<String, String>  GetEffectiveHeaders () const;

            private:
                IO::Network::Socket                     fSocket_;
                State                                   fState_;
                Status                                  fStatus_;
                String                                  fStatusOverrideReason_;
                Streams::BinaryOutputStream<>           fUnderlyingOutStream_;
                Streams::BufferedBinaryOutputStream     fUseOutStream_;
                map<String, String>                     fHeaders_;
                InternetMediaType                       fContentType_;
                Characters::CodePage                    fCodePage_;
                vector<Byte>                            fBytes_;
                ContentSizePolicy                       fContentSizePolicy_;
                uint64_t                                fContentSize_;          // only  maintained for some policies
            };

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Response.inl"

#endif  /*_Stroika_Framework_WebServer_Response_h_*/
