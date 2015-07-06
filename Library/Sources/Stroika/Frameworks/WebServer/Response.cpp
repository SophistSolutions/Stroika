/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>
#include    <sstream>

#include    "../../Foundation/Characters/CString/Utilities.h"
#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/IO/Network/HTTP/Exception.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Response.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



namespace   {
    const   set<String> kDisallowedOtherHeaders_    =   set<String> ({
        IO::Network::HTTP::HeaderName::kContentLength,
        IO::Network::HTTP::HeaderName::kContentType
    });
}





/*
 ********************************************************************************
 ************************ WebServer::Response *******************************
 ********************************************************************************
 */
namespace   {
    // Based on looking at a handlful of typical file sizes...5k to 80k, but ave around
    // 25 and median abit above 32k. Small, not very representative sampling. And the more we use
    // subscripts (script src=x) this number could shrink.
    //
    // MAY want to switch to using SmallStackBuffer<Byte> - but before doing so, do some cleanups of its bugs and make sure
    // its optimized about how much it copies etc. Its really only tuned for POD-types (OK here but not necessarily good about reallocs).
    //
    //      -- LGP 2011-07-06
    const   size_t  kMinResponseBufferReserve_              =   32 * 1024;
    const   size_t  kResponseBufferReallocChunkSizeReserve_ =   16 * 1024;
}

Response::Response (const IO::Network::Socket& s,  Streams::BinaryOutputStream<> outStream, const InternetMediaType& ct)
    : fSocket_ (s)
    , fState_ (State::eInProgress)
    , fStatus_ (StatusCodes::kOK)
    , fStatusOverrideReason_ ()
    , fUnderlyingOutStream_ (outStream)
    , fUseOutStream_ (outStream)
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
    Require (fState_ == State::eCompleted);
}

void    Response::SetContentSizePolicy (ContentSizePolicy csp)
{
    Require (csp == ContentSizePolicy::eAutoCompute or csp == ContentSizePolicy::eNone);
    fContentSizePolicy_ = csp;
}

void    Response::SetContentSizePolicy (ContentSizePolicy csp, uint64_t size)
{
    Require (csp == ContentSizePolicy::eExact);
    Require (fState_ == State::eInProgress);
    fContentSizePolicy_ = csp;
    fContentSize_ = size;
}

void    Response::SetContentType (const InternetMediaType& contentType)
{
    Require (fState_ == State::eInProgress);
    fContentType_ = contentType;
}

void    Response::SetCodePage (Characters::CodePage codePage)
{
    Require (fState_ == State::eInProgress);
    Require (fBytes_.empty ());
    fCodePage_ = codePage;
}

void    Response::SetStatus (Status newStatus, const String& overrideReason)
{
    Require (fState_ == State::eInProgress);
    fStatus_ = newStatus;
    fStatusOverrideReason_ = overrideReason;
}

