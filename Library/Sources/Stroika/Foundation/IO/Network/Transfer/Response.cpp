/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
Optional<InternetMediaType> Response::GetContentType () const
{
    if (Optional<String> i = fHeaders_.Lookup (String_Constant (HTTP::HeaderName::kContentType))) {
        return InternetMediaType{*i};
    }
    return {};
}

Optional<String> Response::GetCharset () const
{
    if (Optional<String> hi = fHeaders_.Lookup (String_Constant (HTTP::HeaderName::kContentType))) {
        static const String_Constant kLBL_{L"; charset="};
        if (Optional<size_t> oi = hi->Find (kLBL_)) {
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

InputStream<Byte> Response::GetDataBinaryInputStream () const
{
    if (fDataBinaryInputStream_.IsMissing ()) {
        fDataBinaryInputStream_ = fData_.As<InputStream<Byte>> ();
    }
    return *fDataBinaryInputStream_;
}

InputStream<Character> Response::GetDataTextInputStream () const
{
    if (fDataTextInputStream_.IsMissing ()) {
        fDataTextInputStream_ = Streams::TextReader (GetDataBinaryInputStream (), GetCharset ());
    }
    return *fDataTextInputStream_;
}
