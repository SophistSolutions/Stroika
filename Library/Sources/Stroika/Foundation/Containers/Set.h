/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
 *  TODO:
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
 *      @todo   Consider if Union/Difference etc methods should delegate to virtual reps? Or other better
 *              performance approaches? One closely related issue is the backend type returned. Now we use
 *              default but maybe should use left or right side type?
 * 
 *      @todo   https://stroika.atlassian.net/browse/STK-754 - Add (REP METHOD) should return bool if really added (if size changed) to make a few things 
 *              like addif cheaper. But think through carefully semantics of object - we actually change what is there, but it maybe just pading
 *              soemthing - so DOES have affect but doesnt change if its present or not.
 */

namespace Stroika::Foundation::Containers {

    using Common::IEqualsComparer;
    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterable;
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
     *  \em vs std::set<T>:
     *      Stroika's Set<T> is like std::set<T>, except that
     *          o   you can separately select different algorithms (besides red-black tree) and not change the API used (Set<T>).
     *          o   You don't need to have a less<T> method defined. You just need to provide some mechanism (either operator== or argument to constructor)
     *              saying how to compare elements for equality
     *          o   If you have a less<T> already defined, like std::set<T>, this will be used by default to construct a tree-based set.
     *          o   Sets can also be implemented by hash-tables, etc.
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
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note Note About Update method
     *      We intentionally omit the Update () method since update given an iterator would do the same thing
     *      as Container::Add(). We COULD enhance Add () to take an optional hint parameter in a future version of Stroika.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison equality (==, <=> etc) - because all sets have an equalsComparer for their elements
     *        o ordering (<,<=> etc) not provided, because a set has no intrinsic ordering between the set elements
     *        o when comparing a Set to any Itererable<> - this is treated as 'set' equality comparison
     *
     *        It remains questionable whether or not we should have overloads for comparing Set<> and Iterable<>. When
     *        also done with other containers like Sequence, this could produce ambiguity. (like comparing Set with Sequence).
     *        But thats probably OK, becase when we have ambiguity, we can always explicitly resolve it. So keep these
     *        overloads which are pretty convenient.
     */
    template <typename T>
    class Set : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Set;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a Set<> as EqualityComparer, but
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the IEqualsComparer () concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole Set<>s
         */
        using ElementEqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (T, T)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = is_convertible_v<POTENTIALLY_ADDABLE_T, value_type>;

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements.
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>, but
         *        for simplicity sake, many of the other constructors force that conversion.
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         *
         * \req IEqualsComparer<EQUALS_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *        Collection<int> c;
         *        std::vector<int> v;
         *
         *        Set<int> s1  = {1, 2, 3};
         *        Set<int> s2  = s1;
         *        Set<int> s3{ s1 };
         *        Set<int> s4{ s1.begin (), s1.end () };
         *        Set<int> s5{ c };
         *        Set<int> s6{ v };
         *        Set<int> s7{ v.begin (), v.end () };
         *        Set<int> s8{ move (s1) };
         *        Set<int> s9{ 1, 2, 3 };
         *        Set<int> s10{ Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }), c };
         *      \endcode
         */
        Set ();
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit Set (EQUALS_COMPARER&& equalsComparer);
        Set (Set&& src) noexcept      = default;
        Set (const Set& src) noexcept = default;
        Set (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        Set (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <IIterable<T> ITERABLE_OF_ADDABLE>
        explicit Set (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<decay_t<ITERABLE_OF_ADDABLE>, Set<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Set{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IEqualsComparer<T> EQUALS_COMPARER, IIterable<T> ITERABLE_OF_ADDABLE>
        Set (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit Set (shared_ptr<_IRep>&& rep) noexcept;
        explicit Set (const shared_ptr<_IRep>& rep) noexcept;

    public:
        /**
         */
        nonvirtual Set& operator= (Set&& rhs) noexcept = default;
        nonvirtual Set& operator= (const Set& rhs)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are equal
         *
         *  \note   @see also EqualsComparer{} to compare whole Set<>s
         *
         *  @todo rename Set<>::GetElementEqualsComparer() to Set<>::GetElementEqualsComparer
         */
        nonvirtual ElementEqualityComparerType GetElementEqualsComparer () const;

    public:
        /**
         */
        nonvirtual bool Contains (ArgByValueType<value_type> item) const;

    public:
        /**
         */
        nonvirtual bool ContainsAll (const Iterable<value_type>& items) const;

    public:
        /**
         */
        nonvirtual bool ContainsAny (const Iterable<value_type>& items) const;

    public:
        /**
         *  Checks if each element of this set is contained in the argument set. This is NOT proper subset, but
         *  allows for equality.
         */
        nonvirtual bool IsSubsetOf (const Set& superset) const;

    public:
        /**
         *  Like Contains - but a Set<> can use a comparison that only examines a part of T,
         *  making it useful to be able to return the rest of T.
         */
        nonvirtual optional<value_type> Lookup (ArgByValueType<value_type> item) const;

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
        nonvirtual void Add (ArgByValueType<value_type> item);

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
        nonvirtual bool AddIf (ArgByValueType<value_type> item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \note mutates container
         */
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual void AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IIterable<T> ITERABLE_OF_ADDABLE>
        nonvirtual void AddAll (ITERABLE_OF_ADDABLE&& items);

    public:
        /**
         *  \brief Remove the item (given by value or iterator pointing to it) from the contain. The item MUST exist.
         * 
         *  \req argument (i or item) is present in the Set.
         *
         *  @see RemoveIf () to remove without the requirement that the value exist in the Set
         *
         *  \param nextI - if provided (not null) - will be filled in with the next value after where iterator i is pointing - since i is invalidated by changing the container)
         *
         *  \note mutates container
         */
        nonvirtual void Remove (ArgByValueType<value_type> item);
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  RemoveIf item if present. Whether present or not it does the same thing and
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
        nonvirtual bool RemoveIf (ArgByValueType<value_type> item);

    public:
        /**
         *  \brief RemoveAll removes all, or all matching (predicate, iterator range, equals comparer or whatever) items.
         * 
         *  The no-arg overload removes all (quickly).
         * 
         *  The overloads that remove some subset of the items returns the number of items so removed.
         * 
         *  \note mutates container
         */
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual size_t RemoveAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IIterable<T> ITERABLE_OF_ADDABLE>
        nonvirtual size_t RemoveAll (const ITERABLE_OF_ADDABLE& s);
        nonvirtual void   RemoveAll ();
        template <predicate<T> PREDICATE>
        nonvirtual size_t RemoveAll (PREDICATE&& p);

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
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
        nonvirtual Set Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const;

    public:
        struct EqualsComparer;

    public:
        /**
         * simply indirect to @Set<>::EqualsComparer (always defined because Set<> knows how to compare T items.
         */
        nonvirtual bool operator== (const Set& rhs) const;
        nonvirtual bool operator== (const Iterable<value_type>& rhs) const;

    public:
        /**
         *  \brief return true iff the Intersection () is non-empty
         *
         *  Returns true iff the two specified containers contain at least one element in common. 
         */
        nonvirtual bool Intersects (const Iterable<value_type>& rhs) const;
        static bool     Intersects (const Set& lhs, const Iterable<value_type>& rhs);
        static bool     Intersects (const Iterable<value_type>& lhs, const Set& rhs);
        static bool     Intersects (const Set& lhs, const Set& rhs);

    public:
        /**
         */
        nonvirtual Set Intersection (const Iterable<value_type>& rhs) const;
        static Set     Intersection (const Set& lhs, const Iterable<value_type>& rhs);
        static Set     Intersection (const Iterable<T>& lhs, const Set& rhs);
        static Set     Intersection (const Set& lhs, const Set& rhs);

    public:
        /**
         */
        nonvirtual Set Union (const Iterable<value_type>& rhs) const;
        nonvirtual Set Union (ArgByValueType<value_type> rhs) const;
        static Set     Union (const Set& lhs, const Iterable<value_type>& rhs);
        static Set     Union (const Iterable<value_type>& lhs, const Set& rhs);
        static Set     Union (const Set& lhs, const Set& rhs);

    public:
        /**
         */
        nonvirtual Set Difference (const Set& rhs) const;
        nonvirtual Set Difference (const Iterable<value_type>& rhs) const;
        nonvirtual Set Difference (ArgByValueType<value_type> rhs) const;

    public:
        /**
         *      Synonym for Add/AddAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual Set& operator+= (ArgByValueType<value_type> item);
        nonvirtual Set& operator+= (const Iterable<value_type>& items);

    public:
        /**
         *      Synonym for RemoveIf/RemoveAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual Set& operator-= (ArgByValueType<value_type> item);
        nonvirtual Set& operator-= (const Iterable<value_type>& items);

    public:
        /**
         *      Synonym for *this = *this ^ Set<T> {items }
         *
         *  \note mutates container
         */
        nonvirtual Set& operator^= (const Iterable<value_type>& items);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         *  Note the return value of this find function is an Iterator, when it could have been as easily an optional<T>. Reason
         *  to return an iterator is so that it can be fed back into a Remove (iterator) call which allows more efficient removeal.
         */
        nonvirtual Iterator<value_type> find (ArgByValueType<value_type> item) const;

    public:
        /**
         * \brief STL-ish alias for Add ().
         *
         *  \note mutates container
         */
        nonvirtual void insert (ArgByValueType<value_type> item);

    public:
        /**
         * \brief STL-ish alias for Remove ().
         *
         *  \note mutates container
         */
        nonvirtual void erase (ArgByValueType<value_type> item);
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    protected:
        /**
         *  \brief Utility to get WRITABLE underlying shared_ptr (replacement for what we normally do - _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ())
         *         but where we also handle the cloning/patching of the associated iterator
         * 
         *  When you have a non-const operation (such as Remove) with an argument of an Iterator<>, then due to COW,
         *  you may end up cloning the container rep, and yet the Iterator<> contains a pointer to the earlier rep (and so maybe unusable).
         * 
         *  Prior to Stroika 2.1b14, this was handled elegantly, and automatically, by the iterator patching mechanism. But that was deprecated (due to cost, and
         *  rarity of use), in favor of this more restricted feature, where we just patch the iterators on an as-needed basis.
         * 
         *  \todo @todo - could be smarter about moves and avoid some copies here - I think, and this maybe performance sensitive enough to look into that... (esp for COMMON case where no COW needed)
         */
        nonvirtual tuple<_IRep*, Iterator<value_type>> _GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i);

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

    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const                              = 0;
        virtual shared_ptr<_IRep>           CloneEmpty () const                                            = 0;
        virtual shared_ptr<_IRep>           CloneAndPatchIterator (Iterator<value_type>* i) const          = 0;
        virtual bool                        Equals (const typename Iterable<value_type>::_IRep& rhs) const = 0;
        /**
         *  Return true iff item found.
         *  ONLY IF returns true, and arg oResult is non-null, *oResult set to found item.
         *  ONLY IF returns true, and arg iResult is non-null, *iResult set to found item.
         */
        virtual bool Lookup (ArgByValueType<value_type> item, optional<value_type>* oResult, Iterator<value_type>* iResult) const = 0;
        virtual void Add (ArgByValueType<value_type> item)                                                                        = 0;
        /**
         *  Return true iff item found. Either way assure it doesn't exsist
         */
        virtual bool RemoveIf (ArgByValueType<value_type> item)                          = 0;
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) = 0;

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
        nonvirtual bool _Equals_Reference_Implementation (const typename Iterable<value_type>::_IRep& rhs) const;
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
     *  \note   Not to be confused with ElementEqualityComparerType and GetElementEqualsComparer () which compares ELEMENTS of Set<T> for equality.
     */
    template <typename T>
    struct Set<T>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        nonvirtual bool operator() (const Set& lhs, const Set& rhs) const;
        nonvirtual bool operator() (const Set& lhs, const Iterable<value_type>& rhs) const;
        nonvirtual bool operator() (const Iterable<value_type>& lhs, const Set& rhs) const;
    };

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
    Set<T> operator- (const Set<T>& lhs, const Iterable<T>& rhs);

    /**
     *   Alias for Set<>::Intersection.
     */
    template <typename T>
    Set<T> operator^ (const Set<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    Set<T> operator^ (const Iterable<T>& lhs, const Set<T>& rhs);
    template <typename T>
    Set<T> operator^ (const Set<T>& lhs, const Set<T>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Set.inl"

#endif /*_Stroika_Foundation_Containers_Set_h_ */
