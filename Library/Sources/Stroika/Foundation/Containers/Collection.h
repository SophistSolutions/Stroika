/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_h_
#define _Stroika_Foundation_Containers_Collection_h_   1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"

#include    "UpdatableIterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   EachWith() and probably other things should use new EmptyClone() strategy - so cheaper and
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             *  \brief  A Collection<T> is a container to manage an un-ordered collection of items, without equality defined for T
             *
             *  A Collection<T> is a container pattern to manage an un-ordered collection of items,
             *  without equality defined for T. This is both an abstract interface, but the Collection<T>
             *  class it actually concrete because it automatically binds to a default implementation.
             *
             *  A Collection<T> is the simplest kind of collection. It allows addition and
             *  removal of elements, but makes no guarantees about element ordering. Two
             *  collections are considered equal if they contain the same items, even if iteration
             *  order is different.
             *
             *  \em Performance
             *      Collections are typically designed to optimize item addition and iteration.
             *      They are fairly slow at item access (as they have no keys). Removing items
             *      is usually slow, except in the context of an Iterator<T>, where it is usually
             *      very fast. Collection comparison (operator==) is often very slow in the worst
             *      case (n^2) and this worst case is the relatively common case of identical
             *      bags.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
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
             */
            template    <typename T>
            class   Collection : public UpdatableIterable<T> {
            private:
                using   inherited   =   UpdatableIterable<T>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   Collection<T>;

            protected:
                class   _IRep;

            protected:
                using   _CollectionSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _CollectionSharedPtrIRep;

            public:
                Collection ();
                Collection (const Collection<T>& src);
                Collection (Collection<T>&& src);
                Collection (const initializer_list<T>& src);
                template < typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Collection<T>*>::value >::type >
                explicit Collection (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Collection (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Collection (const _SharedPtrIRep& src);
                explicit Collection (_SharedPtrIRep&& src);

#if     qDebug
            public:
                ~Collection ();
#endif
            public:
                nonvirtual  Collection<T>& operator= (const Collection<T>& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  Collection<T>& operator= (Collection<T> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  Collection<T>& operator= (Collection<T> && rhs) = default;
#endif

            public:
                /**
                 * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 * Add the given item(s) to this Collection<T>. Note - if the given items are already present, another
                 * copy will be added. No promises are made about where the added value will appear in iteration.
                 */
                nonvirtual  void    Add (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value>::type >
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

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
                nonvirtual  void    Update (const Iterator<T>& i, T newValue);

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
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Collection<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
                 *
                 *  The no-argument verison Produces an empty bag.
                 */
                nonvirtual  void    RemoveAll ();
                template    <typename COPY_FROM_ITERATOR_OF_T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& c);

            public:
                /**
                 *  \em EXPERIMENTAL API (2014-01-08)
                 *
                 *  Apply the function funciton to each element, and return all the ones for which it was true.
                 *
                 *  @todo This is the same as Iterbale<T>::Where() - so unclear its useful - except that you know the
                 *      return type.... Consider if thats enough. PROBABLY Iterable::Where should take a destination target container
                 *      as oiptional arg.
                 *
                 */
                nonvirtual  Collection<T>    EachWith (const function<bool(const T& item)>& doToElement) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 * \brief STL-ish alias for Remove ().
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    erase (T item);
                nonvirtual  void    erase (const Iterator<T>& i);

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
                nonvirtual  Collection<T>& operator+= (T item);
                nonvirtual  Collection<T>& operator+= (const Iterable<T>& items);

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<T>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<T>* s)
                        : fAccessor (s->_GetReadOnlyIterableIRepReference ())
                    {
                    }
                    nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const
                    {
                        EnsureMember (fAccessor.cget (), REP_SUB_TYPE);
                        return static_cast<const REP_SUB_TYPE&> (*fAccessor.cget ());   // static cast for performance sake - dynamic cast in Ensure
                    }
                };
#else
                /**
                 */
                template    <typename T2>
                using   _SafeReadRepAccessor = typename Iterable<T>::template _SafeReadRepAccessor<T2>;
#endif

            protected:
                /**
                 */
                template    <typename T2>
                using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Collection<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Collection<T> container API.
             */
            template    <typename T>
            class   Collection<T>::_IRep : public UpdatableIterable<T>::_IRep {
            private:
                using   inherited = typename UpdatableIterable<T>::_IRep;

            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            protected:
                using   _SharedPtrIRep = typename Collection<T>::_SharedPtrIRep;

            public:
                using   PassTArgByValueType = typename inherited::PassTArgByValueType;

            public:
                virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const                  =   0;
                virtual void                Add (PassTArgByValueType item)                                          =   0;
                virtual void                Update (const Iterator<T>& i, PassTArgByValueType newValue)             =   0;
                virtual void                Remove (const Iterator<T>& i)                                           =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Collection.inl"

#endif  /*_Stroika_Foundation_Containers_Collection_h_ */
