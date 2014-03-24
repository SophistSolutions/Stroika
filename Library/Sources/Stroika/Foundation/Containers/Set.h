/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

#include    "../StroikaPreComp.h"

#include    <set>

#include    "../Configuration/Common.h"
#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/Optional.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
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
             */
            template    <typename T, typename TRAITS = Set_DefaultTraits<T>>
            class   Set : public Iterable<T> {
            private:
                using   inherited   =   Iterable<T>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template _USING_SHARED_IMPL_<_IRep>;

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
                Set (const Set<T, TRAITS>& s);
                Set (const std::initializer_list<T>& s);
                Set (const std::set<T>& s);
                template <typename CONTAINER_OF_T>
                explicit Set (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Set (const _SharedPtrIRep& rep);

#if     qDebug
            public:
                ~Set ();
#endif

            public:
                nonvirtual  Set<T, TRAITS>& operator= (const Set<T, TRAITS>& rhs);

            public:
                /**
                 */
                nonvirtual  bool Contains (T item) const;

            public:
                /**
                 *  Like Contains - but a Set<> can use a comparison that only examines a part of T,
                 *  making it useful to be able to return the rest of T.
                 */
                nonvirtual  Memory::Optional<T> Lookup (T item) const;

            public:
                /**
                 *  Add when T is already present has no effect on the container (not an error or exception).
                 */
                nonvirtual  void    Add (T item);

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
                nonvirtual  bool    Intersects (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Set<T, TRAITS>   Intersection (const Set<T, TRAITS>& rhs) const;

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
                 *  Alias for Union
                 */
                nonvirtual  Set<T, TRAITS>   operator+ (const Iterable<T>& rhs) const;

            public:
                /**
                 *  Alias for Difference.
                 */
                nonvirtual  Set<T, TRAITS>   operator- (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                *   Alias for Intersection.
                */
                nonvirtual  Set<T, TRAITS>   operator^ (const Set<T, TRAITS>& rhs) const;

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
                nonvirtual  Set<T, TRAITS>& operator-= (const Set<T, TRAITS>& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Set<T, TRAITS>& rhs) const;

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
                /**
                 */
                template    <typename T2>
                using _SafeReadRepAccessor = Iterable<T>::_SafeReadRepAccessor<T2>;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Set<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Set<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Set<T, TRAITS>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool                Equals (const _IRep& rhs) const                                 =   0;
                virtual bool                Contains (T item) const                                         =   0;
                virtual Memory::Optional<T> Lookup (T item) const                                           =   0;
                virtual void                RemoveAll ()                                                    =   0;
                virtual void                Add (T item)                                                    =   0;
                virtual void                Remove (T item)                                                 =   0;
                virtual void                Remove (const Iterator<T>& i)                                   =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) =   0;
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
