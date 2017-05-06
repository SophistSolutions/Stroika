/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_h_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_h_

#include "../../StroikaPreComp.h"

#include "../DataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   DataHyperRectangle_DenseVector<T, INDEXES...> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename... INDEXES>
                class DataHyperRectangle_DenseVector : public DataHyperRectangle<T, INDEXES...> {
                private:
                    using inherited = DataHyperRectangle<T, INDEXES...>;

                public:
                    DataHyperRectangle_DenseVector (INDEXES... dimensions);
                    DataHyperRectangle_DenseVector (const DataHyperRectangle_DenseVector<T, INDEXES...>& src);

                public:
                    nonvirtual DataHyperRectangle_DenseVector<T, INDEXES...>& operator= (const DataHyperRectangle_DenseVector<T, INDEXES...>& rhs);

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
