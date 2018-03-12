/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Configuration/Concepts.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Memory/Optional.h"
#include "../../Memory/SmallStackBuffer.h"

#include "../Common.h"

/**
 *  \file
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *  TODO:
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Redo Contains1 versus Contains using partial template specialization of STLContainerWrapper - easy
 *              cuz such a trivial class. I can use THAT trick to handle the case of forward_list too. And GetLength...
 *
 *      @todo   Add special subclass of ForwardIterator that tracks PREVPTR - and use to cleanup stuff
 *              that uses forward_list code...
 *
 *      @todo   VERY INCOMPLETE Patch support. Unclear if/how I can do patch support generically - perhaps using
 *              some methods only called by array impls, and some only by returing iteratore on erase impls, etc,
 *              or perhaps with template specialization.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DataStructures {

                namespace Private_ {
                    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (erase, (x.erase (x.begin (), x.begin ())));
                }

                /**
                 *  Use this to wrap an underlying stl container (like std::vector or stl:list, etc) to adapt
                 *  it for Stroika containers.
                 *
                 *  This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                 *  DOES provide 'deletion'/update safety.
                 */
                template <typename STL_CONTAINER_OF_T>
                class STLContainerWrapper : public STL_CONTAINER_OF_T, public Debug::AssertExternallySynchronizedLock {
                private:
                    using inherited = STL_CONTAINER_OF_T;

                public:
                    using value_type = typename STL_CONTAINER_OF_T::value_type;

                public:
                    /**
                     *  pass through CTOR args to underlying container
                     */
                    template <typename... EXTRA_ARGS>
                    STLContainerWrapper (EXTRA_ARGS&&... args);

                public:
                    class ForwardIterator;

                public:
                    /*
                     *  Take iteartor 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
                     *  iteartor from 'this' - which points at the same logical position. This requires that this container
                     *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
                     *  the container they were iterating over).
                     */
                    nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper<STL_CONTAINER_OF_T>* movedFrom);

                public:
                    nonvirtual bool Contains (value_type item) const;

                public:
                    /*
                     */
                    template <typename FUNCTION>
                    nonvirtual void Apply (FUNCTION doToElement) const;
                    template <typename FUNCTION>
                    nonvirtual typename STL_CONTAINER_OF_T::iterator FindFirstThat (FUNCTION doToElement);
                    template <typename FUNCTION>
                    nonvirtual typename STL_CONTAINER_OF_T::const_iterator FindFirstThat (FUNCTION doToElement) const;

                public:
                    template <typename PREDICATE>
                    nonvirtual bool FindIf (PREDICATE pred) const;

                public:
                    nonvirtual void Invariant () const;

                public:
                    static typename STL_CONTAINER_OF_T::iterator remove_constness (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it);

                private:
                    template <typename CHECK_ = STL_CONTAINER_OF_T>
                    static typename STL_CONTAINER_OF_T::iterator remove_constness_ (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it, typename std::enable_if<Private_::has_erase<CHECK_>::value>::type* = 0);
                    template <typename CHECK_ = STL_CONTAINER_OF_T>
                    static typename STL_CONTAINER_OF_T::iterator remove_constness_ (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it, typename std::enable_if<!Private_::has_erase<CHECK_>::value>::type* = 0);
                };

                /**
                 *      STLContainerWrapper::ForwardIterator is a private utility class designed
                 *  to promote source code sharing among the patched iterator implementations.
                 *
                 *  \note   ForwardIterator takes a const-pointer the the container as argument since this
                 *          iterator never MODIFIES the container.
                 *
                 *          However, it does a const-cast to maintain a non-const pointer since that is needed to
                 *          option a non-const iterator pointer, which is needed by some classes that use this, and
                 *          there is no zero (or even low for forward_list) cost way to map from const to non const
                 *          iterators (needed to perform the update).
                 *
                 *          @see https://stroika.atlassian.net/browse/STK-538
                 */
                template <typename STL_CONTAINER_OF_T>
                class STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator {
                public:
                    using CONTAINER_TYPE = STLContainerWrapper<STL_CONTAINER_OF_T>;

                public:
                    using value_type = typename STLContainerWrapper<STL_CONTAINER_OF_T>::value_type;

                public:
                    /**
                     *          @see https://stroika.atlassian.net/browse/STK-538
                     */
                    explicit ForwardIterator (const CONTAINER_TYPE* data);
                    explicit ForwardIterator (const ForwardIterator& from) = default;

                public:
                    nonvirtual bool Done () const;

                public:
                    template <typename VALUE_TYPE>
                    nonvirtual bool More (VALUE_TYPE* current, bool advance);
                    template <typename VALUE_TYPE>
                    nonvirtual void More (Memory::Optional<VALUE_TYPE>* current, bool advance);

                public:
                    /**
                     * Only legal to call if underlying iterator is random_access
                     */
                    nonvirtual size_t CurrentIndex () const;

                public:
                    nonvirtual void SetCurrentLink (typename CONTAINER_TYPE::iterator l);

                public:
                    nonvirtual bool Equals (const typename STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator& rhs) const;

                public:
                    /**
                     *          @see https://stroika.atlassian.net/browse/STK-538
                     */
                    CONTAINER_TYPE*                   fData;
                    typename CONTAINER_TYPE::iterator fStdIterator;

                protected:
                    bool fSuppressMore{true};
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "STLContainerWrapper.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_ */
