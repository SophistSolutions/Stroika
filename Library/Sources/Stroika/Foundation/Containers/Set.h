/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Add/FIX three-way-comparer support - operator<=> etc - for Set. I Think it can be
 *              easily defined as operator<> on ELT, followed by operator<> on value and then as a string
 *              of those operations.
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
 *              >   Set_HashTable
 *              >   Set_RedBlackTree
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

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     * \brief Set<T> is a container of T, where once an item is added, additionally adds () do nothing.
     *
     *      The Set class is based on SmallTalk-80, The Language & Its Implementation,
     *      page 148.
     *
     *      The basic idea here is that you cannot have multiple copies of the same
     *      thing into the set (like a mathematical set).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Set_LinkedList<>
     *      o   @see Concrete::Set_stdset<>
     *
     *  \em Factory:
     *      @see Factory::Set_Factory<> to see default implementations.
     *
     *  \em Design Note:
     *      Included <set> and have explicit CTOR for set<> so that Stroika Set can be used more interoperably
     *      with set<> - and used without an explicit CTOR. Use Explicit CTOR to avoid accidental converisons. But
     *      if you declare an API with Set<T> arguments, its important STL sources passing in set<T> work transparently.
     *
     *      Similarly for std::initalizer_list.
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison equality (==, <=> etc) - because all sets have an equalsComparer for their elements
     *        o ordering (<,<=> etc) not provided, because a set has no intrinsic ordering to its elements
     *        o when comparing a Set to any Itererable<> - this is treated as 'set' equality comparison
     */
    template <typename T>
    class Set : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _SetRepSharedPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _SetRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Set<T>;

    public:
        /**
         *  This is the type returned by GetEqualsComparer () and CAN be used as the argument to a Set<> as EqualityComparer, but
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole Set<>s
         */
        using EqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (T, T)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         *
         *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

    public:
        /**
         *  For the CTOR overload with CONTAINER_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements.
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>, but
         *        for simplicity sake, many of the other constructors force that conversion.
         *
         * \req IsEqualsComparer<EQUALS_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *        Collection<int> c;
         *        std::vector<int> v;
         *
         *        Set<int> s1  = {1, 2, 3};
         *        Set<int> s2  = s1;
         *        Set<int> s3  { s1 };
         *        Set<int> s4  { s1.begin (), s1.end () };
         *        Set<int> s5  { c };
         *        Set<int> s6  { v };
         *        Set<int> s7  { v.begin (), v.end () };
         *        Set<int> s8  { move (s1) };
         *        Set<int> s9  { 1, 2, 3 };
         *        Set<int> s10 { Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }), c };
         *      \endcode
         *
         *  \note Don't apply (CONTAINER_OF_ADDABLE&& src) constructor to non-containers (non-iterables), 
         *        and don't allow it to apply to SUBCLASSES of Set (since then we want to select the Set (const Set& from) constructor)
         */
        Set ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        explicit Set (EQUALS_COMPARER&& equalsComparer);
        Set (const Set& src) noexcept = default;
        Set (Set&& src) noexcept      = default;
        Set (const initializer_list<T>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        Set (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit Set (CONTAINER_OF_ADDABLE&& src);
        template <typename EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set (EQUALS_COMPARER&& equalsComparer, const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        Set (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        Set (EQUALS_COMPARER&& equalsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit Set (const _SetRepSharedPtr& rep) noexcept;
        explicit Set (_SetRepSharedPtr&& rep) noexcept;

#if qDebug
    public:
        ~Set ();
#endif

    public:
        /**
         */
        nonvirtual Set& operator= (const Set& rhs) = default;
        nonvirtual Set& operator= (Set&& rhs) noexcept = default;

    public:
        /**
         *  Return the function used to compare if two elements are equal
         *
         *  \note   @see also EqualsComparer{} to compare whole Set<>s
         *
         *  @todo rename Set<>::GetEqualsComparer() to Set<>::GetElementEqualsComparer
         */
        nonvirtual EqualityComparerType GetEqualsComparer () const;

    public:
        /**
         */
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         *  Checks if each element of this set is contained in the argument set. This is NOT proper subset, but
         *  allows for equality.
         */
        nonvirtual bool IsSubsetOf (const Set<T>& superset) const;

    public:
        /**
         *  Like Contains - but a Set<> can use a comparison that only examines a part of T,
         *  making it useful to be able to return the rest of T.
         */
        nonvirtual optional<T> Lookup (ArgByValueType<T> item) const;

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
         *
         *  \note mutates container
         */
        nonvirtual void Add (ArgByValueType<T> item);

    public:
        /**
         *  Add item if not already present, and return true if added, and false if already present.
         *  Note - we chose to return true in the case of addition because this is the case most likely
         *  when a caller would want to take action.
         *
         *  \par Example Usage
         *      \code
         *          if (s.AddIf (n)) {
         *              write_to_disk (n);
         *          }
         *      \endcode
         *
         *  \note mutates container
         */
        nonvirtual bool AddIf (ArgByValueType<T> item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \note mutates container
         */
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        nonvirtual void AddAll (CONTAINER_OF_ADDABLE&& s);

    public:
        /**
         *  The overload taking an item doesn't require the value exists, but removes it if it does.
         *  The overload taking an iterator requires the iterator is valid.
         *
         *  @see RemoveIf ()
         *
         *  \note mutates container
         */
        nonvirtual void Remove (ArgByValueType<T> item);
        nonvirtual void Remove (const Iterator<T>& i);

    public:
        /**
         *  RemoveIf item if not already present. Whether present or not it does the same thing and
         *  assures the item is no longer present, but returns true iff the call made a change (removed
         *  the item).
         *
         *  Note - we chose to return true in the case of removal because this is the case most likely
         *  when a caller would want to take action.
         *
         *  \par Example Usage
         *      \code
         *          if (s.RemoveIf (n)) {
         *              write_to_disk(n);
         *          }
         *      \endcode
         *
         *  @see Remove ()
         *
         *  \note mutates container
         */
        nonvirtual bool RemoveIf (ArgByValueType<T> item);

    public:
        /**
         *  \note mutates container
         */
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& s);
        nonvirtual void RemoveAll ();

    public:
        /**
         *  Apply the function funciton to each element, and return all the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset.
         *
         *  @see Iterable<T>::Where
         *
         *  \par Example Usage
         *      \code
         *          Set<int> s{ 1, 2, 3, 4, 5 };
         *          VerifyTestResult ((s.Where ([](int i) {return Math::IsPrime (i); }) == Set<int>{ 2, 3, 5 }));
         *      \endcode
         */
        nonvirtual Set<T> Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;

    public:
        struct EqualsComparer;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * simply indirect to @Set<>::EqualsComparer
         */
        nonvirtual bool operator== (const Set& rhs) const;
        nonvirtual bool operator== (const Iterable<T>& rhs) const;
#endif

    public:
        /**
         */
        nonvirtual bool Intersects (const Iterable<T>& rhs) const;

    public:
        /**
         */
        nonvirtual Set<T> Intersection (const Iterable<T>& rhs) const;

    public:
        /**
         */
        nonvirtual Set<T> Union (const Iterable<T>& rhs) const;
        nonvirtual Set<T> Union (ArgByValueType<T> rhs) const;

    public:
        /**
         */
        nonvirtual Set<T> Difference (const Set<T>& rhs) const;
        nonvirtual Set<T> Difference (ArgByValueType<T> rhs) const;

    public:
        /**
         *      Synonym for Add/AddAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual Set<T>& operator+= (ArgByValueType<T> item);
        nonvirtual Set<T>& operator+= (const Iterable<T>& items);

    public:
        /**
         *      Synonym for Remove/RemoveAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual Set<T>& operator-= (ArgByValueType<T> item);
        nonvirtual Set<T>& operator-= (const Iterable<T>& items);

    public:
        /**
         *      Synonym for *this = *this ^ Set<T> {items }
         *
         *  \note mutates container
         */
        nonvirtual Set<T>& operator^= (const Iterable<T>& items);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief STL-ish alias for Add ().
         *
         *  \note mutates container
         */
        nonvirtual void insert (ArgByValueType<T> item);

    public:
        /**
         * \brief STL-ish alias for Remove ().
         *
         *  \note mutates container
         */
        nonvirtual void erase (ArgByValueType<T> item);
        nonvirtual void erase (const Iterator<T>& i);

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
     *  \brief  Implementation detail for Set<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Set<T> container API.
     */
    template <typename T>
    class Set<T>::_IRep : public Iterable<T>::_IRep {
    private:
        using inherited = typename Iterable<T>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _SetRepSharedPtr = typename Set<T>::_SetRepSharedPtr;

    public:
        virtual EqualityComparerType GetEqualsComparer () const                             = 0;
        virtual _SetRepSharedPtr     CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
        virtual bool                 Equals (const typename Iterable<T>::_IRep& rhs) const  = 0;
        virtual bool                 Contains (ArgByValueType<T> item) const                = 0;
        virtual optional<T>          Lookup (ArgByValueType<T> item) const                  = 0;
        virtual void                 Add (ArgByValueType<T> item)                           = 0;
        virtual void                 Remove (ArgByValueType<T> item)                        = 0;
        virtual void                 Remove (const Iterator<T>& i)                          = 0;
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif

        /*
         *  Reference Implementations (often not used except for ensure's, but can be used for
         *  quickie backends).
         *
         *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
         *  the sucblass refers to the method in a subclass virtual override.
         */
    protected:
        /**
         *  \note - this doesn't require a Compare function argument because it indirects to 'Contains'
         */
        nonvirtual bool _Equals_Reference_Implementation (const typename Iterable<T>::_IRep& rhs) const;
    };

    /**
     *  \brief Compare Set<>s or Iterable<>s for equality. 
     *
     *  Two Sets are considered equal if they contain the same elements (by comparing them with EqualsCompareFunctionType (defaults to operator==)).
     *  Note, if two equalsComparer functions are provided, they must produce the same result comparing elements.
     *
     *  \req lhs and rhs arguments must have the same (or equivilent) EqualsComparers.
     *
     *  EqualsComparer is commutative ().
     *
     *  @todo - document computational complexity
     *
     *  \note   This EqualsComparer template is defined even though not needed to provide alternate element comparer to provide extra overloads for
     *          operator()()
     *
     *  \note   If any argument is an Iterable, it is treated/compared as if it was a set (aka Iterable<T>::SetEquals)
     *
     *  \note   Not to be confused with EqualityComparerType and GetEqualsComparer () which compares ELEMENTS of Set<T> for equality.
     */
    template <typename T>
    struct Set<T>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        nonvirtual bool operator() (const Set& lhs, const Set& rhs) const;
        nonvirtual bool operator() (const Set& lhs, const Iterable<T>& rhs) const;
        nonvirtual bool operator() (const Iterable<T>& lhs, const Set& rhs) const;
    };

    /**
     *  Basic comparison operator overloads with the obivous meaning, and simply indirect to @Set<T>::EqualsComparer
     *
     *  \note It remains questionable whether or not we should have overloads for comparting Set<> and Iterable<>. When
     *        also done with other containers like Sequence, this could produce ambiguity. (like comparing Set with Sequence).
     *        But thats probably OK, becase when we have ambiguity, we can always explicitly resolve it. So keep these
     *        overloads which are pretty convenient.
     */
#if __cpp_impl_three_way_comparison < 201907
    template <typename T>
    bool operator== (const Set<T>& lhs, const Set<T>& rhs);
    template <typename T>
    bool operator== (const Set<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    bool operator== (const Iterable<T>& lhs, const Set<T>& rhs);
    template <typename T>
    bool operator!= (const Set<T>& lhs, const Set<T>& rhs);
    template <typename T>
    bool operator!= (const Set<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    bool operator!= (const Iterable<T>& lhs, const Set<T>& rhs);
#endif

    /**
     *  Alias for Set<>::Union
     */
    template <typename T>
    Set<T> operator+ (const Set<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    Set<T> operator+ (const Set<T>& lhs, const T& rhs);

    /**
     *  Alias for Set<>::Difference.
     */
    template <typename T>
    Set<T> operator- (const Set<T>& lhs, const Set<T>& rhs);

    /**
     *   Alias for Set<>::Intersection.
     */
    template <typename T>
    Set<T> operator^ (const Set<T>& lhs, const Iterable<T>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Set.inl"

#endif /*_Stroika_Foundation_Containers_Set_h_ */
