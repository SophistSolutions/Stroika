/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/CodeCvt.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/StringBuilder.h"
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
    sb += "{";
    sb += "Subject-Common-Name: " + Characters::ToString (fSubjectCommonName) + ",";
    sb += "Subject-Company-Name: " + Characters::ToString (fSubjectCompanyName) + ",";
    sb += "Issuer: " + Characters::ToString (fIssuer) + L",";
    sb += "Validation-Status: " + Characters::ToString (fValidationStatus) + ",";
    sb += "}";
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

optional<String> Response::GetCharset () const
{
    if (optional<String> hi = fHeaders_.Lookup (String::FromStringConstant (HTTP::HeaderName::kContentType))) {
        static const String kLBL_ = "; charset="_k;
        if (optional<size_t> oi = hi->Find (kLBL_)) {
            return hi->SubString (*oi + kLBL_.size ());
        }
    }
    return nullopt;
}

String Response::ToString () const
{
    StringBuilder sb;
    sb += "{";
    sb += "Headers: " + Characters::ToString (fHeaders_) + ",";
    sb += "Status: " + Characters::ToString (fStatus_) + ",";
    sb += "ServerEndpointSSLInfo_: " + Characters::ToString (fServerEndpointSSLInfo_) + ",";
    sb += "}";
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
        // @todo NOTE - in Stroika v2.1  - this parameter to TextReader::New was treated as a 'charset' but since 3.0d1, its 
        // a 'locale name'. I dont think the mapping between charset and locale names is well defined/portable. But IF ever needed to be adjusted/
        // fixed, this is the place todo that mapping...
        fDataTextInputStream_ = Streams::TextReader::New (GetDataBinaryInputStream (), GetCharset ()? Characters::CodeCvt<> {*GetCharset ()}: Characters::CodeCvt<>{});
    }
    return *fDataTextInputStream_;
}
