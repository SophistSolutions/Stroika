/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/Tokenize.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



