/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/ModuleInit.h"

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {

            /*
             ********************************************************************************
             ************************** InternetMediaTypeRegistry ***************************
             ********************************************************************************
             */
            inline const InternetMediaTypeRegistry InternetMediaTypeRegistry::Default ()
            {
                return InternetMediaTypeRegistry{};
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_inl_*/
