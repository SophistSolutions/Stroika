/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Streams/TextReader.h"
#include "../../Streams/iostream/InputStreamFromStdIStream.h"

#include "Reader.h"

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
    return Read (Streams::TextReader (in));
}

VariantValue Variant::Reader::Read (const Memory::BLOB& in)
{
    return Read (in.As<Streams::InputStream<Memory::Byte>::Ptr> ());
}

VariantValue Variant::Reader::Read (istream& in)
{
    return Read (Streams::TextReader (InputStreamFromStdIStream<Memory::Byte>{in}, true));
}

VariantValue Variant::Reader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character>{in});
}
