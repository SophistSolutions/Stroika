/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Users_h_
#define _Stroika_Foundation_Execution_Users_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::Foundation::Execution {

    enum class UserNameFormat : uint8_t {
        ePrettyName,

        eDEFAULT = ePrettyName,

        Stroika_Define_Enum_Bounds (ePrettyName, ePrettyName)
    };
    Characters::String GetCurrentUserName (UserNameFormat format = UserNameFormat::eDEFAULT);

}

#endif /*_Stroika_Foundation_Execution_Users_h_*/
