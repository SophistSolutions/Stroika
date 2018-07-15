/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_h_
#define _Stroika_Foundation_Containers_Collection_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Configuration/Concepts.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Where(hide iterable one) and probably other things should use new EmptyClone() strategy - so cheaper and
 *              returns something of same underlying data structure  type.
 *
 *      @todo   Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *      @todo   Have Difference/Union/Interesection??? methods/?? Do research....
 *
 *      @todo   Consider adding RetainAll (Set<T>) API - like in Collection.h, and Java. Key diff is was force
 *              use of SET as arg - not another Bag? Or maybe overload with different container types as args?
 *              COULD do 2 versions - one with Iterable<T> and one with Set<T>. trick is to get definition
 *              to work without untoward dependencies between set and bag code? I think that means
 *              most of the check impl needs to be in the envelope to avoid always building it through vtables.
 *
 *      @todo   Perhaps add a Shake() method that produces a NEW Collection of a possibly different backend TYPE!
 *              with a random ordering.
 *
 *              Or add Bag<> class that does basically this.
 *
 *      @todo   Document relationship with Java Collection<T> (http://docs.oracle.com/javase/7/docs/api/java/util/Collection.html)
 *              Similar but Stroika container interface/containter connection, Equals handling, and other language specific
 *              issues. Java has more 'all' variants (and retainAll - see possible todo above). And java makes this
 *              strictly an interface - used by lots of other subtypes (like set<>) - which we may wish to do as
 *              well (not sure - sterl feels this is a very bad idea, and I'm ambivilent).
 */

namespace Stroika::Foundation {
    namespace Containers {

        using Configuration::ArgByValueType;
        using Traversal::Iterable;
        using Traversal::Iterator;

        /**
         *  \brief  A Collection<T> is a container to manage an un-ordered collection of items, without equality defined for T
         *
         *  A Collection<T> is a container pattern to manage an un-ordered collection of items,
         *  without equality defined for T. This is both an abstract interface, but the Collection<T>
         *  class it actually concrete because it automatically binds to a default implementation.
         *
         *  \note   This means that the order in which items appear during iteration is <em>arbitrary</em> and
         *          may vary from collection to collection type.
         *
         *          Some sub-types of Collection<T> - like SortedCollection<> - do make specific promises
         *          about ordering.
         *
         *  A Collection<T> is the simplest kind of collection. It allows addition and
         *  removal of elements, but makes no guarantees about element ordering.
         *
         *  \em Performance
         *      Collections are typically designed to optimize item addition and iteration.
         *      They are fairly slow at item access (as they have no keys). Removing items
         *      is usually slow, except in the context of an Iterator<T>, where it is usually
         *      very fast. Collection comparison (operator==) is often very slow in the worst
         *      case (n^2) and this worst case is the relatively common case of identical
         *      bags.
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *
         *  \note   Shake
         *      Considered adding a Shake() method (when this was called Bag<T>), but that would restrict
         *      the use to backends capable of this randomizing of order (eg. not hashtables
         *      or trees), and is incompatible with the idea of subtypes like  SortedCollection<T>. Also
         *      since a Collection<> intrinsically has no ordering, I'm not totally sure what it would
         *      mean to Shake/change its ordering?
         *
         *  \note   Equals (operator==, operator!=)
         *          We do not provide a notion of Equals() or operator==, operator!=, because
         *          its not clear how to compare collections.
         *
         *          The caller may use the inherited (from Iterable<>) SetEquals, MultiSetEquals, or SequnceEquals()
         *          as appropriate.
         *
         *  \note Note About Iterators
         *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
         *
         *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
         *          the iterators are automatically updated internally to behave sensibly.
         *
         */
        template <typename T>
        class Collection : public Iterable<T> {
        private:
            using inherited = Iterable<T>;

        public:
            /**
             *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
             */
            using ArchetypeContainerType = Collection<T>;

        protected:
            class _IRep;

        protected:
            using _CollectionRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

        public:
            /**
             *  For the CTOR overload with CONTAINER_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
             *  all the elements, and which has elements (iterated) convertable to T.
             *
             *  \par Example Usage
             *      \code
             *        Sequence<int> s;
             *        std::vector<int> v;
             *
             *        Collection<int> c1  = {1, 2, 3};
             *        Collection<int> c2  = c1;
             *        Collection<int> c3  { c1 };
             *        Collection<int> c4  { c1.begin (), c1.end () };
             *        Collection<int> c5  { s };
             *        Collection<int> c6  { v };
             *        Collection<int> c7  { v.begin (), v.end () };
             *        Collection<int> c8  { move (c1) };
             *      \endcode
             */
            Collection ();
            Collection (const Collection& src) noexcept = default;
            Collection (Collection&& src) noexcept      = default;
            Collection (const initializer_list<T>& src);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
            Collection (const CONTAINER_OF_ADDABLE& src);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            Collection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        protected:
            explicit Collection (const _CollectionRepSharedPtr& src) noexcept;
            explicit Collection (_CollectionRepSharedPtr&& src) noexcept;

#if qDebug
        public:
            ~Collection ();
#endif
        public:
            nonvirtual Collection& operator= (const Collection& rhs) = default;
            nonvirtual Collection& operator= (Collection&& rhs) = default;

        public:
            /**
             * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
             */
            template <typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual bool Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {}) const;

        public:
            /**
             * Add the given item(s) to this Collection<T>. Note - if the given items are already present, another
             * copy will be added. No promises are made about where the added value will appear in iteration.
             */
            nonvirtual void Add (ArgByValueType<T> item);

        public:
            /**
             *  \note   AddAll/2 is alias for .net AddRange ()
             */
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            nonvirtual void AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value>* = nullptr>
            nonvirtual void AddAll (const CONTAINER_OF_ADDABLE& s);

        public:
            /**
             * This function requires that the iterator 'i' came from this container.
             *
             * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
             *
             *      @todo DOCUMENT SEMANTICS MORE CLEARLY - THINKING THROUGH SUBCLASS SORTEDCollection - DEFINE SO STILL MAKES SENSE THERE!!!
             *      LIKE EQUIV TO REMOVE(i) and ADD newValue - but more efficient? No - maybe just does remove(i) and add (newValue?))
             *      and document promised semantics about if you will encounter newvalue again when you continue iterating!
             *
             *      MAYBE best answer is to LOSE this Update() method for bag<> - useful for Sequence<> - but maybe not here!
             */
            nonvirtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue);

        public:
            /**
             * It is legal to remove something that is not there. This function removes the first instance of item
             * (or each item for the 'items' overload), meaning that another instance of item could still be in the
             * Collection<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
             *
             * SECOND OVERLOAD:
             * This function requires that the iterator 'i' came from this container.
             *
             * The value pointed to by 'i' is removed.
             */
            template <typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});
            nonvirtual void Remove (const Iterator<T>& i);

        public:
            /**
             * It is legal to remove something that is not there. This function removes the first instance of item
             * (or each item for the 'items' overload), meaning that another instance of item could still be in the
             * Collection<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
             *
             *  The no-argument verison Produces an empty bag.
             */
            nonvirtual void RemoveAll ();
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual void RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, const EQUALS_COMPARER& equalsComparer = {});
            template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& c, const EQUALS_COMPARER& equalsComparer = {});

        public:
            /**
             *  Apply the function funciton to each element, and return all the ones for which it was true.
             *
             *  @see Iterable<T>::Where
             */
            nonvirtual Collection<T> Where (const function<bool(ArgByValueType<T>)>& doToElement) const;

        public:
            /**
             * \brief STL-ish alias for RemoveAll ().
             */
            nonvirtual void clear ();

        public:
            /**
             * \brief STL-ish alias for Remove ().
             */
            template <typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual void erase (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});
            nonvirtual void erase (const Iterator<T>& i);

        public:
            /**
             *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
             *
             *  *DEVELOPER NOTE*
             *      Note - we use an overload
             *      of Collection<T> for the container case instead of a template, because I'm not sure how to use specializations
             *      to distinguish the two cases. If I can figure that out, this can transparently be
             *      replaced with operator+= (X), with appropriate specializations.
             */
            nonvirtual Collection<T>& operator+= (ArgByValueType<T> item);
            nonvirtual Collection<T>& operator+= (const Iterable<T>& items);

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
         *  \brief  Implementation detail for Collection<T> implementors.
         *
         *  Protected abstract interface to support concrete implementations of
         *  the Collection<T> container API.
         */
        template <typename T>
        class Collection<T>::_IRep : public Iterable<T>::_IRep {
        private:
            using inherited = typename Iterable<T>::_IRep;

        protected:
            _IRep () = default;

        public:
            virtual ~_IRep () = default;

        protected:
            using _CollectionRepSharedPtr = typename Collection<T>::_CollectionRepSharedPtr;

        public:
            virtual _CollectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const    = 0;
            virtual void                    Add (ArgByValueType<T> item)                              = 0;
            virtual void                    Update (const Iterator<T>& i, ArgByValueType<T> newValue) = 0;
            virtual void                    Remove (const Iterator<T>& i)                             = 0;
#if qDebug
            virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif
        };

        /**
         *  Basic operator overload with the obivous meaning (Collection<T> copy and Collection<T>::AddAll)
         */
        template <typename T>
        Collection<T> operator+ (const Iterable<T>& lhs, const Collection<T>& rhs);
        template <typename T>
        Collection<T> operator+ (const Collection<T>& lhs, const Iterable<T>& rhs);
        template <typename T>
        Collection<T> operator+ (const Collection<T>& lhs, const Collection<T>& rhs);
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Collection.inl"

#endif /*_Stroika_Foundation_Containers_Collection_h_ */
