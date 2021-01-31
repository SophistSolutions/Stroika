/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "Response.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using IO::Network::HTTP::ClientErrorException;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 **************************** WebServer::Response *******************************
 ********************************************************************************
 */
namespace {
    // Based on looking at a handlful of typical file sizes...5k to 80k, but ave around
    // 25 and median abit above 32k. Small, not very representative sampling. And the more we use
    // subscripts (script src=x) this number could shrink.
    //
    // MAY want to switch to using SmallStackBuffer<byte> - but before doing so, do some cleanups of its bugs and make sure
    // its optimized about how much it copies etc. Its really only tuned for POD-types (OK here but not necessarily good about reallocs).
    //
    //      -- LGP 2011-07-06
    constexpr size_t kResponseBufferReallocChunkSizeReserve_ = 16 * 1024;
}

Response::Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const InternetMediaType& ct)
    : fSocket_{s}
    , fState_{State::eInProgress}
    , fStatus_{StatusCodes::kOK}
    , fStatusOverrideReason_{}
    , fUnderlyingOutStream_{outStream}
    , fUseOutStream_{Streams::BufferedOutputStream<byte>::New (outStream)}
    , fHeaders_{}
    , fCodePage_{Characters::kCodePage_UTF8}
    , fBytes_{}
    , fContentSizePolicy_{ContentSizePolicy::eAutoCompute}
{
    fHeaders_.pServer = L"Stroka-Based-Web-Server"sv;
    if (not ct.empty ()) {
        fHeaders_.pContentType = ct;
    }
}

InternetMediaType Response::GetContentType () const
{
    // DEPRECATED
    return ReadHeader ([] (const auto& h) { return h.pContentType ().value_or (InternetMediaType{}); });
}

void Response::AddHeader (const String& headerName, const String& value)
{
    // DEPRECATED
    UpdateHeader ([&] (IO::Network::HTTP::Headers* headers) { headers->Set (headerName, value); });
}

void Response::SetContentSizePolicy (ContentSizePolicy csp)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (csp == ContentSizePolicy::eAutoCompute or csp == ContentSizePolicy::eNone);
    fContentSizePolicy_ = csp;
}

void Response::SetContentSizePolicy (ContentSizePolicy csp, uint64_t size)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (csp == ContentSizePolicy::eExact);
    Require (fState_ == State::eInProgress);
    fContentSizePolicy_      = csp;
    fHeaders_.pContentLength = size;
}

bool Response::IsContentLengthKnown () const
{
    return fContentSizePolicy_ != ContentSizePolicy::eNone;
}

void Response::SetContentType (const InternetMediaType& contentType)
{
    UpdateHeader ([=, this] (auto* header) {
        if (auto ct = header->pContentType ()) {
            header->pContentType = AdjustContentTypeForCodePageIfNeeded_ (contentType);
        }
    });
}

void Response::SetCodePage (Characters::CodePage codePage)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    Require (fBytes_.empty ());
    bool diff  = fCodePage_ != codePage;
    fCodePage_ = codePage;
    if (diff) {
        if (auto ct = fHeaders_.pContentType ()) {
            fHeaders_.pContentType = AdjustContentTypeForCodePageIfNeeded_ (*ct);
        }
    }
}

void Response::SetStatus (Status newStatus, const String& overrideReason)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fStatus_               = newStatus;
    fStatusOverrideReason_ = overrideReason;
}

void Response::AppendToCommaSeperatedHeader (const String& headerName, const String& value)
{
    Require (not value.empty ());
    UpdateHeader ([&] (IO::Network::HTTP::Headers* headers) {
        RequireNotNull (headers);
        if (auto o = headers->LookupOne (headerName)) {
            if (o->empty ()) {
                headers->Add (headerName, value);
            }
            else {
                headers->Add (headerName, *o + L", "sv + value);
            }
        }
        else {
            headers->Add (headerName, value);
        }
    });
}

void Response::ClearHeaders ()
{
    UpdateHeader ([&] (IO::Network::HTTP::Headers* headers) {
        RequireNotNull (headers);
        *headers = IO::Network::HTTP::Headers{};
    });
}

void Response::ClearHeader (const String& headerName)
{
    UpdateHeader ([&] (IO::Network::HTTP::Headers* headers) {
        RequireNotNull (headers);
        headers->Remove (headerName);
    });
}

IO::Network::HTTP::Headers Response::GetHeaders () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return fHeaders_;
}

InternetMediaType Response::AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const
{
    if (DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (ct)) {
        using AtomType = InternetMediaType::AtomType;
        // Don't override already specifed characterset
        Containers::Mapping<String, String> params = ct.GetParameters ();
        params.Add (L"charset"sv, Characters::GetCharsetString (fCodePage_), AddReplaceMode::eAddIfMissing);
        return InternetMediaType{ct.GetType<AtomType> (), ct.GetSubType<AtomType> (), ct.GetSuffix (), params};
    }
    return ct;
}

