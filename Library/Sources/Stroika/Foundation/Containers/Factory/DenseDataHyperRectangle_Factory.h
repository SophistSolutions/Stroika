/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T, typename... INDEXES>
            class DenseDataHyperRectangle;

            namespace Concrete {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a DenseDataHyperRectangle<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling DenseDataHyperRectangle_Factory<T>::Register (), or
                 *  replace it statically by template-specailizing DenseDataHyperRectangle_Factory<T>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename... INDEXES>
                class DenseDataHyperRectangle_Factory {
                private:
                    static atomic<DenseDataHyperRectangle<T, INDEXES...> (*) (INDEXES...)> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the DenseDataHyperRectangle<T, INDEXES...> CTOR does so automatically.
                     */
                    static DenseDataHyperRectangle<T, INDEXES...> mk (INDEXES... dimensions);

                public:
                    /**
                     *  Register a replacement creator/factory for the given DenseDataHyperRectangle<T, INDEXES...>. Note this is a global change.
                     */
                    static void Register (DenseDataHyperRectangle<T, INDEXES...> (*factory) (INDEXES...) = nullptr);

                private:
                    static DenseDataHyperRectangle<T, INDEXES...> Default_ (INDEXES... dimensions);
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
#include "DenseDataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_ */
