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
#include "../../Foundation/Common/Property.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Status.h"
#include "../../Foundation/IO/Network/HTTP/Response.h"
#include "../../Foundation/IO/Network/Socket.h"
#include "../../Foundation/IO/Network/URI.h"
#include "../../Foundation/Memory/BLOB.h"
#include "../../Foundation/Streams/BufferedOutputStream.h"
#include "../../Foundation/Streams/OutputStream.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   REDO THE HTTPRESPONSE USING A BINARY OUTPUT STREAM.
 *              INTERNALLY - based on code page - construct a TEXTOUTPUTSTREAM wrapping that binary output stream!!!
 *
 *      @todo   Have output CODEPAGE param - used for all unincode-string writes. Create Stream wrapper than does the downshuft
 *              to right codepage.
 *
 *      @todo   Cleanup interaction between Flush () and write (bytes) so clearer (document) the different cases of when we state
 *              transition and what is legal for case of transfer-encoding: chunked.
 * 
 *              PROBABLE PLAN:
 *                  RENAME eInProgress -> eInProgressPreparingHeaders
 *                  RENAME eInProgressHeaderSentState -> eInProgressPreparingBodyBeforeHeadersSent & eInProgressPreparingBodyAfterHeadersSent
 * 
 *              Once you enter the state eStartedBody (which NO LONGER IMPLES HEADERS SENT but IMPLIES ILLEGAL TO CHANGE HEADERS EXCEPT INTERNALLY)
 *              Then we can allow you to keep doing nothing but writes (and we internally update the content length accorindly)
 * 
 *              This SHOULD work for currnet idneity mode, and chunked encoding (in later case we would really flush headers).
 *              And I THINK it also works for content-endcoding for gzip.
 */

namespace Stroika::Frameworks::WebServer {

    using std::byte;

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Memory::BLOB;
    using HTTP::Status;

