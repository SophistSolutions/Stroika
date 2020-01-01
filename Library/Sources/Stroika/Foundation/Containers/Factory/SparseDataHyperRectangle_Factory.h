/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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

namespace Stroika::Foundation::Containers {

    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SparseDataHyperRectangle<> container
     *
     *  Note - you can override the underlying factory dynamically by calling SparseDataHyperRectangle_Factory<T>::Register (), or
     *  replace it statically by template-specializing SparseDataHyperRectangle_Factory<T>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<SparseDataHyperRectangle<T, INDEXES...> (*) (Configuration::ArgByValueType<T> defaultItem)> sFactory_;
#else
        static inline atomic<SparseDataHyperRectangle<T, INDEXES...> (*) (Configuration::ArgByValueType<T> defaultItem)> sFactory_{nullptr};
#endif

    public:
        /**
         *  You can call this directly, but there is no need, as the SparseDataHyperRectangle<T, INDEXES...> CTOR does so automatically.
         */
        nonvirtual SparseDataHyperRectangle<T, INDEXES...> operator() (Configuration::ArgByValueType<T> defaultItem = {});

    public:
        /**
         *  Register a replacement creator/factory for the given SparseDataHyperRectangle<T, INDEXES...>. Note this is a global change.
         */
        static void Register (SparseDataHyperRectangle<T, INDEXES...> (*factory) (Configuration::ArgByValueType<T> defaultItem) = nullptr);

    private:
        static SparseDataHyperRectangle<T, INDEXES...> Default_ (Configuration::ArgByValueType<T> defaultItem);
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SparseDataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_ */
