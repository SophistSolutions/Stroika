/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Users_h_
#define _Stroika_Foundation_Execution_Users_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"

#include    "Exceptions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            enum class  UserNameFormat : uint8_t {
                ePrettyName,
                eDEFAULT = ePrettyName,

                Stroika_Define_Enum_Bounds(ePrettyName, ePrettyName)
            };
            Characters::String  GetCurrentUserName (UserNameFormat format = UserNameFormat::eDEFAULT);


        }
    }
}



#endif  /*_Stroika_Foundation_Execution_Users_h_*/