    /*
     * As of yet to specify FLUSH semantics - when we flush... Probably need options (ctor/config)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Response : public IO::Network::HTTP::Response {
    private:
        using inherited = IO::Network::HTTP::Response;

    public:
        /**
         */
        Response ()                    = delete;
        Response (const Response&)     = delete;
        Response (Response&& src);
        Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const optional<InternetMediaType>& ct = nullopt);

    public:
        // Reponse must be completed (OK to Abort ()) before being destroyed
        ~Response () = default;

    public:
        nonvirtual Response& operator= (const Response&) = delete;

    public:
        /*
         * Note - the code page is only applied to string/text conversions and content-types which are know text-based content types.
         * For ContentTypes
         *      o   text / * {avoid comment-character}
         *      o   application/json
         *  and any other content type that returns true to InternetMediaType::IsTextFormat () the codepage is added to the content-type as in:
         *          "text/html; charset=UTF-8"
         *
         * codePage.Set ()
         *      REQUIRES:
         *          GetState () == eInProgress
         *          TotalBytesWritten == 0
         * 
         * \note - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *         the characterset will be automatically folded into the used contentType. To avoid this, 
         *         Use UpdateHeader() to mdofiy teh contenttype field directly.
         * 
         */
        Common::Property<Characters::CodePage> codePage;

    public:
        /*
         * Note - this refers to an HTTP "Content-Type" - which is really potentially more than just a InternetMediaType, often
         * with the characterset appended.
         *
         *  \req GetState () == eInProgress     // since this calls rwHeaders()
         * 
         *  NOTE - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (contentType), then
         *  the characterset will be automatically folded into the used contentType (on WRITES to the property - not reads). To avoid this, 
         *  use rwHeader().contentType directly.
         * 
         *  && OVERRIDES set of contentType property
         *  @todo DOC / NOTE THIS CLASS OVERRIDES the assignment of contentType to do above logic...
         * 
         *      Common::Property <optional<InternetMediaType>> contentType;
         */

    public:
        /**
         *  \note about states - certain properties (declared here and inherited) - like rwHeaders, and writes to properites like (XXX) cannot be done
         *        unless the current state is eInProgress; and these are generally checked with assertions.
         */
        enum class State : uint8_t {
            eInProgress,                // A newly constructed Response starts out InProgress
            eInProgressHeaderSentState, // It then transitions to 'header sent' state
            eCompleted,                 // and finally to Completed

            Stroika_Define_Enum_Bounds (eInProgress, eCompleted)
        };

    public:
        /**
         *  The state is changed by calls to Flush (), Abort (), Redirect (), and End ()
         */
        Common::ReadOnlyProperty<State> state;

    public:
        /**
         *  This cannot be reversed, but puts the response into a mode where it won't emit the body of the response.
         */
        nonvirtual void EnterHeadMode ();

    public:
        /**
         * This begins sending the parts of the message which have already been accumulated to the client.
         * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
         * IFF you first set the respose.transferEncoding mode to TransferEncoding::eChunked.
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
        nonvirtual void Redirect (const URI& url);

    public:
        /**
         *  Depending on modes, write MAY or MAY NOT call Flush () sending the headers. So callers
         *  should set any headers before calling write (or printf which calls write).
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
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika 2.1b10")]] void clear ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            fBodyBytes_.clear ();
        }
        [[deprecated ("Since Stroika 2.1b10")]] bool empty () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            return fBodyBytes_.empty ();
        }
        [[deprecated ("Since Stroika 2.1b10")]] const vector<byte>& GetBytes () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            return fBodyBytes_;
        }
        [[deprecated ("Since Stroika v2.1b10 use this->state property")]] inline State GetState () const
        {
            return this->state ();
        }
        [[deprecated ("Since Stroika v2.1b10 use this->status property")]] Status GetStatus () const
        {
            return this->status ();
        }
        [[deprecated ("Since Stroika v2.1b10 use this->statusAndOverrideREason property")]] void SetStatus (Status newStatus, const String& overrideReason = wstring{})
        {
            this->statusAndOverrideReason = make_tuple (newStatus, overrideReason);
        }
        [[deprecated ("Since Stroika 2.1b10, use contentType() property directly")]] void SetContentType (const InternetMediaType& newCT)
        {
            this->rwHeaders ().contentType = AdjustContentTypeForCodePageIfNeeded_ (newCT);
        }
        [[deprecated ("Since 2.1b10, use headers() directly")]] IO::Network::HTTP::Headers    GetHeaders () const
        {
            return this->headers ();
        }
        [[deprecated ("Since Stroika 2.1b10 - use codePage()")]] Characters::CodePage         GetCodePage () const
        {
            shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
            return fCodePage_;
        }
        [[deprecated ("Since Stroika 2.1b10 - use codePage()")]] void SetCodePage (Characters::CodePage newCodePage)
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Require (fState_ == State::eInProgress);
            Require (fBodyBytes_.empty ());
            bool diff  = fCodePage_ != newCodePage;
            fCodePage_ = newCodePage;
            if (diff) {
                if (auto ct = this->rwHeaders ().contentType ()) {
                    this->rwHeaders ().contentType = AdjustContentTypeForCodePageIfNeeded_ (*ct);
                }
            }
        }
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeader()")]] InternetMediaType GetContentType () const
        {
            return this->headers ().contentType ().value_or (InternetMediaType{});
        }
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeader")]] void                       AddHeader (const String& headerName, const String& value)
        {
            this->rwHeaders ().Set (headerName, value);
        }
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeaders directly")]] void            AppendToCommaSeperatedHeader (const String& headerName, const String& value)
        {
            Require (not value.empty ());
            auto& updateHeaders = this->rwHeaders ();
            if (auto o = updateHeaders.LookupOne (headerName)) {
                if (o->empty ()) {
                    updateHeaders.Add (headerName, value);
                }
                else {
                    updateHeaders.Add (headerName, *o + L", "sv + value);
                }
            }
            else {
                updateHeaders.Add (headerName, value);
            }
        }
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeaders directly")]] nonvirtual void ClearHeader (const String& headerName)
        {
            // DEPRECATED
            this->rwHeaders ().Remove (headerName);
        }
        [[deprecated ("Since Stroika 2.1b10 - use UpdateHeaders directly")]] void ClearHeaders ()
        {
            this->rwHeaders () = IO::Network::HTTP::Headers{};
        }
        [[deprecated ("Since 2.1b10, use headers() directly")]] IO::Network::HTTP::Headers GetEffectiveHeaders () const
        {
            return this->headers ();
        }
        template <typename FUNCTION>
        [[deprecated ("Since 2.1b10, use rwHeaders() directly")]] inline auto UpdateHeader (FUNCTION&& f)
        {
            Require (fState_ == State::eInProgress);
            return std::forward<FUNCTION> (f) (&this->rwHeaders ());
        }
        template <typename FUNCTION>
        [[deprecated ("Since 2.1b10, use headers() directly")]] inline  auto ReadHeader (FUNCTION&& f) const
        {
            return std::forward<FUNCTION> (f) (this->headers ());
        }

    private:
        nonvirtual InternetMediaType AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const;

    private:
        IO::Network::Socket::Ptr                 fSocket_;
        State                                    fState_{State::eInProgress};
        Streams::OutputStream<byte>::Ptr         fUnderlyingOutStream_;
        Streams::BufferedOutputStream<byte>::Ptr fUseOutStream_;
        Characters::CodePage                     fCodePage_{Characters::kCodePage_UTF8};
        vector<byte>                             fBodyBytes_{};
        bool                                     fHeadMode_{false};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Framework_WebServer_Response_h_*/
