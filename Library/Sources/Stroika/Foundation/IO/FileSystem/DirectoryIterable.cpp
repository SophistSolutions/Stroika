/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "DirectoryIterator.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

#include "DirectoryIterable.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Traversal;

/*
 ********************************************************************************
 ******************** IO::FileSystem::DirectoryIterable *************************
 ********************************************************************************
 */
DirectoryIterable::DirectoryIterable (const filesystem::path& directoryName, IteratorReturnType iteratorReturns)
    : Iterable<filesystem::path>{MakeIterableFromIterator (DirectoryIterator (directoryName, iteratorReturns))}
{
}
