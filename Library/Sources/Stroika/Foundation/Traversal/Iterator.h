/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_h_
#define _Stroika_Foundation_Traversal_Iterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <iterator>
#include <memory>
#include <optional>
#include <string>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"

/**
 *
 *  \file
 *
 *  TODO
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-446 - AssertExternallySynchronizedMutex
 *
 *      @todo   Consider if we want to make the promise currently defined below in Equals()
 *              about iterating two originally equal iterators. The trouble is - this doesn
 *              work with generators!!! -- REVIEW-- LGP 2013-12-30
 *
 *              THIS IS BAD AND MUST BE REWRITEN - NOT WAHT WE WANT - TOO STRONG A PROMISE.
 *
 *      @todo   Speed tweaks
 *
 *              The Major Design limitation of this approach to iterators is that it requires a non-inlinable
 *              function call per iteration (roughly). Basically - if you pass a callback into an iterator rep
 *              then to APPLY that function on each iteration requires a non-inlinable (indirect through pointer)
 *              function call, or if you do the reverse, and directly use the iterator so no you can inline
 *              apply the function, you must call a virtual function for each iteration to bump the next pointer.
 *
 *              Fundamentally - you have multiple polymorphism (on representation of container and thing to apply
 *              at each iteration).
 *
 *              Two tricks:
 *              (1)     Paging.
 *                      Read a bunch at a time. This is tricky to implement and doesn't affect overall computational
 *                      complexity (because it reduces number of virtual calls by a constant factor). But if that
 *                      constant factor is big enough - 10-100-1000? - that still could be relevant pragmatically.
 *
 *                      The biggest challenge is preserving the existing safety and patch semantics generically,
 *                      in light of update during iteration, and making sure for uses where that doesn't help its
 *                      not allowed to hurt.
 *
 *              (2)     special case common combinations. For example - at the point in the code where you have:
 *                      Iterator<T> i;
 *                      for (; i != e.end (); ++i) {
 *                          do_this();
 *                      }
 *
 *                      One could put in special purpose code for the Iterator<T>::operator++ that said:
 *                      if (dynamic_cast<Sequence_Array::_IRep*> (myIRep) != nullptr) {
 *                          then peek and cheat...
 *                      }
 *                      Maybe when the iterator is constructed - it checks for a couple important types
 *                      and sets a flag, so the only cost when this doesn't work is checking that bool flag.
 *                      And the benefit in the more common case is you avoid the virtual function call! so the it++ can be
 *                      inlined (a big win often times).
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Identical type to std::iterator<> - but duplicated here because std::iterator<> was deprecated in C++17.
     *  We just need a handy way to capture all the defaults/properties for our iterator class.
     */
    template <typename CATEGORY, typename T, typename DIFF = ptrdiff_t, typename POINTER = const T*, typename REFERENCE = const T&>
    struct DefaultIteratorTraits {
        using iterator_category = CATEGORY;
        using value_type        = T;
        using difference_type   = DIFF;
        using pointer           = POINTER;
        using reference         = REFERENCE;
    };

    /**
     *  \brief
     *      An Iterator<T> is a copyable object which allows traversing the contents of some container. It is like an std::const_iterator.
     *
     *  \@todo EXPLAIN HOW THIS IS CONNECTED TO c++20 'range' and probably make this work with ranges!!!
     * 
     *  An Iterator<T> is typically associated with some container (that is being iterated over)
     *  and which allows traversal from start to finish.
     *  (The iterator itself essentially provides a notion of *start* to *finish*).
     *
     *  There need not actually be a 'container' object. Other 'iterators' can be created, so long as
     *  they act like an iterator.
     *
     *  An Iterator<T> is a copyable object which can safely be used to capture (copy) the state of
     *  iteration and continue iterating from that spot.
     * 
     *  An Iterator<T> be be thought of as (const) referencing a container (or other information source)
     *  
     *  It is (since Stroika 2.1b14) illegal to use an iterator after its underlying container
     *  has been modified (rule as in STL, but unlike most STLs, Stroika will automatically detect such
     *  illegal use in debug builds).
     *
     *  Iterators CAN be used to MODIFY a container, but not directly - only by passing that iterator as an
     *  argument to a container method (such as Remove). Here the iterator cannot actually update the container
     *  but acts as an marker/indicator of what element to update. Such APIs will optionally return an updated iterator,
     *  so that you can continue with iteration (if desired).
     * 
     *  \note PRIOR to Stroika 2.1b14 it was true that
     * 
     *        "If the underlying container is modified, the iterator will be automatically
     *        updated to logically account for that update. Iterators are robust in the presence
     *        of changes to their underlying container. Adding or removing items from a container
     *        will not invalidate the iteration."
     *
     *        "Different kinds of containers can make further guarantees about the behavior of iterators
     *        in the presence of container modifications. For example a SequenceIterator will always
     *        traverse any items added after the current traversal index, and will never traverse items
     *        added with an index before the current traversal index.
     *
     *        But this is NO LONGER TRUE.
     *
     *  \par Example Usage
     *      \code
     *          for (Iterator<T> i = container.MakeIterator (); not i.Done (); i.Next ())  {
     *              f (i.Current ());
     *          }
     *      \endcode
     *
     *  or:
     *      \code
     *          for (Iterator<T> i = container.begin (); i; ++i)  {
     *              f (*i);
     *          }
     *      \endcode
     *
     *  or:
     *      \code
     *          for (Iterator<T> i = container.begin (); i != container.end (); ++i)  {
     *              f (*i);
     *          }
     *      \endcode
     *
     *  or:
     *      \code
     *          for (T i : container)  {
     *              f (i);
     *          }
     *      \endcode
     *
     *  Key Differences between Stroika Iterators and STL Iterators:
     *
     *      1.      Stroika iterators (in debug builds) will detect if they are used after
     *              the underlying container has changed (some STL's may do this too?)
     *
     *      2.      Stroika iterators carry around their 'done' state all in one object.
     *              For compatibility with existing C++ idiom, and some C++11 language features
     *              Stroika iterators inherit from std::iterator<> and allow use of end(),
     *              and i != end() to check for if an iterator is done. But internally,
     *              Stroika just checks i.Done(), and so can users of Stroika iterators.
     *
     *      3.      Stroika iterators are not 'random access'. They just go forwards, one step at a
     *              time. In STL, some kinds of iterators act more like pointers where you can do
     *              address arithmetic.
     *              <<<< RETHINK - WE WANT BIDIITERATOR/ETC>>>>
     *
     *      4.      In STL, reverse iterators are a special type, incompatible with regular iterators.
     *              In Stroika, reverse iterators are also created with rbegin(), rend (), but
     *              their type is no different than regular iterators.
     *              <<<< NYI >>>>
     *
     * Some Rules about Iterators:
     *
     *      1.      Iterators can be copied. They always refer to the same
     *              place they did before the copy, and the old iterator is unaffected
     *              by iteration in the new iterator.
     *
     *  Interesting Design (Implementation) Notes:
     *
     *      -   We considered a design for Current() - where it would dynamically
     *          grab the current value, as opposed to being defined to be frozen/copied
     *          at the time of iteration.
     *
     *          The advantage of the path not taken is that if you iterated several times without
     *          examining the current value, and if the cost of copying was relatively large, this
     *          definition would have worked out better.
     *
     *          However, because I think its far more frequent that the copies are cheap, the
     *          user will want to look at each value, and the cost in terms of thread locking
     *          and probably virtual function calls, the current approach of freezing / copying
     *          on iteration seemed better.
     *
     *  \note   Design Note
     *          Until Stroika 2.1d6, Iterator<> used CopyOnWrite (COW) - SharedByValue, instead of unique_ptr.
     *
     *          SharedByValue costs a bit more when the iterators are never copied. But saves a lot of cost when iterators
     *          are copied (cuz with unique_ptr they need to actually be cloned).
     *
     *          I DID run some simple tests to see how often we even use the Clone method. It turns out - quite rarely.
     *          And most can be eliminated by slightly better Move constructor support on the iterator class.
     *
     *  \note Satisfies Concepts:
     *      o   static_assert (regular<Iterator<T>>);
     *      o   static_assert (input_iterator<Iterator<T>>);
     *      o   static_assert (sentinel_for<default_sentinel_t, Iterator<T>>);
     * 
     *  @see Iterable<T>
     *
     *  \note   \em Thread-Safety
     *
     *          Iterator<T> instances are \em not thread-safe. That is - they cannot be read and
     *          or written from multiple threads at a time.
     *
     *          However, given how Iterators are meant to be, and are typically, used, this presents
     *          no problem.
     *
     *          They can be safely transferred across threads, and the underlying things being iterated over
     *          can be safely and transparently read/written from other threads
     *
     *          <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename ITERATOR_TRAITS = DefaultIteratorTraits<forward_iterator_tag, T>>
    class Iterator {
    public:
        static_assert (constructible_from<optional<T>, T>,
                       "Must be able to create optional<T> to use Iterator, because Iterator uses this internally");
        static_assert (copyable<T>); // cannot use as type constraint on T cuz fails?? ill understood - probably complex usages with incomplete types..

    public:
        /**
         *  \brief  difference_type = typename ITERATOR_TRAITS::difference_type;
         */
        using difference_type = typename ITERATOR_TRAITS::difference_type;

    public:
        /**
         *  \brief  value_type = typename ITERATOR_TRAITS::value_type;
         */
        using value_type = typename ITERATOR_TRAITS::value_type;

    public:
        /**
         *  \brief  pointer = typename ITERATOR_TRAITS::pointer;
         */
        using pointer = typename ITERATOR_TRAITS::pointer;

    public:
        /**
         *  \brief  reference = typename ITERATOR_TRAITS::reference;
         */
        using reference = typename ITERATOR_TRAITS::reference;

    public:
        /**
         *  \brief  iterator_category = typename ITERATOR_TRAITS::iterator_category;
         */
        using iterator_category = typename ITERATOR_TRAITS::iterator_category;

    public:
        class IRep;

    public:
        /**
         */
        using RepSmartPtr [[deprecated ("Since Stroika v3.0d1 - just use unique_ptr<IRep> directly")]] = unique_ptr<IRep>;

    private:
        /*
         *  Mostly internal type to select a constructor for the special END iterator.
         */
        enum class ConstructionFlagForceAtEnd_ {
            ForceAtEnd
        };

    public:
        /**
         *  \brief
         *      This overload is usually not called directly. Instead, iterators are
         *      usually created from a container (eg. Bag<T>::begin()).
         *
         *  Iterators are safely copyable, preserving their current position.
         *
         *  CTOR overload taking nullptr - is the same as GetEmptyIterator ()
         *
         *  \note since copy constructor calls Clone_, these can throw exceptions but move copies/assignments are no-except
         * 
         *  \note for ranges to work, the return type of Iterable<T>::end () must be a 'sentinel_for' compatible concept
         *        which implies it must be default constructible. So interpret default construction of Iterator as meaning empty/end sentinel.
         * 
         *  \req RequireNotNull (rep.get ())
         */
        Iterator (const unique_ptr<IRep>& rep) noexcept;
        Iterator (unique_ptr<IRep>&& rep) noexcept;
        Iterator (Iterator&& src) noexcept;
        Iterator (const Iterator& src);
        constexpr Iterator (const default_sentinel_t&) noexcept;
        constexpr Iterator (nullptr_t) noexcept;
        constexpr Iterator () noexcept;

    private:
        constexpr Iterator (ConstructionFlagForceAtEnd_) noexcept;

    public:
        /**
         *  \brief  Iterators are safely copyable, preserving their current position. Copy-Assigning could throw since it probably involves a Clone()
         */
        nonvirtual Iterator& operator= (Iterator&& rhs) noexcept;
        nonvirtual Iterator& operator= (const Iterator& rhs);

    public:
        /**
         *  \brief
         *      Return the Current value pointed to by the Iterator<T> (same as Current())
         *
         *  Support for range-based-for, and STL style iteration in general (containers must also
         *  support begin, end).
         *
         *  This function is a synonym for @ref Current();
         * 
         *  \note Until Stroika 2.1r1, this returned T, and was switched to return const T& on the theory that it might
         *        perform better, but testing has not confirmed that (though this does appear to be existing practice in things like STL).
         * 
         *  \note It is illegal (but goes undetected) to hang onto (and use) the reference returned past when the iterator is next modified
         * 
         *  \note use of for (auto& c : Iterable<>) won't work with Stroika Iterator<> classes since operator* returns const reference only
         *        as we don't allow updating containers by fiddling with the iterator only.
         */
        nonvirtual const T& operator* () const;

    public:
        /**
         *  \brief
         *      Return a pointer to the current value pointed to by the Iterator<T> (like Current())
         *
         *  This function allows you to write i->b, where i is an iterator and b is a member of the type
         *  iterated over by i.
         *
         *  Note - the lifetime of this pointer is short - only until the next operation on the wrapper
         *  class instance Iterator<T>.
         */
        nonvirtual const value_type* operator->() const;

    public:
        /**
         *  \brief
         *      preincrement
         *
         *      Advance iterator; support for range-based-for, and STL style iteration in
         *      general (containers must also support begin, end).
         *
         *  Advance iterator; support for range-based-for, and STL style iteration in general
         *  (containers must also support begin, end).
         *
         *  operator++ can be called anytime as long as Done () is not true (must be called
         *  prior to operator++). It then it iterates to the  item in the container (i.e. it
         *  changes the value returned by Current).
         *
         *  Note - the value return by Current() is frozen (until the next operator++() call)
         *  when this method is called. Its legal to update the underlying container, but those
         *  values won't be seen until the next iteration.
         * 
         *  \note As of Stroika v3.0d1, we again support post-increment (operator++ (int)), NOT because its useful, but because its required by std+c++
         *        to be considered an input iterator (see https://en.cppreference.com/w/cpp/iterator/weakly_incrementable).
         * 
         *        It is slower, and not recommended. BUT because of this - supported.
         */
        nonvirtual Iterator& operator++ ();
        nonvirtual Iterator  operator++ (int);

    public:
        /*
         *  \req operator++ can be called 'i' times (on a copy of this), and the result returned.
         *
         *  \note   don't use unsigned 'i' because that works less well with overloads and ambiguity.
         *  \note   similar to std::advance, but allows for simpler usage (i + n)
         */
        nonvirtual Iterator operator+ (int i) const;

    public:
        /*
         *      \brief  return not Done ()
         *
         *  \em Design Note:
         *      I HATE type punning - which this is. And I may want to lose this.
         *
         *      However, this API works beautifully with Iterable<>::Find - and perhaps other things that
         *      return iterators.
         *
         *      also, it allows
         *          Iterator<T> n = ...;
         *          while (n) {
         *          }
         *          not sure that's better than while (not n.Done ())???
         */
        nonvirtual explicit operator bool () const;

    public:
        /**
         *  \brief Equals () checks if two iterators are equal to one another (point to the same position in the sequence).
         *
         *  \em NB: Equals () is the same notion of equality as used by STL iterators.
         *
         *  \em NB: It is \req required that the two iterators being compared must come from the same source, or from the special source nullptr.
         *
         *  Very roughly, the idea is that to be 'equal' - two iterators must be iterating over the same source,
         *  and be up to the same position. The slight exception to this is that any two iterators that are Done()
         *  are considered Equals (). This is mainly because we use a different representation for 'done'
         *  iterators.
         *
         *  @TODO - NOTE - SEE TODO ABOUT ABOUT THIS GUARANTEE??? - NO - TOO STRONG - REVISE!!!
         *
         *  Note - for Equals. The following assertion will succeed:
         *
         *          Iterator<T> x = getIterator();
         *          Iterator<T> y = x;
         *          x++;
         *          y++;
         *          Assert (Equals (x, y));     // assume x and y not already 'at end' then...
         *                                      // will always succeed (x++ and y++ )
         *
         *  However,
         *          Iterator<T> x = getIterator();
         *          Iterator<T> y = x;
         *          x++;
         *          modify_underlying_container()
         *          y++;
         *          if (Equals (x, y)) {
         *              may or may not be so
         *          }
         *
         *  Note that Equals is *commutative*.
         */
        nonvirtual bool operator== (const Iterator& rhs) const;
        nonvirtual bool operator== (const default_sentinel_t& rhs) const;

    public:
        /**
         *  \brief
         *  Returns the value of the current item visited by the Iterator<T>, and is illegal to call if Done()
         *
         *  Current() returns the value of the current item visited by the Iterator<T>.
         *
         *  Only one things can change the current value of Current():
         *      o   any non-const method of the iterator
         *
         *  Two subsequent calls to *it *cannot* return different values with no
         *  intervening (non-const) calls on the iterator.
         *
         *  The value of returned is undefined (Assertion error) if called when Done().
         *
         *  operator*() is a common synonym for Current().
         *
         *  @see operator*()
         *  @see operator++()
         *
         *  \note Until Stroika 2.1r1, this returned T, and was switched to return const T& on the theory that it might
         *        perform better, but testing has not confirmed that (though this does appear to be existing practice in things like STL).
         * 
         *  \note It is illegal (but goes undetected) to hang onto (and use) the reference returned past when the iterator is next modified
         */
        nonvirtual const T& Current () const;

    public:
        /**
         *  \brief
         *      Done () means there is nothing left in this iterator (a synonym for (it == container.end ()).
         *
         *  Done () means there is nothing left to visit in this iterator.
         *
         *  Once an iterator is Done(), it can never transition to 'not Done()'.
         *
         *  When an iterator is Done(), it is illegal to call Current().
         *
         *  Calling Done() *may* change (initialize) the value which would be returned by the next
         *  call to Current().
         *
         *      NB: There are *no* modifications to an underlying container which will directly change
         *      the value of Done(). This value only changes the next time the cursor is advanced
         *      via a call to operator++();
         *
         *      if it comes from container, then (it == container.end ()) is true iff it.Done()
         */
        nonvirtual bool Done () const;

    public:
        /**
         *  \brief
         *      Set to done and disassociate with owner.
         *
         *   Equivalent to *this = GetEmptyIterator();
         *
         *  @see clear () - and alias
         */
        nonvirtual void reset ();

    public:
        /**
         *  \brief
         *      Set to done and disassociate with owner.
         *
         *  Equivalent to *this = GetEmptyIterator();
         *
         *  @see reset () - and alias
         */
        nonvirtual void clear ();

    public:
        /**
         *  \brief
         *      Used by *someContainer*::end ()
         *
         *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
         *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
         * 
         *  \note this is something like the c++20 ranges sentinel idea, except that we don't use a separate type (perhaps a mistake on my part).
         */
        static constexpr default_sentinel_t GetEmptyIterator () noexcept;

    public:
        /**
         *  \brief
         *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
         *      mainly intended for implementors.
         *
         *  Get a reference to the IRep owned by the iterator.
         *  This is an implementation detail, mainly intended for implementors.
         */
        nonvirtual IRep& GetRep ();

    public:
        /**
         *  \brief
         *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
         *      mainly intended for implementors.
         *
         *  Get a reference to the IRep owned by the iterator.
         *  This is an implementation detail, mainly intended for implementors.
         */
        nonvirtual const IRep& ConstGetRep () const;

    public:
        /*
         *  \brief Refresh the current iterator state based on what is in the underlying IRep
         * 
         *  Useful when you change the rep directly. This should VERY RARELY be needed - just in implementing iterator patching (say during a remove).
         */
        nonvirtual void Refresh ();

    public:
        /**
         *  \brief, does nothing if !qDebug, but if qDebug, checks internal state and asserts in good shape
         */
        nonvirtual void Invariant () const noexcept;

    private:
        unique_ptr<IRep> fRep_;
        optional<T>      fCurrentValue_;

    private:
        static unique_ptr<IRep> Clone_ (const IRep& rep);

    public:
        template <typename SHARED_T>
        using PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1 - use unique_ptr directly")]] = unique_ptr<SHARED_T>;
        template <typename SHARED_T, typename... ARGS_TYPE>
        [[deprecated ("Since Stroika v3.0d1 - make_unique directly")]] static unique_ptr<SHARED_T> MakeSmartPtr (ARGS_TYPE&&... args)
        {
            return make_unique<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
    };

    /**
     *
     *  \brief  Implementation detail for iterator implementors.
     *
     *  IRep is a support class used to implement the @ref Iterator<T> pattern.
     *
     *  \note IRep subclasses are constructed already pointing at the first element.
     *        So a leading call to More (&value, false); can be used to fetch the first value
     *        and value.has_value() will be false if there were no values
     *
     *  Subclassed by concrete container writers.
     * 
     *  \note Design Note:
     *      o   More (optional<T>*, bool advance) API combines operator++ and iterator != end ()
     *      o   The reason it combines the two, is because they TYPICALLY are done together at the same time,
     *          and its a virtual call, so combining the two into a single call will most frequently be a
     *          performance optimization.
     * 
     *  typical uses:
     *
     *          it++ -> More (&ignoredValue, true)
     *          *it -> More (&v, false); return *v;
     *          Done -> More (&v, false); return v.has_value();
     */
    template <typename T, typename ITERATOR_TRAITS>
    class Iterator<T, ITERATOR_TRAITS>::IRep {
    protected:
        IRep () = default;

    public:
        virtual ~IRep () = default;

    public:
        using RepSmartPtr [[deprecated ("Since Stroika v3.0d1 - use unique_ptr<IRep> directly")]] = unique_ptr<IRep>;

    public:
        /**
         * Clone() makes a copy of the state of this iterator, which can separately be tracked with Equals ()
         * and/or More() to get values and move forward through the iteration.
         */
        virtual unique_ptr<IRep> Clone () const = 0;
        /**
         *  More () takes two required arguments - one an optional<T>* result, and the other a flag about whether or
         *  not to advance.
         *
         *  If advance is true, it moves the iterator to the next legal position 
         *
         *  \note (requires not Done() before advancing) - NEW Since Stroika 2.1b14
         *
         *          BEFORE 2.1b14: It WAS legal to call More () with advance true even when already at the end of iteration.
         *          This design choice was made to be multi-threading friendly.
         *
         *  This function returns the current value in result if the iterator is positioned at a valid position,
         *  and sets result to an nullopt if at the end - its 'at end'.
         *
         *  \req result != nullptr
         *
         *  \em Design Note
         *      We chose to use a pointer parameter instead of a return value to avoid extra
         *      initialization/copying. Note that the return value optimization rule doesn't apply
         *      to assignment, but only initialization.
         *
         */
        virtual void More (optional<T>* result, bool advance) = 0;
        /**
         * \brief two iterators must be iterating over the same source, and be up to the same position.
         *
         *  \req rhs != nullptr
         *
         *  \req this and rhs must be of the same dynamic type, and come from the same iterable object
         *
         *  @see Iterator<T>::Equals for details
         */
        virtual bool Equals (const IRep* rhs) const = 0;
#if qDebug
        /**
         */
        virtual void Invariant () const noexcept;
#endif
    };

    /**
     *  \brief More clear way of writing '&*' - convert iterator to pointer.
     *
     *  Sometimes (especially when interacting with low level code) its handy to convert an iterator
     *  to a pointer. This is always legal for a short period (@todo reference to docs/why).
     *
     *  But the idiom is somewhat queer, and wrapping in this method makes it a bit more clear.
     *
     *  \note This returns a const pointer for a const_iterator, and a pointer for a regular (non-cost) iterator.
     */
    template <typename ITERATOR>
    constexpr typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i);

    /**
     *  IInputIterator concept: std::input_iterator and iterated over values convertible to OF_T
     * 
     *  \note this does not require the input iterator is OF T objects, merely that the T objects it iterates over
     *  can be converted to OF_T objects.
     */
    template <typename ITERATOR, typename OF_T>
    concept IInputIterator = input_iterator<ITERATOR> and is_convertible_v<std::iter_value_t<ITERATOR>, OF_T>;
    static_assert (IInputIterator<Iterator<int>, int>);
    static_assert (IInputIterator<Iterator<long int>, int>);
    static_assert (IInputIterator<Iterator<int>, long int>);
    static_assert (not IInputIterator<Iterator<string>, int>);

    // see Satisfies Concepts
    //      @todo would be nice to include these tests generically as part of template declaration, but cannot figure out how
    //      to get that working (probably due to when incomplete types evaluated) --LGP 2024-08-21
    static_assert (input_iterator<Iterator<int>>);
    static_assert (regular<Iterator<int>>);
    static_assert (sentinel_for<default_sentinel_t, Iterator<int>>);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Iterator.inl"

#endif /*_Stroika_Foundation_Traversal_Iterator_h_ */
