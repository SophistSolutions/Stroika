/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_h_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_h_

#include "../../StroikaPreComp.h"

#include "../Stack.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   VERY INCOMPLETE/WRONG IMPLEMENTATION - ESPECIALLY COPYING WRONG!!!
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   DataHyperRectangle_DenseVector<T> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class DataHyperRectangle_DenseVector : public Stack<T> {
                private:
                    using inherited = Stack<T>;

                public:
                    DataHyperRectangle_DenseVector ();
                    DataHyperRectangle_DenseVector (const DataHyperRectangle_DenseVector<T>& src);

                public:
                    nonvirtual DataHyperRectangle_DenseVector<T>& operator= (const DataHyperRectangle_DenseVector<T>& rhs);

                private:
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
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

#include "DataHyperRectangle_DenseVector.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_h_ */
