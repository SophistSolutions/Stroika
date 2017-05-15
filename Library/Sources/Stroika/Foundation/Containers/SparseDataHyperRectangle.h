/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_h_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "DataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            using Configuration::ArgByValueType;
            using Traversal::Iterable;
            using Traversal::Iterator;

            /**
             *  A SparseDataHyperRectangle<> is a DataHyperRectangle where you specify a special 'default' value, which will appear
             *  in any cell you 'get' without first setting (as if the hyper-rectangle was pre-initialized to that value).
             *
              * But default values don't show up when iterating.
             */
            template <typename T, typename... INDEXES>
            class SparseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
            private:
                using inherited = DataHyperRectangle<T, INDEXES...>;

            public:
                SparseDataHyperRectangle (Configuration::ArgByValueType<T> defaultItem = {});
                SparseDataHyperRectangle (const SparseDataHyperRectangle<T, INDEXES...>& src);

            public:
                nonvirtual SparseDataHyperRectangle<T, INDEXES...>& operator= (const SparseDataHyperRectangle<T, INDEXES...>& rhs) = default;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SparseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_SparseDataHyperRectangle_h_ */
