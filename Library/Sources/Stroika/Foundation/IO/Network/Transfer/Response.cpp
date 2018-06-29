/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Execution/Exceptions.h"
#include "../../../Execution/RequiredComponentMissingException.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"
#include "../HTTP/Methods.h"

#include "Response.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;

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

#if 0
void    Response::ThrowIfFailed () const
{
    HTTP::Exception::ThrowIfError (fStatus_);
}
#endif

InputStream<Byte>::Ptr Response::GetDataBinaryInputStream () const
{
    if (not fDataBinaryInputStream_.has_value ()) {
        fDataBinaryInputStream_ = fData_.As<InputStream<Byte>::Ptr> ();
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
