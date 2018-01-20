/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SparseDataHyperRectangle.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   SparseDataHyperRectangle_stdmap<T, INDEXES...> is an Dense Vector-based concrete implementation of the SparseDataHyperRectangle<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename... INDEXES>
                class SparseDataHyperRectangle_stdmap : public SparseDataHyperRectangle<T, INDEXES...> {
                private:
                    using inherited = SparseDataHyperRectangle<T, INDEXES...>;

                public:
                    /**
                     */
                    SparseDataHyperRectangle_stdmap (Configuration::ArgByValueType<T> defaultItem = {});
                    SparseDataHyperRectangle_stdmap (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& src);

                public:
                    nonvirtual SparseDataHyperRectangle_stdmap<T, INDEXES...>& operator= (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& rhs);

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

#include "SparseDataHyperRectangle_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_h_ */