void Response::Flush ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"Response::Flush"};
    DbgTrace (L"fState_ = %s", Characters::ToString (fState_).c_str ());
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fState_ == State::eInProgress) {
        {
            String  statusMsg = fStatusOverrideReason_.empty () ? IO::Network::HTTP::Exception::GetStandardTextForStatus (fStatus_, true) : fStatusOverrideReason_;
            wstring version   = L"1.1";
            wstring tmp       = Characters::CString::Format (L"HTTP/%s %d %s\r\n", version.c_str (), fStatus_, statusMsg.c_str ());
            string  utf8      = String (tmp).AsUTF8 ();
            fUseOutStream_.Write (reinterpret_cast<const byte*> (Containers::Start (utf8)), reinterpret_cast<const byte*> (Containers::End (utf8)));
        }

        {
            for (const auto& i : GetHeaders ().As<> ()) {
                string utf8 = Characters::Format (L"%s: %s\r\n", i.fKey.c_str (), i.fValue.c_str ()).AsUTF8 ();
                fUseOutStream_.Write (reinterpret_cast<const byte*> (Containers::Start (utf8)), reinterpret_cast<const byte*> (Containers::End (utf8)));
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"headers: %s", Characters::ToString (headers2Write).c_str ());
#endif
        }

        const char kCRLF[] = "\r\n";
        fUseOutStream_.Write (reinterpret_cast<const byte*> (kCRLF), reinterpret_cast<const byte*> (kCRLF + 2));
        fState_ = State::eInProgressHeaderSentState;
    }
    // write BYTES to fOutStream
    if (not fBytes_.empty ()) {
        Assert (fState_ != State::eCompleted); // We PREVENT any writes when completed
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"bytes.size: %lld", static_cast<long long> (fBytes_.size ()));
#endif
        fUseOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
        fBytes_.clear ();
    }
    if (fState_ != State::eCompleted) {
        fUseOutStream_.Flush ();
    }
    Ensure (fBytes_.empty ());
}

void Response::End ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Flush ();
    fState_ = State::eCompleted;
    Ensure (fState_ == State::eCompleted);
    Ensure (fBytes_.empty ());
}

void Response::Abort ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fState_ != State::eCompleted) {
        fState_ = State::eCompleted;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        fBytes_.clear ();
    }
    Ensure (fState_ == State::eCompleted);
    Ensure (fBytes_.empty ());
}

void Response::Redirect (const URI& url)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fBytes_.clear ();

    // PERHAPS should clear some header values???
    fHeaders_.pConnection = IO::Network::HTTP::Headers::eClose;
    fHeaders_.pLocation   = url;
    SetStatus (StatusCodes::kMovedPermanently);
    Flush ();
    fState_ = State::eCompleted;
}

void Response::write (const byte* s, const byte* e)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Require ((fState_ == State::eInProgress) or (GetContentSizePolicy () != ContentSizePolicy::eAutoCompute));
    Require (s <= e);
    if (s < e) {
        Containers::ReserveSpeedTweekAddN (fBytes_, (e - s), kResponseBufferReallocChunkSizeReserve_);
        fBytes_.insert (fBytes_.end (), s, e);
        if (GetContentSizePolicy () == ContentSizePolicy::eAutoCompute) {
            // Because for autocompute - illegal to call flush and then write
            fHeaders_.pContentLength = fBytes_.size ();
        }
    }
}

void Response::write (const wchar_t* s, const wchar_t* e)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Require ((fState_ == State::eInProgress) or (GetContentSizePolicy () != ContentSizePolicy::eAutoCompute));
    Require (s <= e);
    if (s < e) {
        wstring tmp   = wstring (s, e);
        string  cpStr = Characters::WideStringToNarrow (tmp, fCodePage_);
        if (not cpStr.empty ()) {
            fBytes_.insert (fBytes_.end (), reinterpret_cast<const byte*> (cpStr.c_str ()), reinterpret_cast<const byte*> (cpStr.c_str () + cpStr.length ()));
            if (GetContentSizePolicy () == ContentSizePolicy::eAutoCompute) {
                // Because for autocompute - illegal to call flush and then write
                fHeaders_.pContentLength = fBytes_.size ();
            }
        }
    }
}

void Response::printf (const wchar_t* format, ...)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    va_list                                            argsList;
    va_start (argsList, format);
    String tmp = Characters::FormatV (format, argsList);
    va_end (argsList);
    write (tmp);
}

String Response::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"Socket: " + Characters::ToString (fSocket_) + L", ";
    sb += L"State_: " + Characters::ToString (fState_) + L", ";
    sb += L"StatusOverrideReason_: '" + Characters::ToString (fStatusOverrideReason_) + L"', ";
    sb += L"Headers: " + Characters::ToString (GetHeaders ()) + L", ";
    sb += L"}";
    return sb.str ();
}
