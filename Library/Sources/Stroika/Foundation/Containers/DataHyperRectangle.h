/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataHyperRectangle_h_
#define _Stroika_Foundation_Containers_DataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo https://stroika.atlassian.net/browse/STK-580 - get DataHyperRectangle to the point where it is minimally usable!
 *
 *      @todo DataHyperRectangle<int, int, int> x = Concrete::DenseDataHyperRectangle_Vector<int, int, int>{3, 4}; simplified - not saying int so many times.
 *            make use make_DataHyperRectangle () - cuz I think functions can do type inference where classes cannot.
 *
 *      @todo MAYBE operator== ()/!= should be on _Sparse/_Dense - not here!
 *
 *      @todo need ability to return reference somehow... a[3][4] = 3; or a[3][4].x = 3;
 *
 *      @todo need ability create arbitrary subslices - like numpy ... a[3..4] or a[3..4,_,5]... will need some alternate syntax
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  \note   Aliases: Data-Cube, Date Cube, Hyper-Cube, Hypercube
     */
    template <typename T, typename... INDEXES>
    class DataHyperRectangle : public Iterable<tuple<T, INDEXES...>> {
    private:
        using inherited = Iterable<tuple<T, INDEXES...>>;

    protected:
        class _IRep;

    protected:
        using _DataHyperRectangleRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = DataHyperRectangle<T, INDEXES...>;

    public:
        /**
         *  \note - its typically best to select a specific subtype of DataHyperRectangle to construct so you can 
         *          specify the appropriate additional parameters. Typically use subclasses 
         *          @see SparseDataHyperRectangle or @see DenseDataHyperRectangle.
         *
         *  \par Example:
         *      DataHyperRectangle2<int> x1 = DenseDataHyperRectangle_Vector<int, size_t, size_t>{3, 4};
         *      DataHyperRectangle2<int> x1 = Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>{3, 4};
         *      DataHyperRectangle2<int> x2 = Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>{};
         */
        DataHyperRectangle (const DataHyperRectangle<T, INDEXES...>& src) noexcept;
        DataHyperRectangle (DataHyperRectangle<T, INDEXES...>&& src) noexcept;

    protected:
        explicit DataHyperRectangle (const _DataHyperRectangleRepSharedPtr& src) noexcept;
        explicit DataHyperRectangle (_DataHyperRectangleRepSharedPtr&& src) noexcept;

    public:
        /**
         */
        nonvirtual DataHyperRectangle<T, INDEXES...>& operator= (const DataHyperRectangle<T, INDEXES...>& rhs) = default;
        nonvirtual DataHyperRectangle<T, INDEXES...>& operator= (DataHyperRectangle<T, INDEXES...>&& rhs) = default;

    public:
        /**
         */
        nonvirtual T GetAt (INDEXES... indexes) const;

    public:
        /**
         */
        nonvirtual void SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v);

    private:
        template <typename INDEX, typename... REST_OF_INDEXES>
        struct TemporarySliceReference_ {
            const DataHyperRectangle<T, INDEXES...>& fCube;
            tuple<REST_OF_INDEXES...>                fSliceIdxes;
            T                                        operator[] (INDEX i) const
            {
                return fCube.GetAt (i, forward<REST_OF_INDEXES> (fSliceIdxes)...);
            }
        };

    public:
        /**
         *  EXAMPLE USAGE:
         *      DataHyperRectangle<double, int, int> m (2,2);
         *      Assert (m[1][1] == 0);
         */
        template <typename INDEX, typename... REST_OF_INDEXES>
        nonvirtual TemporarySliceReference_<REST_OF_INDEXES...> operator[] (INDEX i1) const;

    public:
        /**
         *  Two DataHyperRectangle are considered equal if they contain the same elements (by comparing them with EQUALS_COMPARER)
         *  in exactly the same order.
         *
         *  Equals is commutative().
         *
         *  A DataHyperRectangle<T, INDEXES...> doesn't generally require a comparison for individual elements
         *  be be defined, but obviously to compare if the containers are equal, you must
         *  compare the individual elements (at least sometimes).
         *
         *  If == is predefined, you can just call Equals() - but if its not, or if you wish
         *  to compare with an alternative comparer, just pass it as a template parameter.
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Equals (const DataHyperRectangle& rhs, const EQUALS_COMPARER& equalsComparer = {}) const;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

    protected:
        nonvirtual void _AssertRepValidType () const;
    };

    using Traversal::IteratorOwnerID;

    /**
     *  \brief  Implementation detail for DataHyperRectangle<T, INDEXES...> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the DataHyperRectangle<T, INDEXES...> container API.
     */
    template <typename T, typename... INDEXES>
    class DataHyperRectangle<T, INDEXES...>::_IRep : public Iterable<tuple<T, INDEXES...>>::_IRep {
    private:
        using inherited = _IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _DataHyperRectangleRepSharedPtr = typename DataHyperRectangle<T, INDEXES...>::_DataHyperRectangleRepSharedPtr;

    public:
        virtual _DataHyperRectangleRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const         = 0;
        virtual T                               GetAt (INDEXES... indexes) const                               = 0;
        virtual void                            SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v) = 0;
    };

#if 0
    template <typename T, size_t N>
    using DataHyperRectangleN = void;
    // @todo - use generator to do this automatically/genrally
    template <typename T>
    using DataHyperRectangleN<T,1> = DataHyperRectangle<T, size_t>;
    template <typename T>
    using DataHyperRectangleN<T, 2> = DataHyperRectangle<T, size_t, size_t>;
    template <typename T, 3>
    using DataHyperRectangleN = DataHyperRectangle<T, size_t, size_t, size_t>;
    template <typename T, 4>
    using DataHyperRectangleN = DataHyperRectangle<T, size_t, size_t, size_t, size_t>;
#endif
    /**
     *  @todo see if there is a way to define this genericly using templates/sequences - DataHyperRectangleN<N>
     */
    template <typename T>
    using DataHyperRectangle1 = DataHyperRectangle<T, size_t>;
    template <typename T>
    using DataHyperRectangle2 = DataHyperRectangle<T, size_t, size_t>;
    template <typename T>
    using DataHyperRectangle3 = DataHyperRectangle<T, size_t, size_t, size_t>;
    template <typename T>
    using DataHyperRectangle4 = DataHyperRectangle<T, size_t, size_t, size_t, size_t>;

    /**
     *      Syntactic sugar for Equals()
     *
     *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
     *          use a different comparer, call Equals() directly.
     */
    template <typename T, typename... INDEXES>
    bool operator== (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs);

    /**
     *      Syntactic sugar for not Equals()
     *
     *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
     *          use a different comparer, call Equals() directly.
     */
    template <typename T, typename... INDEXES>
    bool operator!= (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_DataHyperRectangle_h_ */
