/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>
#include    <sstream>

#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/Containers/STL/SetUtils.h"
#include    "../../Foundation/DataExchangeFormat/BadFormatException.h"
#include    "../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/IO/Network/HTTP/Exception.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "HTTPResponse.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



namespace   {
    const   set<String> kDisallowedOtherHeaders_    =   Containers::STL::mkS<String> (
                IO::Network::HTTP::HeaderName::kContentLength,
                IO::Network::HTTP::HeaderName::kContentType
            );
}





/*
 ********************************************************************************
 ************************ WebServer::HTTPResponse *******************************
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

HTTPResponse::HTTPResponse (const IO::Network::Socket& s,  Streams::BinaryOutputStream outStream, const InternetMediaType& ct)
    : fSocket_ (s)
    , fUnderlyingOutStream_ (outStream)
    , fUseOutStream_ (outStream)
    , fState_ (eInProgress)
    , fStatus_ (StatusCodes::kOK)
    , fStatusOverrideReason_ ()
    , fHeaders_ ()
    , fContentType_ (ct)
    , fCodePage_ (Characters::kCodePage_UTF8)
    , fBytes_ ()
    , fContentSizePolicy_ (eAutoCompute_CSP)
    , fContentSize_ (0)
{
    AddHeader (IO::Network::HTTP::HeaderName::kServer, L"Stroka-Based-Web-Server");
}

HTTPResponse::~HTTPResponse ()
{
    Require (fState_ == eCompleted);
}

void    HTTPResponse::SetContentSizePolicy (ContentSizePolicy csp)
{
    Require (csp == eAutoCompute_CSP or csp == eNone_CSP);
    fContentSizePolicy_ = csp;
}

void    HTTPResponse::SetContentSizePolicy (ContentSizePolicy csp, uint64_t size)
{
    Require (csp == eExact_CSP);
    Require (fState_ == eInProgress);
    fContentSizePolicy_ = csp;
    fContentSize_ = size;
}

void    HTTPResponse::SetContentType (const InternetMediaType& contentType)
{
    Require (fState_ == eInProgress);
    fContentType_ = contentType;
}

void    HTTPResponse::SetCodePage (Characters::CodePage codePage)
{
    Require (fState_ == eInProgress);
    Require (fBytes_.empty ());
    fCodePage_ = codePage;
}

void    HTTPResponse::SetStatus (Status newStatus, const String& overrideReason)
{
    Require (fState_ == eInProgress);
    fStatus_ = newStatus;
    fStatusOverrideReason_ = overrideReason;
}

void    HTTPResponse::AddHeader (String headerName, String value)
{
    Require (fState_ == eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    ClearHeader (headerName);
    fHeaders_.insert (map<String, String>::value_type (headerName, value));
}

void    HTTPResponse::ClearHeader ()
{
    Require (fState_ == eInProgress);
    fHeaders_.clear ();
}

void    HTTPResponse::ClearHeader (String headerName)
{
    Require (fState_ == eInProgress);
    Require (kDisallowedOtherHeaders_.find (headerName) == kDisallowedOtherHeaders_.end ());
    map<String, String>::iterator i = fHeaders_.find (headerName);
    if (i != fHeaders_.end ()) {
        fHeaders_.erase (i);
    }
}

map<String, String>  HTTPResponse::GetSpecialHeaders () const
{
    return fHeaders_;
}

map<String, String>  HTTPResponse::GetEffectiveHeaders () const
{
    map<String, String>  tmp =   GetSpecialHeaders ();
    switch (GetContentSizePolicy ()) {
        case    eAutoCompute_CSP:
        case    eExact_CSP: {
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

void    HTTPResponse::Flush ()
{
    if (fState_ == eInProgress) {
        {
            String  statusMsg   =   fStatusOverrideReason_.empty () ? IO::Network::HTTP::Exception::GetStandardTextForStatus (fStatus_, true) : fStatusOverrideReason_;
            wstring version =   L"1.1";
            wstring tmp =   Characters::Format (L"HTTP/%s %d %s\r\n", version.c_str (), fStatus_, statusMsg.c_str ());
            string  utf8    =   String (tmp).AsUTF8 ();
            fUseOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
        }

        {
            map<String, String>  headers2Write   =   GetEffectiveHeaders ();
            for (map<String, String>::const_iterator i = headers2Write.begin (); i != headers2Write.end (); ++i) {
                wstring tmp =   Characters::Format (L"%s: %s\r\n", i->first.c_str (), i->second.c_str ());
                string  utf8    =   String (tmp).AsUTF8 ();
                fUseOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
            }
        }

        const char  kCRLF[] =   "\r\n";
        fUseOutStream_.Write (reinterpret_cast<const Byte*> (kCRLF), reinterpret_cast<const Byte*> (kCRLF + 2));
        fState_ = eInProgressHeaderSentState;
    }
    // write BYTES to fOutStream
    if (not fBytes_.empty ()) {
        Assert (fState_ != eCompleted); // We PREVENT any writes when completed
        fUseOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
        fBytes_.clear ();
    }
    if (fState_ != eCompleted) {
        fUseOutStream_.Flush ();
    }
    Ensure (fBytes_.empty ());
}

void    HTTPResponse::End ()
{
    Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderSentState));
    Flush ();
    fState_ = eCompleted;
    Ensure (fState_ == eCompleted);
    Ensure (fBytes_.empty ());
}

void    HTTPResponse::Abort ()
{
    if (fState_ != eCompleted) {
        fState_ = eCompleted;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        fBytes_.clear ();
    }
    Ensure (fState_ == eCompleted);
    Ensure (fBytes_.empty ());
}

void    HTTPResponse::Redirect (const String& url)
{
    Require (fState_ == eInProgress);
    fBytes_.clear ();

    // PERHAPS should clear some header values???
    AddHeader (L"Connection", L"close");    // needed for redirect
    AddHeader (L"Location", url);           // needed for redirect
    SetStatus (StatusCodes::kMovedPermanently);
    Flush ();
    fState_ = eCompleted;
}

void    HTTPResponse::write (const Byte* s, const Byte* e)
{
    Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderSentState));
    Require ((fState_ == eInProgress) or (GetContentSizePolicy () != eAutoCompute_CSP));
    Require (s <= e);
    if (s < e) {
        Containers::ReserveSpeedTweekAddN (fBytes_, (e - s), kResponseBufferReallocChunkSizeReserve_);
        fBytes_.insert (fBytes_.end (), s, e);
        if (GetContentSizePolicy () == eAutoCompute_CSP) {
            // Because for autocompute - illegal to call flush and then write
            fContentSize_ = fBytes_.size ();
        }
    }
}

void    HTTPResponse::write (const wchar_t* s, const wchar_t* e)
{
    Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderSentState));
    Require ((fState_ == eInProgress) or (GetContentSizePolicy () != eAutoCompute_CSP));
    Require (s <= e);
    if (s < e) {
        wstring tmp = wstring (s, e);
        string cpStr = Characters::WideStringToNarrow (tmp, fCodePage_);
        if (not cpStr.empty ()) {
            fBytes_.insert (fBytes_.end (), reinterpret_cast<const Byte*> (cpStr.c_str ()), reinterpret_cast<const Byte*> (cpStr.c_str () + cpStr.length ()));
            if (GetContentSizePolicy () == eAutoCompute_CSP) {
                // Because for autocompute - illegal to call flush and then write
                fContentSize_ = fBytes_.size ();
            }
        }
    }
}