void    Response::AddHeader (String headerName, String value)
{
    Require (fState_ == State::eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    ClearHeader (headerName);
    fHeaders_.insert (map<String, String>::value_type (headerName, value));
}

void    Response::ClearHeader ()
{
    Require (fState_ == State::eInProgress);
    fHeaders_.clear ();
}

void    Response::ClearHeader (String headerName)
{
    Require (fState_ == State::eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    map<String, String>::iterator i = fHeaders_.find (headerName);
    if (i != fHeaders_.end ()) {
        fHeaders_.erase (i);
    }
}

map<String, String>  Response::GetSpecialHeaders () const
{
    return fHeaders_;
}

map<String, String>  Response::GetEffectiveHeaders () const
{
    map<String, String>  tmp =   GetSpecialHeaders ();
    switch (GetContentSizePolicy ()) {
        case    ContentSizePolicy::eAutoCompute:
        case    ContentSizePolicy::eExact: {
                wostringstream  buf;
                buf << fContentSize_;
                tmp.insert (map<String, String>::value_type (IO::Network::HTTP::HeaderName::kContentLength, buf.str ()));
            }
            break;
    }
    if (not fContentType_.empty ()) {
        wstring contentTypeString   =   fContentType_.As<wstring> ();
        // Don't override already specifed characterset
        if (fContentType_.IsTextFormat () and contentTypeString.find (';') == wstring::npos) {
            contentTypeString += L"; charset=" + Characters::GetCharsetString (fCodePage_);

        }
        tmp.insert (map<String, String>::value_type (IO::Network::HTTP::HeaderName::kContentType, contentTypeString));
    }
    return tmp;
}

void    Response::Flush ()
{
    if (fState_ == State::eInProgress) {
        {
            String  statusMsg   =   fStatusOverrideReason_.empty () ? IO::Network::HTTP::Exception::GetStandardTextForStatus (fStatus_, true) : fStatusOverrideReason_;
            wstring version =   L"1.1";
            wstring tmp =   Characters::CString::Format (L"HTTP/%s %d %s\r\n", version.c_str (), fStatus_, statusMsg.c_str ());
            string  utf8    =   String (tmp).AsUTF8 ();
            fUseOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
        }

        {
            map<String, String>  headers2Write   =   GetEffectiveHeaders ();
            for (auto i = headers2Write.begin (); i != headers2Write.end (); ++i) {
                wstring tmp =   Characters::CString::Format (L"%s: %s\r\n", i->first.c_str (), i->second.c_str ());
                string  utf8    =   String (tmp).AsUTF8 ();
                fUseOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
            }
        }

        const char  kCRLF[] =   "\r\n";
        fUseOutStream_.Write (reinterpret_cast<const Byte*> (kCRLF), reinterpret_cast<const Byte*> (kCRLF + 2));
        fState_ = State::eInProgressHeaderSentState;
    }
    // write BYTES to fOutStream
    if (not fBytes_.empty ()) {
        Assert (fState_ != State::eCompleted); // We PREVENT any writes when completed
        fUseOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
        fBytes_.clear ();
    }
    if (fState_ != State::eCompleted) {
        fUseOutStream_.Flush ();
    }
    Ensure (fBytes_.empty ());
}

void    Response::End ()
{
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Flush ();
    fState_ = State::eCompleted;
    Ensure (fState_ == State::eCompleted);
    Ensure (fBytes_.empty ());
}

void    Response::Abort ()
{
    if (fState_ != State::eCompleted) {
        fState_ = State::eCompleted;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        fBytes_.clear ();
    }
    Ensure (fState_ == State::eCompleted);
    Ensure (fBytes_.empty ());
}

void    Response::Redirect (const String& url)
{
    Require (fState_ == State::eInProgress);
    fBytes_.clear ();

    // PERHAPS should clear some header values???
    AddHeader (String_Constant (L"Connection"), String_Constant (L"close"));    // needed for redirect
    AddHeader (String_Constant (L"Location"), url);           // needed for redirect
    SetStatus (StatusCodes::kMovedPermanently);
    Flush ();
    fState_ = State::eCompleted;
}

void    Response::write (const Byte* s, const Byte* e)
{
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

void    Response::write (const wchar_t* s, const wchar_t* e)
{
    Require ((fState_ == State::eInProgress) or (fState_ == State::eInProgressHeaderSentState));
    Require ((fState_ == State::eInProgress) or (GetContentSizePolicy () != ContentSizePolicy::eAutoCompute));
    Require (s <= e);
    if (s < e) {
        wstring tmp = wstring (s, e);
        string cpStr = Characters::WideStringToNarrow (tmp, fCodePage_);
        if (not cpStr.empty ()) {
            fBytes_.insert (fBytes_.end (), reinterpret_cast<const Byte*> (cpStr.c_str ()), reinterpret_cast<const Byte*> (cpStr.c_str () + cpStr.length ()));
            if (GetContentSizePolicy () == ContentSizePolicy::eAutoCompute) {
                // Because for autocompute - illegal to call flush and then write
                fContentSize_ = fBytes_.size ();
            }
        }
    }
}
