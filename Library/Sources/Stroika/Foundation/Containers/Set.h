/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

#include    "../StroikaPreComp.h"

#include    <set>

#include    "../Configuration/Common.h"
#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/Optional.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"

#include    "UpdatableIterable.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   EachWith() and probably other things should use new EmptyClone() strategy - so cheaper and
 *              returns something of same underlying data structure  type.
 *
 *      @todo   Implement more backends
 *              >   Set_BitString
 *              >   Set_Array
 *              >   Set_LinkedList
 *              >   Set_HashTable
 *              >   Set_RedBlackTree
 *              >   Set_stlset
 *              >   Set_stlunordered_set (really is hashset)
 *              >   Set_Treap
 *
 *      @todo   Experiment with new operator+/operator- set difference/union functions for ease of use
 *              Would it be better to have global functions (overloading issues?). Or is this OK.
 *
 *              Then see if we can lose the STL/SetUtils code? Gradually ...
 *
 *      @todo   Consider if Union/Difference etc methods should delegate to virtual reps? Or other better
 *              performance approaches? One closely related issue is the backend type returned. Now we use
 *              default but maybe should use left or right side type?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   Set_DefaultTraits {
                /**
                 */
                using   EqualsCompareFunctionType   =   EQUALS_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);

                /**
                 *  Define typedef for this Set traits object (so other traits can generically allow recovery of the
                 *  underlying Set's TRAITS objects.
                 */
                using   SetTraitsType               =   Set_DefaultTraits<T, EQUALS_COMPARER>;
            };


            /**
             *      The Set class is based on SmallTalk-80, The Language & Its Implementation,
             *      page 148.
             *
             *      The basic idea here is that you cannot have multiple copies of the same
             *      thing into the set (like a mathemactical set).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *
             *  \em Concrete Implementations:
             *      o   @see Concrete::Set_LinkedList<>
             *      o   @see Concrete::Set_stdset<>
             *
             *  \em Factory:
             *      @see Concrete::Set_Factory<> to see default implementations.
             *
             *  \em Design Note:
             *      Included <set> and have explicit CTOR for set<> so that Stroika Set can be used more interoperably
             *      with set<> - and used without an explicit CTOR. Use Explicit CTOR to avoid accidental converisons. But
             *      if you declare an API with Set<T> arguments, its important STL sources passing in set<T> work transparently.
             *
             *      Similarly for std::initalizer_list.
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            template    <typename T, typename TRAITS = Set_DefaultTraits<T>>
            class   Set : public UpdatableIterable<T> {
            private:
                using   inherited   =   UpdatableIterable<T>;

            protected:
                class   _IRep;

            protected:
                using   _SetSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SetSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Set<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Set<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using   TraitsType              =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   EqualsCompareFunctionType   =   typename TraitsType::EqualsCompareFunctionType;

            public:
                /**
                 */
                Set ();
                Set (const Set<T, TRAITS>& src);
                Set (Set<T, TRAITS>&& src);
                Set (const initializer_list<T>& src);
                Set (const set<T>& src);
                template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value>::type>
                explicit Set (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Set (const _SharedPtrIRep& rep);
                explicit Set (_SharedPtrIRep&& rep);

#if     qDebug
            public:
                ~Set ();
#endif

            public:
                /**
                 */
                nonvirtual  Set<T, TRAITS>& operator= (const Set<T, TRAITS>& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  Set<T, TRAITS>& operator= (Set<T, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  Set<T, TRAITS>& operator= (Set<T, TRAITS> && rhs) = default;
#endif

            public:
                /**
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 *  Like Contains - but a Set<> can use a comparison that only examines a part of T,
                 *  making it useful to be able to return the rest of T.
                 */
                nonvirtual  Memory::Optional<T> Lookup (T item) const;

            public:
                /**
                 *  Add when T is already present has may have no effect (logically has no effect) on the
                 *  container (not an error or exception).
                 *
                 *  So for a user-defined type T (or any type where the caller specifies the compare function)
                 *  it is left undefined whether or not the new (not included) attributes assocaited with the added
                 *  item make it into the Set.
                 *
                 *  If you really want an association list (Mapping) from one thing to another, use that.
                 */
                nonvirtual  void    Add (T item);

            public:
                /**
                 *  Add item if not already present, and return true if added, and false if already present.
                 *  Note - we chose to return true in the case of addition because this is the case most likely
                 *  when a caller would want to take action.
                 *
                 *  EXAMPLE:
                 *      if (s.AddIf (n)) {
                 *          write_to_disk(n);
                 *      }
                 */
                nonvirtual  bool    AddIf (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

            public:
                /**
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 *  RemoveIf item if not already present. Whether present or not it does the same thing and
                 *  assures the item is no longer present, but returns true iff the call made a change (removed
                 *  the item).
                 *
                 *  Note - we chose to return true in the case of removeal because this is the case most likely
                 *  when a caller would want to take action.
                 *
                 *  EXAMPLE:
                 *      if (s.RemoveIf (n)) {
                 *          write_to_disk(n);
                 *      }
                 */
                nonvirtual  bool    RemoveIf (T item);

            public:
            public:
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& s);
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 *  \em EXPERIMENTAL API (2014-01-08)
                 *
                 *  Apply the function funciton to each element, and return all the ones for which it was true.
                 */
                nonvirtual  Set<T, TRAITS>    EachWith (const function<bool(const T& item)>& doToElement) const;

            public:
                /**
                 *  Two Sets are considered equal if they contain the same elements (by comparing them with operator==).
                 *
                 *  Equals is commutative().
                 *
                 *  @todo - document computational complexity
                 */
                nonvirtual  bool    Equals (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool    Intersects (const Iterable<T>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Set<T, TRAITS>   Intersection (const Iterable<T>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Set<T, TRAITS>   Union (const Iterable<T>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Set<T, TRAITS>   Difference (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 *      Synonym for Add/AddAll.
                 *
                 *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
                 */
                nonvirtual  Set<T, TRAITS>& operator+= (T item);
                nonvirtual  Set<T, TRAITS>& operator+= (const Iterable<T>& items);

            public:
                /**
                 *      Synonym for Remove/RemoveAll.
                 *
                 *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
                 */
                nonvirtual  Set<T, TRAITS>& operator-= (T item);
                nonvirtual  Set<T, TRAITS>& operator-= (const Iterable<T>& items);

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 * \brief STL-ish alias for Add ().
                 */
                nonvirtual  void    insert (T item);

            public:
                /**
                 * \brief STL-ish alias for Remove ().
                 */
                nonvirtual  void    erase (T item);

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
             *  \brief  Implementation detail for Set<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Set<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Set<T, TRAITS>::_IRep : public UpdatableIterable<T>::_IRep {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            protected:
                using   _SharedPtrIRep = typename Set<T, TRAITS>::_SharedPtrIRep;

            public:
                virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const                  =   0;
                virtual bool                Equals (const _IRep& rhs) const                                         =   0;
                virtual bool                Contains (T item) const                                                 =   0;
                virtual Memory::Optional<T> Lookup (T item) const                                                   =   0;
                virtual void                Add (T item)                                                            =   0;
                virtual void                Remove (T item)                                                         =   0;
                virtual void                Remove (const Iterator<T>& i)                                           =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            public:
                nonvirtual bool    _Equals_Reference_Implementation (const _IRep& rhs) const;
            };


            /**
             *  operator indirects to Set<>::Equals()
             */
            template    <typename T, typename TRAITS>
            bool    operator== (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs);

            /**
             *  operator indirects to Set<>::Equals()
             */
            template    <typename T, typename TRAITS>
            bool    operator!= (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs);

            /**
             *  Alias for Set<>::Union
             */
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   operator+ (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs);

            /**
             *  Alias for Set<>::Difference.
             */
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   operator- (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs);

            /**
             *   Alias for Set<>::Intersection.
             */
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   operator^ (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs);


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Set.inl"

#endif  /*_Stroika_Foundation_Containers_Set_h_ */
