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
using namespace Streams::iostream;

using std::byte;

/*
 ********************************************************************************
 ******************************* Variant::Reader ********************************
 ********************************************************************************
 */
Streams::InputStream::Ptr<byte> Variant::Reader::_ToByteReader (istream& in)
{
    return InputStreamFromStdIStream::New<byte> (in);
}

Streams::InputStream::Ptr<Characters::Character> Variant::Reader::_ToCharacterReader (const Traversal::Iterable<Characters::Character>& in)
{
    return Streams::TextReader::New (in);
}

Streams::InputStream::Ptr<Characters::Character> Variant::Reader::_ToCharacterReader (wistream& in)
{
    return InputStreamFromStdIStream::New<Characters::Character> (in);
}
