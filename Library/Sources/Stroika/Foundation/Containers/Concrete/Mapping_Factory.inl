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


                template    <typename Key, typename T>
                inline  Mapping<Key, T>  mkMapping_Default ()
                {
#if COMPILE_FACTORY_MAPPING_USING_LINKEDLIST_
                    return Mapping_LinkedList<Key, T> ();
#else
                    return Mapping_stdmap<Key, T> ();
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
