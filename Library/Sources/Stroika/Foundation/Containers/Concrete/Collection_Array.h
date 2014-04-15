/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Collection.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 * \brief   Collection_Array<T> is an Array-based concrete implementation of the Collection<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   Collection_Array : public Collection<T> {
                private:
                    using   inherited   =     Collection<T>;

                public:
                    Collection_Array ();
                    Collection_Array (const Collection<T>& src);
                    Collection_Array (const Collection_Array<T>& src);
                    Collection_Array (const T* start, const T* end);

                public:
                    nonvirtual  Collection_Array<T>& operator= (const Collection_Array<T>& rhs);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Collection<T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete collection, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();

                public:
                    /*
                     * This optional API allows pre-reserving space as an optimizaiton.
                     */
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Collection_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Collection_Array_h_ */
