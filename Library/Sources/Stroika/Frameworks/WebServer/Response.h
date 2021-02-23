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
     *  \note Set headers (with this->rwHeaders()...) as early as practical (before calling write or Flush).
     *  \note To use chunked transfer encoding (NOT THE DEFAULT) - call
     *      \code
     *        response->rwHeaders ().transferEncoding = HTTP::TransferEncoding::eChunked;
     *        ... then do 
     *        response->write (...);
     *      \endcode
     *
     *  TODO:
     *      @todo Support https://stroika.atlassian.net/browse/STK-727 - HTTP Chunked Transfer Trailers. We do support
     *            chunked transfers, but require all the headers set first.
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
        Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const optional<HTTP::Headers>& initialHeaders = nullopt);

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
         *          GetState () == ePreparingHeaders
         *          TotalBytesWritten == 0
         * 
         * \note - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *         the characterset will be automatically folded into the used contentType. To avoid this, 
         *         Use UpdateHeader() to mdofiy teh contenttype field directly.
         * 
         *  \req this->headersCanBeSet() to set property
         */
        Common::Property<Characters::CodePage> codePage;

    public:
        /*
         * Note - this refers to an HTTP "Content-Type" - which is really potentially more than just a InternetMediaType, often
         * with the characterset appended.
         *
         *  \req GetState () == ePreparingHeaders     // since this calls rwHeaders()
         * 
         *  NOTE - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (contentType), then
         *  the characterset will be automatically folded into the used contentType (on WRITES to the property - not reads). To avoid this, 
         *  use rwHeader().contentType directly.
         * 
         *  && OVERRIDES set of contentType property
         *  @todo DOC / NOTE THIS CLASS OVERRIDES the assignment of contentType to do above logic...
         * 
         *      Common::Property <optional<InternetMediaType>> contentType;
         * 
         *  \req this->headersCanBeSet() to set property
         */

    public:
        /**
         *  \note about states - certain properties (declared here and inherited) - like rwHeaders, and writes to properites like (XXX) cannot be done
         *        unless the current state is ePreparingHeaders; and these are generally checked with assertions.
         */
        enum class State : uint8_t {
            ePreparingHeaders,                  // A newly constructed Response starts out ePreparingHeaders state
            ePreparingBodyBeforeHeadersSent,    // headers can no longer be adjusted, but have not been sent over the wire
            ePreparingBodyAfterHeadersSent,     // headers have now been sent over the wire
            eCompleted,                         // and finally to Completed

            Stroika_Define_Enum_Bounds (ePreparingHeaders, eCompleted)
        };

    public:
        /**
         *  The state may be changed by calls to Flush (), Abort (), Redirect (), and End (), and more...
         */
        Common::ReadOnlyProperty<State> state;

    public:
        /**
         *  Check this (readonly) property before updating headers. For now, this is the same as this->state == ePreparingHeaders
         * 
         *  \note - even HTTP 1.1 allows for headers to be sent AFTER we've started sending chunks, so this interpretation MAY
         *        change over time, but the current implementation only allows setting headers while in the preparingHeaders state
         *        (so in other words, before any calls to Flush or write).
         */
        Common::ReadOnlyProperty<bool> headersCanBeSet;

    public:
        /**
         *  This cannot be reversed, but puts the response into a mode where it won't emit the body of the response.
         * 
         *  \req this->state == State::ePreparingHeaders
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
         * 
         *  This can be called in any state.
         */
        nonvirtual void Flush ();

    public:
        /**
         * This signifies that the given request has been handled. Its illegal to write to this request object again, or modify
         * any aspect of it. The state must be ePreparingHeaders or ePreparingBodyAfterHeadersSent and it sets the state to eCompleted.
         * 
         *  \reqquire this->state() != eComplete
         *  \ensure this->state() == eComplete
         */
        nonvirtual void End ();

    public:
        /**
         * This can be called anytime, but has no effect if the status = eCompleted. It has the effect of throwing away all
         * unsent data, and closing the associated socket.
         *
         *  Can be called in any state
         *  \ensure this->state() == eComplete
         */
        nonvirtual void Abort ();

    public:
        /**
         * Only legal to call if state is ePreparingHeaders. It sets the state to eCompleted.
         *
         *  \require this->headersCanBeSet
         *  \ensure this->state() == eComplete
         */
        nonvirtual void Redirect (const URI& url);

    public:
        /**
         *  Depending on modes, write MAY or MAY NOT call Flush () sending the headers. So callers
         *  should set any headers before calling write (or printf which calls write).
         * 
         *  Note for string and wchar_t* writes, this uses this->codePage to encode the characters.
         * 
         *  \require (fState_ != State::eCompleted);
         *  \require ((fState_ == State::ePreparingHeaders) or (fState_ == State::ePreparingBodyBeforeHeadersSent) or (this->headers ().transferEncoding ()->Contains (HTTP::TransferEncoding::eChunked)));
         */
        nonvirtual void write (const BLOB& b);
        nonvirtual void write (const byte* start, const byte* end);
        nonvirtual void write (const wchar_t* e);
        nonvirtual void write (const wchar_t* s, const wchar_t* e);
        nonvirtual void write (const String& e);

    public:
        /**
         *  Creates a string, and indirects to write ()
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
            Require (fState_ == State::ePreparingHeaders);
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
            Require (fState_ == State::ePreparingHeaders);
            return std::forward<FUNCTION> (f) (&this->rwHeaders ());
        }
        template <typename FUNCTION>
        [[deprecated ("Since 2.1b10, use headers() directly")]] inline  auto ReadHeader (FUNCTION&& f) const
        {
            return std::forward<FUNCTION> (f) (this->headers ());
        }

    private:
        nonvirtual bool InChunkedMode_ () const;

    private:
        nonvirtual InternetMediaType AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const;

    private:
        IO::Network::Socket::Ptr                 fSocket_;
        bool                                     fInChunkedModeCache_{false};
        State                                    fState_{State::ePreparingHeaders};
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
