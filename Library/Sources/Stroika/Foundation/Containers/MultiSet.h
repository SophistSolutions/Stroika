/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_h_
#define _Stroika_Foundation_Containers_MultiSet_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   use
 *              using   CountType = size_t;
 *              But tricky part is has to be folded into MultiSetEntry<>...
 *
 *      @todo   IMPORTANT - FIX TRAITS support like I did for Mapping/Set<> - Sorted...
 *              see git commit # 3c5bf0ecd686af850ff77761cf94142a33f48588
 *
 *              Key is adding MultiSetTraitsType to the traits and making generic base class
 *              for MultiSet<T> - its traits - same as wtih SortedTraits.
 *
 *              Also likewise key for MultiSet_stdmap<> - cuz now you cannot assign MultiSet_stdmap<> to
 *              MultiSet<T>!!!!
 *
 *      @todo   Fix MultiSet<> CTOR overload taking pointers (sb iterators) - overload so both cases -
 *              Iterator<T> and Iterator<MultisetEntry<T>>.
 *
 *      @todo   Consider rewriting all MultiSet<> concrete types using Mapping<T,counttype> concrete impl?
 *              Might not work easily but document why... (Add () semantics - but maybe).
 *
 *      @todo   AddAll() and CTOR for MultiSet (and SortedMultiSet and concrete types) is confused by having
 *              overload taking T* and MultiSetEntry<T>*. Issue is that we cannot do templated iterator
 *              and templated objhect CTOR while these are iteratored (without mcuh better partial
 *              template specializaiton - I THINK????). Maybe use different method for one or the other
 *              to distinguish?
 *
 *              USE SFINAE stuff we used in Mapping<> etc. Simplify AddAll and do the magic in Add.
 *
 *      @todo   Current DOCS for MultiSet::Remove() say that for variant T, count, the value MUST be present.
 *              But I think this is not in the spirit we've used elsewhere, due to multithreading.
 *              Better to allow them to not be present, else hard to synchonize (check and remove)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   MultiSet_DefaultTraits {
                /**
                 */
                using   EqualsCompareFunctionType   =   EQUALS_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
            };


            template    <typename T>
            class   MultiSetEntry {
            public:
                MultiSetEntry (T item);
                MultiSetEntry (T item, size_t count);
                MultiSetEntry (pair<T, size_t> item);

            public:
                nonvirtual  bool    operator== (const MultiSetEntry<T>& rhs) const;
                nonvirtual  bool    operator!= (const MultiSetEntry<T>& rhs) const;

            public:
                T       fItem;
                size_t  fCount;
            };


            /**
             *  A MultiSet<T, TRAITS> a collection of T elements, but where each time you add something, the tally
             *  tracks the number of times that thing has been entered. This is not a commonly used class,
             *  but handy when you want to count things.
             *
             *  MultiSet<T, TRAITS> inherits from Iterable<MultiSetEntry<T>> instead of Iterable<T> because if you are
             *  using a MultiSet, you probably want access to the counts as you iterate - not just the
             *  unique elements (though we make it easy to get that iterator too with Elements () or
             *  UniqueElements ()).
             *
             *  A MultiSet<T, TRAITS> makes no promises about ordering of elements in iteration.
             *
             *  See also http://en.wikipedia.org/wiki/Multiset_(abstract_data_type)#Multiset
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note   \em Aliases         Tally (Stroika 1.0), Bag (from SmallTalk-80)
             *
             */
            template    <typename T, typename TRAITS = MultiSet_DefaultTraits<T>>
            class   MultiSet : public Iterable<MultiSetEntry<T>> {
            private:
                using   inherited   =   Iterable<MultiSetEntry<T>>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using       ArchetypeContainerType      =   MultiSet<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using       TraitsType                  =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using       EqualsCompareFunctionType   =   typename TraitsType::EqualsCompareFunctionType;

            public:
                /**
                 *      \brief  MultiSetOfElementType is just a handly copy of the *T* template type which this
                 *              MultiSet<T, TRAITS> parameterizes counting.
                 */
                using   MultiSetOfElementType  =   T;

            protected:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                MultiSet ();
                MultiSet (const MultiSet<T, TRAITS>& src);
                MultiSet (const std::initializer_list<T>& s);
                MultiSet (const std::initializer_list<MultiSetEntry<T>>& s);
                template    <typename CONTAINER_OF_T>
                explicit MultiSet (const CONTAINER_OF_T& src);
                MultiSet (const T* start, const T* end);
                MultiSet (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end);

            protected:
                explicit MultiSet (const _SharedPtrIRep& rep);

#if     qDebug
            public:
                ~MultiSet ();
#endif

            public:
                /**
                 *  Contains (item) is equivilent to OccurrencesOf (item) != 0, but maybe faster (since it doesn't need to compute
                 *  the fully tally).
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 */
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const MultiSetEntry<T>& item);

            public:
                /**
                 */
                nonvirtual  void    AddAll (const T* start, const T* end);
                nonvirtual  void    AddAll (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& src);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 *
                 *  If using the item/count or just item overloads, then MultiSet<> requires that the removed items are present.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (T item, size_t count);
                nonvirtual  void    Remove (const Iterator<MultiSetEntry<T>>& i);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    RemoveAll (T item);

            public:
                /**
                 * if newCount == 0, equivilent to Remove(i). Require not i.Done () - so it must point to a given item.
                 */
                nonvirtual  void    UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount);

            public:
                /**
                 *  OccurrencesOf() returns the number of occurences of 'item' in the tally. The items are compared with operator==.
                 *
                 *  If there are no copies of item in the MultiSet, 0 is returned.
                 */
                nonvirtual  size_t  OccurrencesOf (T item) const;

            public:
                /**
                 *  Returns the sum of all tallys of all contained elements. This is equivilent
                 *  to Elements ().size ().
                 */
                nonvirtual  size_t  TotalOccurrences () const;

            public:
                /**
                 * This is like the MultiSet was a Bag<T>. If something is in there N times,
                 *  it will show up in iteration N times. No guarnatee is made as to order of iteration.
                 *
                 *  Use Example:
                 *      MultiSet<T> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying MultiSet<> will
                 *  appear in the Elements() Iterable<T>.
                 *
                 *  @see UniqueElements
                 */
                nonvirtual  Iterable<T>   Elements () const;

            public:
                /**
                 *  Use Example:
                 *      MultiSet<T> t;
                 *      for (T i : t.UniqueElements ()) {
                 *      }
                 *
                 *  UniqueElements () makes no guarantess about whether or not modifications to the underlying MultiSet<>
                 *  will appear in the UniqueElements() Iterable<T>.
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /*
                 *  Two MultiSet are considered equal if they contain the same elements (by comparing them with operator==)
                 *  with the same count. In short, they are equal if OccurrencesOf() each item in the LHS equals the OccurrencesOf()
                 *  the same item in the RHS.
                 *
                 *  Equals is commutative.
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const MultiSet<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Synonym for Equals() (or !Equals());
                 */
                nonvirtual  bool    operator== (const MultiSet<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const MultiSet<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Synonym for Add (), or AddAll() (depending on argument);
                 */
                nonvirtual  MultiSet<T, TRAITS>&   operator+= (T item);
                nonvirtual  MultiSet<T, TRAITS>&   operator+= (const MultiSet<T, TRAITS>& t);

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<MultiSetEntry<T>>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<MultiSetEntry<T>>& s)
                        : fAccessor (s._GetReadOnlyIterableIRepReference ())
                    {
                    }
                    nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const
                    {
                        EnsureMember (fAccessor.get (), REP_SUB_TYPE);
                        return static_cast<const REP_SUB_TYPE&> (*fAccessor.get ());   // static cast for performance sake - dynamic cast in Ensure
                    }
                };
