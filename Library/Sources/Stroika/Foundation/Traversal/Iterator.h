/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_h_
#define _Stroika_Foundation_Traversal_Iterator_h_ 1

#include "../StroikaPreComp.h"

#include <iterator>
#include <memory>

#include "../Configuration/Common.h"

#include "../Memory/Optional.h"
#include "../Memory/SharedByValue.h"
#include "../Memory/SharedPtr.h"

/**
 *
 *  \file
 *
 *  @todo   https://stroika.atlassian.net/browse/STK-446 - AssertExternallySynchronizedLock
 *
 *  @todo   Consider if we want to make the promise currently defined below in Equals()
 *          about iterating two originally equal iterators. The trouble is - this doesn
 *          work with generators!!! -- REVIEW-- LGP 2013-12-30
 *
 *          THIS IS BAD AND MUST BE REWRITEN - NOT WAHT WE WANT - TOO STRONG A PROMISE.
 *
 *  @todo   Consider adding a Refresh() method to iterator. Semantics would be
 *          to recheck the current item, and recheck the done state of the container.
 *
 *          Use case is with map/reduce. Consider one process doing the mapping, and
 *          another reading the output (if we have &param version of map). Then the
 *          reader could check if done, and wait a moment, and check again (wait? how that?)
 *
 *          The key is that with map reduce you have parallel mapping and processing the output.
 *          But if iterating over output, hard (not impossible) to know you are at the end (length).
 *
 *          Restartable iterators would be one way.
 *
 *  @todo   See if we can replace Rep_Cloner_ stuff with lambda[] inline in type declaration? See if that
 *          has any negative impacts on performacnce/size etc (test win/gcc). Really more an issue
 *          for SharedByValue<> template.
 *
 *  @todo   CONSIDER using enabled_shared_from_this on ireps? Document why we chose to or not to
 *          (maybe test space). From preliminary testing (VC++2012) - it appears to just make things
 *          bigger, not smaller. Must test other compilers too, and maybe disassemble.
 *
 *  @todo   Speed tweeks
 *
 *          The Major Design limitaiton of this approach to iterators is that it requires a non-inlinable
 *          function call per iteration (roughly). Basically - if you pass a callback into an iterator rep
 *          then to APPLY that function on each iteration requires a non-inlinable (indirect through pointer)
 *          function call, or if you do the reverse, and directly use the iteraotr so no you can inline
 *          apply the function, you must call a virtual function for each iteration to bump the next pointer.
 *
 *          Fundementally - you have multiple polypmorphism (on representation of container and thing to apply
 *          at each iteration).
 *
 *          Two tricks:
 *              (1)     Paging.
 *                      Read a bunch at a time. This is tricky to implement and doesn't affect overall computational
 *                      complexity (because it reduces number of virtual calls by a constant factor). But if that
 *                      constant factor is big enough - 10-100-1000? - that still could be relevant pragmatically.
 *
 *                      The biggest challenge is preserving the existing stafety and patch semantics generically,
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
 *                      inlined (a big win oftne times).
 */

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

/**
             *  You can configure this to always use shared_ptr using ./configure, but by default
             *  qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr uses whichever implementation is faster.
             *
             *      This defaults to @see qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr
             */
