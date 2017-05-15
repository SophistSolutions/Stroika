/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DenseDataHyperRectangle_h_
#define _Stroika_Foundation_Containers_DenseDataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "DataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            using Configuration::ArgByValueType;
            using Traversal::Iterable;
            using Traversal::Iterator;

            /**
             *  A DenseDataHyperRectangle<> is a DataHyperRectangle where you specify the max value for each dimension, and it is
             *  a programming (assertion) error to check/add items outside that range. And when iterating (e.g. GetLength ()) you always
             *  find all items.
             */
            template <typename T, typename... INDEXES>
            class DenseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
            private:
                using inherited = DataHyperRectangle<T, INDEXES...>;

            public:
                DenseDataHyperRectangle (INDEXES... dimensions);
                DenseDataHyperRectangle (const DenseDataHyperRectangle<T, INDEXES...>& src);

            public:
                nonvirtual DenseDataHyperRectangle<T, INDEXES...>& operator= (const DenseDataHyperRectangle<T, INDEXES...>& rhs) = default;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DenseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_DenseDataHyperRectangle_h_ */
