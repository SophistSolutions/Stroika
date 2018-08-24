/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/String_Constant.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Exceptions.h"
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

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    const set<String> kDisallowedOtherHeaders_ = set<String> ({IO::Network::HTTP::HeaderName::kContentLength,
                                                               IO::Network::HTTP::HeaderName::kContentType});
}

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<Frameworks::WebServer::Response::State> DefaultNames<Frameworks::WebServer::Response::State>::k;
}
#endif

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
    constexpr size_t kMinResponseBufferReserve_              = 32 * 1024;
    constexpr size_t kResponseBufferReallocChunkSizeReserve_ = 16 * 1024;
}

Response::Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const InternetMediaType& ct)
    : fSocket_ (s)
    , fState_ (State::eInProgress)
    , fStatus_ (StatusCodes::kOK)
    , fStatusOverrideReason_ ()
    , fUnderlyingOutStream_ (outStream)
    , fUseOutStream_ (Streams::BufferedOutputStream<byte>::New (outStream))
    , fHeaders_ ()
    , fContentType_ (ct)
    , fCodePage_ (Characters::kCodePage_UTF8)
    , fBytes_ ()
    , fContentSizePolicy_ (ContentSizePolicy::eAutoCompute)
    , fContentSize_ (0)
{
    AddHeader (IO::Network::HTTP::HeaderName::kServer, String_Constant (L"Stroka-Based-Web-Server"));
}

Response::~Response ()
{
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
    fContentSizePolicy_ = csp;
    fContentSize_       = size;
}

bool Response::IsContentLengthKnown () const
{
    return fContentSizePolicy_ != ContentSizePolicy::eNone;
}

void Response::SetContentType (const InternetMediaType& contentType)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fContentType_ = contentType;
}

void Response::SetCodePage (Characters::CodePage codePage)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    Require (fBytes_.empty ());
    fCodePage_ = codePage;
}

void Response::SetStatus (Status newStatus, const String& overrideReason)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fStatus_               = newStatus;
    fStatusOverrideReason_ = overrideReason;
}

void Response::AddHeader (const String& headerName, const String& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    fHeaders_.Add (headerName, value);
}

void Response::AppendToCommaSeperatedHeader (const String& headerName, const String& value)
{
    Require (not value.empty ());
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (auto o = fHeaders_.Lookup (headerName)) {
        if (o->empty ()) {
            AddHeader (headerName, value);
        }
        else {
            AddHeader (headerName, *o + String_Constant{L", "} + value);
        }
    }
    else {
        AddHeader (headerName, value);
    }
}

void Response::ClearHeaders ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fHeaders_.clear ();
}

void Response::ClearHeader (const String& headerName)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    fHeaders_.Remove (headerName);
}

Mapping<String, String> Response::GetHeaders () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return fHeaders_;
}

Mapping<String, String> Response::GetEffectiveHeaders () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    Mapping<String, String>                             tmp = GetHeaders ();
    switch (GetContentSizePolicy ()) {
        case ContentSizePolicy::eAutoCompute:
        case ContentSizePolicy::eExact: {
            wostringstream buf;
            buf << fContentSize_;
            tmp.Add (IO::Network::HTTP::HeaderName::kContentLength, buf.str ());
        } break;
    }
    if (not fContentType_.empty ()) {
        wstring contentTypeString = fContentType_.As<wstring> ();
        // Don't override already specifed characterset
        if (fContentType_.IsTextFormat () and contentTypeString.find (';') == wstring::npos) {
            contentTypeString += L"; charset=" + Characters::GetCharsetString (fCodePage_);
        }
        tmp.Add (IO::Network::HTTP::HeaderName::kContentType, contentTypeString);
    }
    return tmp;
}

void Response::Flush ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Response::Flush");
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
            Mapping<String, String> headers2Write = GetEffectiveHeaders ();
            for (auto i : headers2Write) {
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

void Response::Redirect (const String& url)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fState_ == State::eInProgress);
    fBytes_.clear ();

    // PERHAPS should clear some header values???
    AddHeader (String_Constant (L"Connection"), String_Constant (L"close")); // needed for redirect
    AddHeader (String_Constant (L"Location"), url);                          // needed for redirect
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
            fContentSize_ = fBytes_.size ();
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
                fContentSize_ = fBytes_.size ();
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
    sb += L"Headers: " + Characters::ToString (GetEffectiveHeaders ()) + L", ";
    sb += L"}";
    return sb.str ();
}
