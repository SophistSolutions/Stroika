/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_

#include "../../StroikaPreComp.h"

#include "../DenseDataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   DenseDataHyperRectangle_Vector<T, INDEXES...> is an Dense Vector-based concrete implementation of the DenseDataHyperRectangle<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename... INDEXES>
                class DenseDataHyperRectangle_Vector : public DenseDataHyperRectangle<T, INDEXES...> {
                private:
                    using inherited = DenseDataHyperRectangle<T, INDEXES...>;

                public:
                    DenseDataHyperRectangle_Vector (INDEXES... dimensions);
                    DenseDataHyperRectangle_Vector (const DenseDataHyperRectangle_Vector<T, INDEXES...>& src);

                public:
                    nonvirtual DenseDataHyperRectangle_Vector<T, INDEXES...>& operator= (const DenseDataHyperRectangle_Vector<T, INDEXES...>& rhs);

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

#include "DenseDataHyperRectangle_Vector.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_ */
