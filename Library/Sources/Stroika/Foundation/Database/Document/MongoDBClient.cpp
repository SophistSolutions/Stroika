/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Database/Exception.h"

#include "MongoDBClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document::MongoDBClient;
using namespace Debug;
