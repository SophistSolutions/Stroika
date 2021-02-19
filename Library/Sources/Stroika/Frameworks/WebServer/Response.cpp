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

Response::Response (Response&& src)
    : Response{src.fSocket_, src.fUnderlyingOutStream_}
{
    fState_             = src.fState_;
    fUseOutStream_      = src.fUseOutStream_;
    fCodePage_          = src.fCodePage_;
    fBodyBytes_         = src.fBodyBytes_;
    fContentSizePolicy_ = src.fContentSizePolicy_;
    fHeadMode_          = src.fHeadMode_;
}

Response::Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const optional<InternetMediaType>& ct)
    : codePage{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Response*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::codePage);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fCodePage_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCodePage) {
              Response*                                          thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::codePage);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              Require (thisObj->fState_ == State::eInProgress);
              Require (thisObj->fBodyBytes_.empty ());
              bool diff           = thisObj->fCodePage_ != newCodePage;
              thisObj->fCodePage_ = newCodePage;
              if (diff) {
                  if (auto ct = thisObj->headers ().contentType ()) {
                      thisObj->rwHeaders ().contentType = thisObj->AdjustContentTypeForCodePageIfNeeded_ (*ct);
                  }
              }
          }}
    , state{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Response*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::state);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fState_;
          }}
    , fSocket_{s}
    , fState_{State::eInProgress}
    , fUnderlyingOutStream_{outStream}
    , fUseOutStream_{Streams::BufferedOutputStream<byte>::New (outStream)}
    , fCodePage_{Characters::kCodePage_UTF8}
    , fBodyBytes_{}
    , fContentSizePolicy_{ContentSizePolicy::eAutoCompute}
{
#if qDebug
    this->status.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) { Require (fState_ == State::eInProgress); return true; });
    this->statusAndOverrideReason.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) { Require (fState_ == State::eInProgress); return true; });
    this->rwHeaders.rwPropertyReadHandlers ().push_front ([this] () { Require (fState_ == State::eInProgress); return nullopt; });
    this->rwHeaders.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) { Require (fState_ == State::eInProgress); return true; });
#endif
    this->contentType.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
        this->rwHeaders ().contentType = propertyChangedEvent.fNewValue ? AdjustContentTypeForCodePageIfNeeded_ (*propertyChangedEvent.fNewValue) : optional<InternetMediaType>{};
        return false; // cut-off - handled
    });
    rwHeaders ().server      = L"Stroka-Based-Web-Server"_k;
    rwHeaders ().contentType = ct;
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
    fContentSizePolicy_              = csp;
    this->rwHeaders ().contentLength = size;
}

bool Response::IsContentLengthKnown () const
{
    return fContentSizePolicy_ != ContentSizePolicy::eNone;
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
            auto    curStatusInfo = this->statusAndOverrideReason ();
            Status  curStatus     = get<0> (curStatusInfo);
            String  statusMsg     = Memory::NullCoalesce (get<1> (curStatusInfo), IO::Network::HTTP::Exception::GetStandardTextForStatus (curStatus, true));
            wstring version       = L"1.1";
            wstring tmp           = Characters::CString::Format (L"HTTP/%s %d %s\r\n", version.c_str (), curStatus, statusMsg.c_str ());
            string  utf8          = String (tmp).AsUTF8 ();
            fUseOutStream_.Write (reinterpret_cast<const byte*> (Containers::Start (utf8)), reinterpret_cast<const byte*> (Containers::End (utf8)));
        }

        {
            for (const auto& i : this->headers ().As<> ()) {
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
    if (not fBodyBytes_.empty ()) {
        Assert (fState_ != State::eCompleted); // We PREVENT any writes when completed
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"bytes.size: %lld", static_cast<long long> (fBodyBytes_.size ()));
#endif
        if (not fHeadMode_) {
            fUseOutStream_.Write (Containers::Start (fBodyBytes_), Containers::End (fBodyBytes_));
        }
        fBodyBytes_.clear ();
    }
    if (fState_ != State::eCompleted) {
        fUseOutStream_.Flush ();
    }
    Ensure (fBodyBytes_.empty ());
}

void Response::End ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Flush ();
    fState_ = State::eCompleted;
    Ensure (fState_ == State::eCompleted);
    Ensure (fBodyBytes_.empty ());
}

void Response::Abort ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fState_ != State::eCompleted) {
        fState_ = State::eCompleted;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        fBodyBytes_.clear ();
    }
    Ensure (fState_ == State::eCompleted);
    Ensure (fBodyBytes_.empty ());
}

void Response::Redirect (const URI& url)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fBodyBytes_.clear ();

    // PERHAPS should clear some header values???
    auto& updatableHeaders      = this->rwHeaders ();
    updatableHeaders.connection = IO::Network::HTTP::Headers::eClose;
    updatableHeaders.location   = url;
    this->status                = HTTP::StatusCodes::kMovedPermanently;
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
        Containers::ReserveSpeedTweekAddN (fBodyBytes_, (e - s), kResponseBufferReallocChunkSizeReserve_);
        fBodyBytes_.insert (fBodyBytes_.end (), s, e);
        if (GetContentSizePolicy () == ContentSizePolicy::eAutoCompute) {
            // Because for autocompute - illegal to call flush and then write
            rwHeaders ().contentLength = fBodyBytes_.size ();
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
            fBodyBytes_.insert (fBodyBytes_.end (), reinterpret_cast<const byte*> (cpStr.c_str ()), reinterpret_cast<const byte*> (cpStr.c_str () + cpStr.length ()));
            if (GetContentSizePolicy () == ContentSizePolicy::eAutoCompute) {
                // Because for autocompute - illegal to call flush and then write
                rwHeaders ().contentLength = fBodyBytes_.size ();
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
    StringBuilder                                       sb = inherited::ToString ().SubString (0, -1); // strip trailing '}'
    sb += L"Socket: " + Characters::ToString (fSocket_) + L", ";
    sb += L"State_: " + Characters::ToString (fState_) + L", ";
    sb += L"}";
    return sb.str ();
}
