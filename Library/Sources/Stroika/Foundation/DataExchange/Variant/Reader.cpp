/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Streams/TextReader.h"
#include "../../Streams/iostream/InputStreamFromStdIStream.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using std::byte;

using Streams::iostream::InputStreamFromStdIStream;

/*
 ********************************************************************************
 ******************************* Variant::Reader ********************************
 ********************************************************************************
 */
Streams::InputStream<byte>::Ptr Variant::Reader::_ToByteReader (istream& in)
{
    return InputStreamFromStdIStream<byte>::New (in);
}

Streams::InputStream<Characters::Character>::Ptr Variant::Reader::_ToCharacterReader (const Traversal::Iterable<Characters::Character>& in)
{
    return Streams::TextReader::New (in);
}

Streams::InputStream<Characters::Character>::Ptr Variant::Reader::_ToCharacterReader (wistream& in)
{
    return InputStreamFromStdIStream<Characters::Character>::New (in);
}
