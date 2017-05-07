/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_h_

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
            class DataHyperRectangle;

            namespace Concrete {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a DataHyperRectangle<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling DataHyperRectangle_Factory<T>::Register (), or
                 *  replace it statically by template-specailizing DataHyperRectangle_Factory<T>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename... INDEXES>
                class DataHyperRectangle_Factory {
                private:
                    static atomic<DataHyperRectangle<T, INDEXES...> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the DataHyperRectangle<T, INDEXES...> CTOR does so automatically.
                     */
                    static DataHyperRectangle<T, INDEXES...> mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given DataHyperRectangle<T, INDEXES...>. Note this is a global change.
                     */
                    static void Register (DataHyperRectangle<T, INDEXES...> (*factory) () = nullptr);

                private:
                    static DataHyperRectangle<T, INDEXES...> Default_ ();
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
#include "DataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_h_ */
