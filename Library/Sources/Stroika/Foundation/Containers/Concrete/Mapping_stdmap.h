/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"

#include    "SortedMapping_stdmap.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 *      @todo   Defining Mapping_stdmap this way makes sense, but runs into deadly
 *              embrace of #include problems. Maybe easier to just implement it
 *              a second time (duplicate the SortedMapping_stdmap<> code).
 *
 *              YES - REDO AS CLONE!
 *
 *
 *      @todo   Fix  this code so
 *        namespace Test5_ToFromSTLMap {
 *            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
 *            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
                can be re-enabled...
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
#if     qCompilerAndStdLib_Supports_TempalteAlias_n2258
                template    <typename Key, typename T>
                using   Mapping_stdmap = SortedMapping_stdmap<Key, T>;
#else
                template    <typename Key, typename T>
                class   Mapping_stdmap : public Mapping<Key, T> {
                private:
                    typedef     Mapping<Key, T>  inherited;

                public:
                    Mapping_stdmap ()
                    // static cast to select right base class CTOR (that wont copy)
                        : inherited (static_cast<inherited&> (SortedMapping_stdmap<Key, T> ())) {
                    }
#if 0
                    // support/enabel these as needed - all SB there
                    Mapping_stdmap (const Mapping_stdmap<Key, T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Mapping_stdmap<Key, T>& operator= (const Mapping_stdmap<Key, T>& m);
#endif


                };
#endif


            }
        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
