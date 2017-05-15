/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Sparse_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Sparse_stdmap_h_

#include "../../StroikaPreComp.h"

#include "../SparseDataHyperRectangle.h"

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
                 *  \brief   DataHyperRectangle_Sparse_stdmap<T, INDEXES...> is an Dense Vector-based concrete implementation of the SparseDataHyperRectangle<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename... INDEXES>
                class DataHyperRectangle_Sparse_stdmap : public SparseDataHyperRectangle<T, INDEXES...> {
                private:
                    using inherited = SparseDataHyperRectangle<T, INDEXES...>;

                public:
                    /**
                     */
                    DataHyperRectangle_Sparse_stdmap (Configuration::ArgByValueType<T> defaultItem = {});
                    DataHyperRectangle_Sparse_stdmap (const DataHyperRectangle_Sparse_stdmap<T, INDEXES...>& src);

                public:
                    nonvirtual DataHyperRectangle_Sparse_stdmap<T, INDEXES...>& operator= (const DataHyperRectangle_Sparse_stdmap<T, INDEXES...>& rhs);

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

#include "DataHyperRectangle_Sparse_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Sparse_stdmap_h_ */
