/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"
#include "../Memory/BLOB.h"

#include "MemoryStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::MemoryStream;

using Characters::String;

/*
 ********************************************************************************
 ***************** MemoryStream::Private_::MemStream2StringHelper_ **************
 ********************************************************************************
 */
Characters::String MemoryStream::Private_::MemStream2StringHelper_ (const span<const byte>& s)
{
    return Characters::ToString (Memory::BLOB{s}); // wrap in BLOB for its ToString function
}