/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_

#include    "Stack_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T, typename TRAITS>
                inline  Stack<T, TRAITS>  mkStack_Default ()
                {
                    return Stack_LinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_ */
