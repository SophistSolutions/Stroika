/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_

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
            class SparseDataHyperRectangle;

            namespace Concrete {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SparseDataHyperRectangle<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SparseDataHyperRectangle_Factory<T>::Register (), or
                 *  replace it statically by template-specailizing SparseDataHyperRectangle_Factory<T>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename... INDEXES>
                class SparseDataHyperRectangle_Factory {
                private:
                    static atomic<SparseDataHyperRectangle<T, INDEXES...> (*) (Configuration::ArgByValueType<T> defaultItem)> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the SparseDataHyperRectangle<T, INDEXES...> CTOR does so automatically.
                     */
                    static SparseDataHyperRectangle<T, INDEXES...> mk (Configuration::ArgByValueType<T> defaultItem = {});

                public:
                    /**
                     *  Register a replacement creator/factory for the given SparseDataHyperRectangle<T, INDEXES...>. Note this is a global change.
                     */
                    static void Register (SparseDataHyperRectangle<T, INDEXES...> (*factory) (Configuration::ArgByValueType<T> defaultItem) = nullptr);

                private:
                    static SparseDataHyperRectangle<T, INDEXES...> Default_ (Configuration::ArgByValueType<T> defaultItem);
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
#include "SparseDataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_ */
