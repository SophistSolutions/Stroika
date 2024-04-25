/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Response.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ********************* Transfer::Response::SSLResultInfo ************************
 ********************************************************************************
 */
String Response::SSLResultInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Subject-Common-Name: "sv << fSubjectCommonName << ","sv;
    sb << "Subject-Company-Name: "sv << fSubjectCompanyName << ","sv;
    sb << "Issuer: "sv << fIssuer << ","sv;
    sb << "Validation-Status: "sv << fValidationStatus << ","sv;
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
optional<InternetMediaType> Response::GetContentType () const
{
    if (optional<String> i = fHeaders_.Lookup (String::FromStringConstant (HTTP::HeaderName::kContentType))) {
        return InternetMediaType{*i};
    }
    return nullopt;
}

optional<Charset> Response::GetCharset () const
{
    if (optional<String> hi = fHeaders_.Lookup (String::FromStringConstant (HTTP::HeaderName::kContentType))) {
        static const String kLBL_ = "; charset="_k;
        if (optional<size_t> oi = hi->Find (kLBL_)) {
            return Charset{hi->SubString (*oi + kLBL_.size ())};
        }
    }
    return nullopt;
}

String Response::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Headers: "sv << fHeaders_ << ","sv;
    sb << "Status: "sv << fStatus_ << ","sv;
    sb << "ServerEndpointSSLInfo_: "sv << fServerEndpointSSLInfo_ << ","sv;
    sb << "}"sv;
    return sb.str ();
}

InputStream::Ptr<byte> Response::GetDataBinaryInputStream () const
{
    if (not fDataBinaryInputStream_.has_value ()) {
        fDataBinaryInputStream_ = fData_.As<InputStream::Ptr<byte>> ();
    }
    return *fDataBinaryInputStream_;
}

InputStream::Ptr<Character> Response::GetDataTextInputStream () const
{
    if (not fDataTextInputStream_.has_value ()) {
        fDataTextInputStream_ = Streams::TextReader::New (GetDataBinaryInputStream (),
                                                          GetCharset () ? Characters::CodeCvt<>{*GetCharset ()} : Characters::CodeCvt<>{});
    }
    return *fDataTextInputStream_;
}
