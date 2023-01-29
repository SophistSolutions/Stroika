/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/InlineBuffer.h"
#include "../Memory/StackBuffer.h"
#include "MemoryStream.h"

#include "TextToByteReader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Debug::AssertExternallySynchronizedMutex;
using Memory::InlineBuffer;
using Memory::StackBuffer;



auto TextToByteReader::New (InputStream<Character>::Ptr srcStream) -> Ptr 
{
    // quick hack but in adequate (assuming we can read to EOF, which is not true) in general so this is bad
    auto s = srcStream.ReadAll ().AsUTF8<string> ();
    return Streams::MemoryStream<std::byte>::New (reinterpret_cast<const std::byte*> (s.data ()),
                                                  reinterpret_cast<const std::byte*> (s.data ()) + s.size ());
}