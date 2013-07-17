/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_

// MAYBE want to do this in general, since it doesnt require operator< etc to be defined: just operator==
// but its not a great general purpose solution (very unperformant).
// But REAL reason todo this - is cuz due to crazy deadly #include embrace, its hard to get the
// other ones working well
#define COMPILE_FACTORY_MAPPING_USING_LINKEDLIST_ 1

#if COMPILE_FACTORY_MAPPING_USING_LINKEDLIST_
#include    "Mapping_LinkedList.h"
#else
#include    "Mapping_stdmap.h"
#endif

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>  mkMapping_Default ()
                {
                    //@todo - convert to traits!!!
#if     COMPILE_FACTORY_MAPPING_USING_LINKEDLIST_
                    return Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> ();
#else
                    return Mapping_stdmap<KEY_TYPE, VALUE_TYPE> ();
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
