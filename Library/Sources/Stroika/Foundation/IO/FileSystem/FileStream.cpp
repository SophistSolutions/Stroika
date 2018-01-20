/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "FileStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            constexpr EnumNames<IO::FileSystem::FileStream::SeekableFlag>  DefaultNames<IO::FileSystem::FileStream::SeekableFlag>::k;
            constexpr EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy> DefaultNames<IO::FileSystem::FileStream::AdoptFDPolicy>::k;
        }
    }
}

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
