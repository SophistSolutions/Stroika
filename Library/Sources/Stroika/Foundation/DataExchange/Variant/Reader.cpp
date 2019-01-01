/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Streams/TextReader.h"
#include "../../Streams/iostream/InputStreamFromStdIStream.h"

#include "Reader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Streams::iostream::InputStreamFromStdIStream;

/*
 ********************************************************************************
 ******************************* Variant::Reader ********************************
 ********************************************************************************
 */
VariantValue Variant::Reader::Read (const Traversal::Iterable<Characters::Character>& in)
{
    return Read (Streams::TextReader::New (in));
}

VariantValue Variant::Reader::Read (const Memory::BLOB& in)
{
    return Read (in.As<Streams::InputStream<byte>::Ptr> ());
}

VariantValue Variant::Reader::Read (istream& in)
{
    return Read (Streams::TextReader::New (InputStreamFromStdIStream<byte>::New (in), Streams::SeekableFlag::eSeekable));
}

VariantValue Variant::Reader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character>::New (in));
}
