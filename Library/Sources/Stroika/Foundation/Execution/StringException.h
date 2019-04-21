/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_h_
#define _Stroia_Foundation_Execution_StringException_h_ 1

#include "../StroikaPreComp.h"

#include <exception>

#include "Exceptions.h"

//_DeprecatedFile_("DEPRECATED in v2.1d18 - #include Exception.h instead of StringException.h");

namespace Stroika::Foundation::Execution {

    using StringException [[deprecated ("DEPRECATED in v2.1d18 - use Exception<>")]] = Exception<exception>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroia_Foundation_Execution_StringException_h_*/