#else
                /**
                 */
                template    <typename T2>
                using   _SafeReadRepAccessor = typename Iterable<MultiSetEntry<T>>::template _SafeReadRepAccessor<T2>;

#endif

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            using   Traversal::IteratorOwnerID;


            /**
             */
            template    <typename T, typename TRAITS>
            class   MultiSet<T, TRAITS>::_IRep
                : public Iterable<MultiSetEntry<T>>::_IRep
#if     !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                                                 , public Traversal::IterableBase::_USING_SHARED_enable_shared_from_this_IMPL_<typename MultiSet<T, TRAITS>::_IRep>
#endif
            {
            private:
                using   inherited   =   typename Iterable<MultiSetEntry<T>>::_IRep;

            protected:
                using   _SharedPtrIRep  =   typename MultiSet<T, TRAITS>::_SharedPtrIRep;

            protected:
                _IRep ();

            public:
                virtual bool        Equals (const _IRep& rhs) const                                         =   0;
                virtual bool        Contains (T item) const                                                 =   0;
                virtual void        RemoveAll ()                                                            =   0;
                virtual void        Add (T item, size_t count)                                              =   0;
                virtual void        Remove (T item, size_t count)                                           =   0;
                virtual void        Remove (const Iterator<MultiSetEntry<T>>& i)                            =   0;
                virtual void        UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)      =   0;
                virtual size_t      OccurrencesOf (T item) const                                            =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T> Elements (const _SharedPtrIRep& rep) const                              =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T> UniqueElements (const _SharedPtrIRep& rep) const                        =   0;
#if     qDebug
                virtual void        AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            protected:
                nonvirtual bool         _Equals_Reference_Implementation (const _IRep& rhs) const;
                nonvirtual Iterable<T>  _Elements_Reference_Implementation (const _SharedPtrIRep& rep) const;
                nonvirtual Iterable<T>  _UniqueElements_Reference_Implementation (const _SharedPtrIRep& rep) const;

            private:
                struct  ElementsIteratorHelperContext_;
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  ElementsIteratorHelper_;

            protected:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  _ElementsIterableHelper;


            private:
                struct  UniqueElementsIteratorHelperContext_;
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  UniqueElementsIteratorHelper_;

            protected:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  _UniqueElementsHelper;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MultiSet.inl"

#endif  /*_Stroika_Foundation_Containers_MultiSet_h_ */
