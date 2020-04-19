/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Characters/ToString.h"
#include "../../../Execution/RequiredComponentMissingException.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"
#include "../HTTP/Methods.h"

#include "Response.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ********************* Transfer::Response::SSLResultInfo ************************
 ********************************************************************************
 */
String Response::SSLResultInfo::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Subject-Common-Name: " + Characters::ToString (fSubjectCommonName) + L",";
    sb += L"Subject-Company-Name: " + Characters::ToString (fSubjectCompanyName) + L",";
    sb += L"Issuer: " + Characters::ToString (fIssuer) + L",";
    sb += L"Validation-Status: " + Characters::ToString (fValidationStatus) + L",";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
optional<InternetMediaType> Response::GetContentType () const
{
    if (optional<String> i = fHeaders_.Lookup (String_Constant (HTTP::HeaderName::kContentType))) {
        return InternetMediaType{*i};
    }
    return {};
}

optional<String> Response::GetCharset () const
{
    if (optional<String> hi = fHeaders_.Lookup (String_Constant (HTTP::HeaderName::kContentType))) {
        static const String_Constant kLBL_{L"; charset="};
        if (optional<size_t> oi = hi->Find (kLBL_)) {
            return hi->SubString (*oi + kLBL_.size ());
        }
    }
    return {};
}

String Response::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Headers: " + Characters::ToString (fHeaders_) + L",";
    sb += L"Status: " + Characters::ToString (fStatus_) + L",";
    sb += L"fServerEndpointSSLInfo_: " + Characters::ToString (fServerEndpointSSLInfo_) + L",";
    sb += L"}";
    return sb.str ();
}

InputStream<byte>::Ptr Response::GetDataBinaryInputStream () const
{
    if (not fDataBinaryInputStream_.has_value ()) {
        fDataBinaryInputStream_ = fData_.As<InputStream<byte>::Ptr> ();
    }
    return *fDataBinaryInputStream_;
}

InputStream<Character>::Ptr Response::GetDataTextInputStream () const
{
    if (not fDataTextInputStream_.has_value ()) {
        fDataTextInputStream_ = Streams::TextReader::New (GetDataBinaryInputStream (), GetCharset ());
    }
    return *fDataTextInputStream_;
}
