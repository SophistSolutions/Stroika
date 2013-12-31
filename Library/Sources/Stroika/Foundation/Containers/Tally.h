/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_h_
#define _Stroika_Foundation_Containers_Tally_h_ 1

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
 *      @todo   IMPORTANT - FIX TRAITS support like I did for Mapping/Set<> - Sorted...
 *              see git commit # 3c5bf0ecd686af850ff77761cf94142a33f48588
 *
 *              Key is adding TallyTraitsType to the traits and making generic base class
 *              for Tally<T> - its traits - same as wtih SortedTraits.
 *
 *              Also likewise key for Tally_stdmap<> - cuz now you cannot assign Tally_stdmap<> to
 *              Tally<T>!!!!
 *
 *      @todo   Consider rewriting all Tally<> concrete types using Mapping<T,counttype> concrete impl?
 *              Might not work easily but document why... (Add () semantics - but maybe).
 *
 *      @todo   AddAll() and CTOR for Tally (and SortedTally and concrete types) is confused by having
 *              overload taking T* and TallyEntry<T>*. Issue is that we cannot do templated iterator
 *              and templated objhect CTOR while these are iteratored (without mcuh better partial
 *              template specializaiton - I THINK????). Maybe use different method for one or the other
 *              to distinguish?
 *
 *              USE SFINAE stuff we used in Mapping<> etc. Simplify AddAll and do the magic in Add.
 *
 *      @todo   Current DOCS for Tally::Remove() say that for variant T, count, the value MUST be present.
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
            struct   Tally_DefaultTraits {
                /**
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
            };


            template    <typename T>
            class   TallyEntry {
            public:
                TallyEntry (T item);
                TallyEntry (T item, size_t count);

                nonvirtual  bool    operator== (const TallyEntry<T>& rhs) const;
                nonvirtual  bool    operator!= (const TallyEntry<T>& rhs) const;

                T       fItem;
                size_t  fCount;
            };


            /**
             *  A Tally<T, TRAITS> a collection of T elements, but where each time you add something, the tally
             *  tracks the number of times that thing has been entered. This is not a commonly used class,
             *  but handy when you want to count things.
             *
             *  Tally<T, TRAITS> inherits from Iterable<TallyEntry<T>> instead of Iterable<T> because if you are
             *  using a Tally, you probably want access to the counts as you iterate - not just the
             *  unique elements (though we make it easy to get that iterator too with MakeBagIterator()).
             *
             *  A Tally<T, TRAITS> makes no promises about ordering of elements in iteration.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Tally_DefaultTraits<T>>
            class   Tally : public Iterable<TallyEntry<T>> {
            private:
                typedef Iterable<TallyEntry<T>> inherited;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                typedef Tally<T, TRAITS>    ArchetypeContainerType;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;

            public:
                /**
                 *      \brief  TallyOfElementType is just a handly copy of the *T* template type which this
                 *              Tally<T, TRAITS> parameterizes counting.
                 */
                typedef T   TallyOfElementType;

            protected:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                Tally ();
                Tally (const Tally<T, TRAITS>& src);
                Tally (const std::initializer_list<T>& s);
                Tally (const std::initializer_list<TallyEntry<T>>& s);
                template    <typename CONTAINER_OF_T>
                explicit Tally (const CONTAINER_OF_T& src);
                Tally (const T* start, const T* end);
                Tally (const TallyEntry<T>* start, const TallyEntry<T>* end);

            protected:
                explicit Tally (const _SharedPtrIRep& rep);

            public:
                /**
                 *  Contains (item) is equivilent to TallyOf (item) != 0, but maybe faster (since it doesn't need to compute
                 *  the fully tally).
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 */
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const TallyEntry<T>& item);

            public:
                /**
                 */
                nonvirtual  void    AddAll (const T* start, const T* end);
                nonvirtual  void    AddAll (const TallyEntry<T>* start, const TallyEntry<T>* end);
                template <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& src);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 *
                 *  If using the item/count or just item overloads, then Tally<> requires that the removed items are present.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (T item, size_t count);
                nonvirtual  void    Remove (const Iterator<TallyEntry<T>>& i);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    RemoveAll (T item);

            public:
                /**
                 * if newCount == 0, equivilent to Remove(i). Require not i.Done () - so it must point to a given item.
                 */
                nonvirtual  void    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount);

            public:
                /**
                 *  TallyOf() returns the number of occurences of 'item' in the tally. The items are compared with operator==.
                 *
                 *  If there are no copies of item in the Tally, 0 is returned.
                 */
                nonvirtual  size_t  TallyOf (T item) const;

            public:
                /**
                 *  Returns the sum of all tallys of all contained elements. This is equivilent
                 *  to Elements ().size ().
                 */
                nonvirtual  size_t  TotalTally () const;

            public:
                /**
                 * This is like the Tally was a Bag<T>. If something is in there N times,
                 *  it will show up in iteration N times. No guarnatee is made as to order of iteration.
                 *
                 *  Use Example:
                 *      Tally<T> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying Tally<> will
                 *  appear in the Elements() Iterable<T>.
                 *
                 *  @see UniqueElements
                 */
                nonvirtual  Iterable<T>   Elements () const;

            public:
                /**
                 *  Use Example:
                 *      Tally<T> t;
                 *      for (T i : t.UniqueElements ()) {
                 *      }
                 *
                 *  UniqueElements () makes no guarantess about whether or not modifications to the underlying Tally<>
                 *  will appear in the UniqueElements() Iterable<T>.
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /*
                 *  Two Tally are considered equal if they contain the same elements (by comparing them with operator==)
                 *  with the same count. In short, they are equal if TallyOf() each item in the LHS equals the TallyOf()
                 *  the same item in the RHS.
                 *
                 *  Equals is commutative.
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Tally<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Synonym for Equals() (or !Equals());
                 */
                nonvirtual  bool    operator== (const Tally<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Tally<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Synonym for Add (), or AddAll() (depending on argument);
                 */
                nonvirtual  Tally<T, TRAITS>&   operator+= (T item);
                nonvirtual  Tally<T, TRAITS>&   operator+= (const Tally<T, TRAITS>& t);

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep : public Iterable<TallyEntry<T>>::_IRep, public enable_shared_from_this<typename Tally<T, TRAITS>::_IRep> {
            private:
                typedef typename Iterable<TallyEntry<T>>::_IRep  inherited;

            protected:
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            protected:
                _IRep ();

            public:
                virtual bool        Equals (const _IRep& rhs) const                                 =   0;
                virtual bool        Contains (T item) const                                         =   0;
                virtual void        RemoveAll ()                                                    =   0;
                virtual void        Add (T item, size_t count)                                      =   0;
                virtual void        Remove (T item, size_t count)                                   =   0;
                virtual void        Remove (const Iterator<TallyEntry<T>>& i)                       =   0;
                virtual void        UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) =   0;
                virtual size_t      TallyOf (T item) const                                          =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T> Elements (const _SharedPtrIRep& rep) const                      =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T> UniqueElements (const _SharedPtrIRep& rep) const                =   0;

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
#include    "Tally.inl"

#endif  /*_Stroika_Foundation_Containers_Tally_h_ */
