/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;
using namespace Stroika::Foundation::Traversal;

using std::byte;

/*
 ********************************************************************************
 ******************************* Variant::Reader ********************************
 ********************************************************************************
 */
InputStream::Ptr<byte> Variant::Reader::_ToByteReader (istream& in)
{
    return InputStreamFromStdIStream::New<byte> (in);
}

InputStream::Ptr<Character> Variant::Reader::_ToCharacterReader (const Iterable<Character>& in)
{
    return Streams::BinaryToText::Reader::New (in);
}

InputStream::Ptr<Character> Variant::Reader::_ToCharacterReader (wistream& in)
{
    return InputStreamFromStdIStream::New<Character> (in);
}