#ifndef qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr
#define qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr
#endif

            /**
             *  An IteratorOwnerID may be any pointer value, or kUnknownIteratorOwnerID.
             *
             *  Though the type is a pointer, its not mean to ever be cast or dereferenced -
             *  just compared (usually for equality, but also < maybe used for things like tree structure).
             *
             *  The motivation for having an iterator owner is to enforce (at least in debug code) the rule that
             *  you can only compare iterators that are derived from the same container. This is also
             *  a requirement of STL (I believe) but I'm not sure ever explicitly stated. Anyhow, even if
             *  I'm wrong about that, its a good idea ;-).
             *
             *  @see kUnknownIteratorOwnerID
             */
            using IteratorOwnerID = const void*;

            /**
             *  This is like the SQL-null - meaning no known owner - not that there is no owner.
             */
            constexpr IteratorOwnerID kUnknownIteratorOwnerID = nullptr;

            /**
             */
            struct IteratorBase {
            public:
/**
                 *      Temporary name/define - for what SharedPtr/shared_ptr impl we are using.
                 *      Experimental, so dont use directly (yet) - til stablized.
                 *          -- LGP 2014-02-23
                 */
#if qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr
                template <typename SHARED_T>
                using SharedPtrImplementationTemplate = Memory::SharedPtr<SHARED_T>;
                template <typename SHARED_T, typename... ARGS_TYPE>
                static Memory::SharedPtr<SHARED_T> MakeSharedPtr (ARGS_TYPE&&... args);
#else
                template <typename SHARED_T>
                using SharedPtrImplementationTemplate = shared_ptr<SHARED_T>;
                template <typename SHARED_T, typename... ARGS_TYPE>
                static shared_ptr<SHARED_T> MakeSharedPtr (ARGS_TYPE&&... args);
#endif

#if qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr
                template <typename SHARED_T>
                using enable_shared_from_this_SharedPtrImplementationTemplate = Memory::enable_shared_from_this<SHARED_T>;
#else
                template <typename SHARED_T>
                using enable_shared_from_this_SharedPtrImplementationTemplate = std::enable_shared_from_this<SHARED_T>;
#endif
            };

            /**
             *  Identical type to std::iterator<> - but duplicated here because std::iterator<> was deprecated in C++17.
             *  We just need a handy way to capture all the defaults/properties for our iterator class.
             */
            template <typename CATEGORY, typename T, typename DIFF = ptrdiff_t, typename POINTER = T*, typename REFERENCE = T&>
            struct DefaultIteratorTraits {
                using iterator_category = CATEGORY;
                using value_type        = T;
                using difference_type   = DIFF;
                using pointer           = POINTER;
                using reference         = REFERENCE;
            };

            /**
             *  \brief
             *      An Iterator<T> is a copyable object which allows traversing the contents of some container.
             *
             *  An Iterator<T> is typically associated with some container (that is being iterated over)
             *  and which allows traversal from start to finish.
             *  (The iterator itself essentially provides a notion of *start* to *finish*).
             *
             *  There need not actually be a 'container' object. Other 'iterators' can be created, so long as
             *  they act like an iterator.
             *
             *  An Iterator<T> is a copyable object which can safely be used to capture (copy) the state of
             *  iteration and continue iterating from that spot. If the underlying container is
             *  modified, the iterator will be automatically updated to logically account for that update.
             *
             *  Iterators are robust in the presence of changes to their underlying container. Adding or
             *  removing items from a container will not invalidate the iteration.
             *
             *  Different kinds of containers can make further guarantees about the behavior of iterators
             *  in the presence of container modifications. For example a SequenceIterator will always
             *  traverse any items added after the current traversal index, and will never traverse items
             *  added with an index before the current traversal index.
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
             *      1.      Stroika iterators continue to work correctly when the underlying
             *              container is modified.
             *
             *      2.      Stroika iterators carry around their 'done' state all in one object.
             *              For compatability with existing C++ idiom, and some C++11 language features
             *              Stroika iterators inherit from std::iterator<> and allow use of end(),
             *              and i != end() to check for if an iterator is done. But internally,
             *              Stroika just checks i.Done(), and so can users of Stroika iterators.
             *
             *      3.      Stroika iterators are not 'random access'. They just go forwards, one step at a
             *              time. In STL, some kinds of iterators act more like pointers where you can do
             *              address arithmatic.
             *              <<<< RETHINK - WE WANT BIDIITERATOR/ETC>>>>
             *
             *      4.      In STL, reverse iterators are a special type, incompatible with regular iterators.
             *              In Stroika, reverse iterators are also created with rbegin(), rend (), but
             *              their type is no differnt than regular iterators.
             *              <<<< NYI >>>>
             *
             * Some Rules about Iterators:
             *
             *      1.      What is Done() cannot be UnDone()
             *              That is, once an iterator is done, it cannot be restarted.
             *
             *      2.      Iterators can be copied. They always refer to the same
             *              place they did before the copy, and the old iterator is unaffected
             *              by iteration in the new (though it can be affected indirectly
             *              thru mutations).
             *
             *      3.      Whether or not you encounter items added after an addition is
             *              undefined (by Iterator<T> but often defined in specifically for
             *              particular container subtypes).
             *
             *      4.      A consequence of the above, is that items added after you are done,
             *              are not encountered.
             *
             *  Interesting Design Notes:
             *
             *      -   We considered a desgin for Current() - where it would dynamically
             *          grab the current value, as opposed to being defined to be frozen/copied
             *          at the time of iteration.
             *
             *          The advantage of the path not taken is that if you iterated several times without
             *          examining the current value, and if the cost of copying was relatively large, this
             *          definition would have worked out better.
             *
             *          However, because I think its far more frequent that the copies are cheap, the
             *          user will want to look at each value, and the cost in terms of thread locking
             *          and probabe virtual function calls, the current approach of freezing / copying
             *          on iteration seemed better.
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
             *          They can be safely transfered across threads, and the underlying things being iterated over
             *          can be safely and transparently read/written from other threads
             *
             *          <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             */
            template <typename T, typename ITERATOR_TRAITS = DefaultIteratorTraits<forward_iterator_tag, T>>
            class Iterator : public IteratorBase {
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
                using IteratorRepSharedPtr = SharedPtrImplementationTemplate<IRep>;

            private:
                struct Rep_Cloner_ {
                    static IteratorRepSharedPtr Copy (const IRep& t);
                };

            public:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored
                 *          by users).
                 */
                using SharedByValueRepType = Memory::SharedByValue<Memory::SharedByValue_Traits<IRep, IteratorRepSharedPtr, Rep_Cloner_>>;

            private:
                /*
                 *  Mostly internal type to select a constructor for the special END iterator.
                 */
                enum ConstructionFlagForceAtEnd_ {
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
                 *  \req RequireNotNull (rep.get ())
                 */
                explicit Iterator (const IteratorRepSharedPtr& rep);
                Iterator (const Iterator& from);
                Iterator () = delete;

            private:
                Iterator (ConstructionFlagForceAtEnd_);

            public:
                /**
                 *  \brief  Iterators are safely copyable, preserving their current position.
                 */
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
                 */
                nonvirtual T operator* () const;

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
                nonvirtual const value_type* operator-> () const;

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
                 */
                nonvirtual Iterator<T>& operator++ ();

                /**
                 *  \brief
                 *      postincrement
                 *
                 *      Advance iterator; support for range-based-for, and STL style iteration in general
                 *      (containers must also support begin, end).
                 *
                 *  This function is identical to @ref operator++();
                 *
                 *  @see operator++()
                 */
                nonvirtual Iterator<T> operator++ (int);

            public:
                /*
                 *  \req operator++ can be called 'i' times (on a copy of this), and the result returned.
                 *
                                 *  \note   dont use unsigned 'i' because that works less well with overloads and ambiguity.
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
                 *      However, this API works beatifully with Iterable<>::FindFirstThat - and perhaps other things that
                 *      return iterators.
                 *
                 *      also, it allows
                 *          Iterator<T> n = ...;
                 *          while (n) {
                 *          }
                 *          not sure thats better than while (not n.Done ())???
                 */
                nonvirtual operator bool () const;

            public:
                /**
                 *  Iterators iterate over something (if not in reality, at least conceptually). This ID is
                 *  used for an iterator to report what its owner is (instance not class), if any.
                 *  Any iterator may report nullptr, meaning that it has no logical owner.
                 *
                 *  @see IteratorOwnerID
                 *
                 *  Once an iterator is created, its owner can never change. Mulitple iterators can share the same owner.
                 *  When an iterator is copied, the copy has the same owner as the source the iterator was copied from.
                 *
                 *  Iterator owners must not be destroyed before the iterators iterating over them
                 *  (this would be a user bug, one which will often be detected by DEBUG stroika builds, but perhaps not,
                 *  and may generate grave disorder in non-DEBUG builds).
                 *
                 *  Iterator owners have no semantics - other than the above, and requirements on @see Equals();
                 *  However, some classes (like Bag<T>, Sequence<T>, Collection<T> etc) may layer on additional
                 *  semantics for these Iterator owners.
                 */
                nonvirtual IteratorOwnerID GetOwner () const;

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
                 *
                 *  @see operator== ().
                 *  @see operator!= ().
                 */
                nonvirtual bool Equals (const Iterator& rhs) const;

            public:
                /**
                 *  \brief
                 *  Returns the value of the current item visited by the Iterator<T>, and is illegal to call if Done()
                 *
                 *  Current() returns the value of the current item visited by the Iterator<T>.
                 *
                 *  Only one things can change the current value of Current():
                 *      o   operator++()
                 *
                 *  Because of this, two subsequent calls to it.Current () *cannot* return different values with no
                 *  intervening calls on the iterator, even if the underlying container changes.
                 *
                 *  So even in multithread scenarios, its always safe to say:
                 *
                 *      if (not it.Done ()) {
                 *          T v = it.Current ();
                 *      }
                 *
                 *  and moreover, even in multithreaded scenarios, where another thread is randomly and rapidly modifying
                 *  a container:
                 *
                 *      if (not it.Done ()) {
                 *          T v1 = it.Current ();
                 *          sleep(1);
                 *          T v2 = it.Current ();
                 *          Assert (v1 == v2);      // they refer to the same value - this COULD fail
                 *                                  // if you defined a queer operator== that had operator==(x,x) return false)
                 *      }
                 *
                 *  The value of Current is undefined (Assertion error) if called when Done().
                 *
                 *  operator*() is a common synonym for Current().
                 *
                 *  @see operator*()
                 *  @see operator++()
                 *
                 */
                nonvirtual T Current () const;

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
                *   Equivilent to *this = GetEmptyIterator();
                *
                *  @see clear () - and alias
                */
                nonvirtual void reset ();

            public:
                /**
                 *  \brief
                 *      Set to done and disassociate with owner.
                 *
                 *  Equivilent to *this = GetEmptyIterator();
                 *
                 *  @see reset () - and alias
                 */
                nonvirtual void clear ();

            public:
                /**
                 *  \brief
                 *      Used by *somecontainer*::end ()
                 *
                 *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
                 *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
                 */
                static Iterator GetEmptyIterator ();

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
                nonvirtual const IRep& GetRep () const;

            private:
                SharedByValueRepType fIterator_;

            private:
                Memory::Optional<T> fCurrent_;

            private:
                static IteratorRepSharedPtr Clone_ (const IRep& rep);
            };

            /**
             *
             *  \brief  Implementation detail for iterator implementors.
             *
             *  IRep is a support class used to implement the @ref Iterator<T> pattern.
             *
             *  Subclassed by front-end container writers.
             *  Most of the work is done in More, which does a lot of work because it is the
             *  only virtual function called during iteration, and will need to lock its
             *  container when doing "safe" iteration. More does the following:
             *  iterate to the next container value if advance is true
             *  (then) copy the current value into current, if current is not null
             *  return true if iteration can continue (not done iterating)
             *
             *  typical uses:
             *
             *          it++ -> More (&ignoredvalue, true)
             *          *it -> More (&v, false); return *v;
             *          Done -> More (&v, false); return v.IsPresent();
             *
             *          (note that for performance and thread safety reasons the iterator envelope
             *           actually passes fCurrent_ into More when implenenting ++it
             */
            template <typename T, typename ITERATOR_TRAITS>
            class Iterator<T, ITERATOR_TRAITS>::IRep {
            protected:
                IRep () = default;

            public:
                virtual ~IRep () = default;

            public:
                using IteratorRepSharedPtr = typename Iterator<T, ITERATOR_TRAITS>::IteratorRepSharedPtr;

            public:
                /**
                 * Clone() makes a copy of the state of this iterator, which can separately be tracked with Equals ()
                 * and/or More() to get values and move forward through the iteration.
                 */
                virtual IteratorRepSharedPtr Clone () const = 0;
                /**
                 *  @see Iterator<T>::GetOwner
                 */
                virtual IteratorOwnerID GetOwner () const = 0;
                /**
                 *  More () takes two required arguments - one an optional result, and the other a flag about whether or
                 *  not to advance.
                 *
                 *  If advance is true, it moves the iterator to the next legal position.
                 *
                 *  It IS legal to call More () with advance true even when already at the end of iteration.
                 *  This design choice was made to be multi-threading friendly.
                 *
                 *  This function returns the current value in result if the iterator is positioned at a valid position,
                 *  and sets result to an empty value if at the end - its 'at end'.
                 *
                 *  \em Design Note
                 *      We chose to use a pointer parameter instead of a return value to avoid extra
                 *      initializaiton/copying. Note that the return value optimization rule doesn't apply
                 *      to assignment, but only initialization.
                 *
                 */
                virtual void More (Memory::Optional<T>* result, bool advance) = 0;
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
            };

            /**
             *  \brief  operator== is a shorthand for lhs.Equals (rhs)
             */
            template <typename T, typename ITERATOR_TRAITS>
            bool operator== (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs);

            /**
             *  \brief  operator== is a shorthand for not lhs.Equals (rhs)
             */
            template <typename T, typename ITERATOR_TRAITS>
            bool operator!= (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs);

            /**
             *  Sometimes (especially when interacting with low level code) its handy to convert an iterator
             *  to a pointer. This is always legal for a short period (@todo reference to docs/why).
             *
             *  But the idiom is somewhat queer, and wrapping in this method makes it a bit more clear.
             */
            template <typename ITERATOR>
            typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i);
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Iterator.inl"

#endif /*_Stroika_Foundation_Traversal_Iterator_h_ */
