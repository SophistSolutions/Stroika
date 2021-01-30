/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Response_h_
#define _Stroika_Framework_WebServer_Response_h_ 1

#include "../StroikaPreComp.h"

#include <map>
#include <string>
#include <vector>

#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Status.h"
#include "../../Foundation/IO/Network/Socket.h"
#include "../../Foundation/Memory/BLOB.h"
#include "../../Foundation/Streams/BufferedOutputStream.h"
#include "../../Foundation/Streams/OutputStream.h"

/*
 * TODO:
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

namespace Stroika::Frameworks::WebServer {

    using std::byte;

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO::Network::HTTP;

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Memory::BLOB;

    /*
     * As of yet to specify FLUSH semantics - when we flush... Probably need options (ctor/config)
     */
    class Response : private Debug::AssertExternallySynchronizedLock {
    public:
        Response ()                    = delete;
        Response (const Response&)     = delete;
        Response (Response&&)  = default;
        Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const InternetMediaType& ct);

    public:
        // Reponse must be completed (OK to Abort ()) before being destroyed
        ~Response () = default;

    public:
        nonvirtual Response& operator= (const Response&) = delete;

    public:
        /**
         *  Allow reference to the headers (modify access) - but do so in the context where we assure single threading
         *  and that this is only done while the transaction is in progress.
         * 
         *  Add the given header to the list of headers to be associated with this reponse.
         *
         * It is legal to call anytime before Flush. Illegal to call after flush. 
         * It is legal to call to replace existing headers values.
         */
        template <typename FUNCTION>
        nonvirtual auto UpdateHeader (FUNCTION&& f);

    public:
        /**
         *  Allow readonly access to the Headers object. This is just checked (assertions) for re-entrancy.
         *  It can be called in any state (during transaction or after).
         */
        template <typename FUNCTION>
        nonvirtual auto ReadHeader (FUNCTION&& f) const;

    public:
        /*
         * Note - this refers to an HTTP "Content-Type" - which is really potentially more than just a InternetMediaType, often
         * with the characterset appended.
         *
         * SetContentType () requires GetState () == eInProgress
         * 
         *  NOTE - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *  the characterset will be automatically folded into the used contentType. To avoid this, 
         *  Use UpdateHeader() to mdofiy teh contenttype field directly.
         */
        nonvirtual void    SetContentType (const InternetMediaType& contentType);

    public:
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeader()")]] InternetMediaType GetContentType () const;

    public:
        /*
         * Note - the code page is only applied to string/text conversions and content-types which are know text-based content types.
         * For ContentTypes
         *      o   text / * {avoid comment-character}
         *      o   application/json
         *  and any other content type that returns true to InternetMediaType::IsTextFormat () the codepage is added to the content-type as in:
         *          "text/html; charset=UTF-8"
         *
         * SetCodePage ()
         *      REQUIRES:
         *          GetState () == eInProgress
         *          TotalBytesWritten == 0
         * 
         * \note - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *         the characterset will be automatically folded into the used contentType. To avoid this, 
         *         Use UpdateHeader() to mdofiy teh contenttype field directly.
         * 
         */
        nonvirtual Characters::CodePage GetCodePage () const;
        nonvirtual void                 SetCodePage (Characters::CodePage codePage);

    public:
        enum class State : uint8_t {
            eInProgress,                // A newly constructed Response starts out InProgress
            eInProgressHeaderSentState, // It then transitions to 'header sent' state
            eCompleted,                 // and finally to Completed

            Stroika_Define_Enum_Bounds (eInProgress, eCompleted)
        };
        nonvirtual State GetState () const;

    public:
        enum class ContentSizePolicy : uint8_t {
            eAutoCompute,
            eExact,
            eNone,

            Stroika_Define_Enum_Bounds (eAutoCompute, eNone)
        };
        nonvirtual ContentSizePolicy GetContentSizePolicy () const;
        /*
         * The 1 arg overload requires csp == NONE or AutoCompute. The 2-arg variant requires
         * its argument is Exact_CSP.
         *
         * Also - SetContentSizePolicy () requires GetState () == eInProgress
         */
        nonvirtual void SetContentSizePolicy (ContentSizePolicy csp);
        nonvirtual void SetContentSizePolicy (ContentSizePolicy csp, uint64_t size);

    public:
        /**
         *  This can be true if:
         *      GetContentSizePolicy () == eAutoCompute or eExact.
         *      \note - you cannot call AddHeader (Content-Length)
         */
        nonvirtual bool IsContentLengthKnown () const;

    public:
        /**
         * This begins sending the parts of the message which have already been accumulated to the client.
         * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
         * if we are NOT in automatic-include-Content-Length mode (NYI).
         *
         * This does NOT End the repsonse, and it CAN be called arbitrarily many times (even after the response has completed - though
         * its pointless then).
         */
        nonvirtual void Flush ();

    public:
        /**
         * This signifies that the given request has been handled. Its illegal to write to this request object again, or modify
         * any aspect of it. The state must be eInProgress or eInProgressHeaderSentState and it sets the state to eCompleted.
         */
        nonvirtual void End ();

    public:
        /**
         * This can be called anytime, but has no effect if the status = eCompleted. It has the effect of throwing away all
         * unsent data, and closing the associated socket.
         */
        nonvirtual void Abort ();

    public:
        /**
         * Only legal to call if state is eInProgress. It sets the state to eCompleted.
         */
        nonvirtual void Redirect (const String& url);

    public:
        /**
         */
        nonvirtual void write (const BLOB& b);
        nonvirtual void write (const byte* start, const byte* end);
        nonvirtual void write (const wchar_t* e);
        nonvirtual void write (const wchar_t* s, const wchar_t* e);
        nonvirtual void write (const String& e);

    public:
        /**
         */
        nonvirtual void printf (const wchar_t* format, ...);

    public:
        /**
         *  writeln () does a write, followed by writing a CRLF
         */
        nonvirtual void writeln (const wchar_t* e);
        nonvirtual void writeln (const String& e);

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         */
        nonvirtual bool empty () const;

    public:
        /**
        // REDO USING BINARY STREAM (CTOR SHOULD TAKE BINARY STREAM CTOR)
        */
        nonvirtual const vector<byte>& GetBytes () const;

    public:
        /*
         * The Default Status is 200 IO::Network::HTTP::StatusCodes::kOK.
         */
        nonvirtual Status GetStatus () const;

    public:
        /*
         * It is only legal to call SetStatus with state == eInProgress.
         *
         * The overrideReason - if specified (not empty) will be used associated with the given status in the HTTP response, and otherwise one will
         * be automatically generated based on the status.
         */
        nonvirtual void SetStatus (Status newStatus, const String& overrideReason = wstring{});

    public:
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeader")]] void AddHeader (const String& headerName, const String& value);

    public:
        /*
         *  For headers whose value is a comma separated list (e.g. HTTP::HeaderName::kAccessControlAllowHeaders), 
         *  append the given value, taking care of commas as needed.
         *
         *  Get the given header, and if non-empty, append ",". Either way append value, and then set the header named by 'headerName' to the resulting value.
         */
        nonvirtual void AppendToCommaSeperatedHeader (const String& headerName, const String& value);

    public:
        /**
         */
        nonvirtual void ClearHeader (const String& headerName);

    public:
        /**
         */
        nonvirtual void ClearHeaders ();

    public:
        /**
         *  \brief return the set of explicitly specified headers.
         *
         *  @see GetEffectiveHeaders ()
         */
        nonvirtual IO::Network::HTTP::Headers GetHeaders () const;

    public:
        [[deprecated ("Since 2.1b10, use GetHeaders() directly")]] IO::Network::HTTP::Headers GetEffectiveHeaders () const
        {
            return GetHeaders ();
        }

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        InternetMediaType AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const;

    private:
        IO::Network::Socket::Ptr                 fSocket_;
        State                                    fState_;
        Status                                   fStatus_;
        String                                   fStatusOverrideReason_;
        Streams::OutputStream<byte>::Ptr         fUnderlyingOutStream_;
        Streams::BufferedOutputStream<byte>::Ptr fUseOutStream_;
        IO::Network::HTTP::Headers               fHeaders_;
        Characters::CodePage                     fCodePage_;
        vector<byte>                             fBytes_;
        ContentSizePolicy                        fContentSizePolicy_;
        uint64_t                                 fContentSize_; // only  maintained for some fContentSizePolicy_ values
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Framework_WebServer_Response_h_*/
