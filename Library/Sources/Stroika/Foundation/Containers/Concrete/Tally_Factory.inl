/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_

#include    "Tally_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T, typename TRAITS>
                inline  Tally<T, TRAITS>  mkTally_Default ()
                {
                    /// TODO @todo - fix TRAITS usage
                    return Tally_Array<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_ */
