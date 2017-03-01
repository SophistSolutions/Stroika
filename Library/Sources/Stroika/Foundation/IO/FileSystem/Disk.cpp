/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Characters/String_Constant.h"
#include "../../Memory/SmallStackBuffer.h"

#include "Disk.h"

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
            constexpr EnumNames<Foundation::IO::FileSystem::BlockDeviceKind> DefaultNames<Foundation::IO::FileSystem::BlockDeviceKind>::k;
        }
    }
}