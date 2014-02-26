/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_h_
#define _Stroika_Foundation_Traversal_Iterable_h_  1

#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"
#include    "../Memory/SharedPtr.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Ordering of parameters to SetEquals() etc templates? Type deduction versus
 *              default parameter?
 *
 *      @todo   REDO DOCS FOR ITERABLE - SO CLEAR ITS ALSO THE BASIS OF "GENERATORS". IT COULD  BE RENAMED
 *              GENERATOR (though dont)
 *
 *      @todo   since Iterator<T> now uses iterator<> traits stuff, so should Iterable<T>?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {



            // experiemnt and see if this helps Mujltiset bug...
            struct  ITERABLEBASE {
            protected:
                /**
                 *      Temporary name/define - for what SharedPtr/shared_ptr impl we are using.
                 *      Experimental, so dont use directly (yet) - til stablized.
                 *          -- LGP 2014-02-23
                 */
                template    <typename SHARED_T>
                using   _USING_SHARED_IMPL_ =   shared_ptr<SHARED_T>;
                //using _USING_SHARED_IMPL_ =   Memory::SharedPtr<SHARED_T>;

                template    <typename SHARED_T>
                using   _USING_SHARED_enable_shared_from_this_IMPL_ =   std::enable_shared_from_this<SHARED_T>;
                //using _USING_SHARED_enable_shared_from_this_IMPL_ =   Memory::enable_shared_from_this<SHARED_T>;
            };


            /**
             *  \brief  Iterable<T> is a base class for containers which easily produce an Iterator<T>
             *          to traverse them.
             *
             *  Iterable<T> is a base class for containers which easily produce an Iterator<T> to traverse them.
             *
             *  The Stroika iterators can be used either directly, or in the STL begin/end style - and this
             *  class supports both styles of usage.
             *
             *  Iterable<T> also supports read-only applicative operations on the contained data.
             *
             *  Iterable<T> is much like idea of 'abstract readonly container', but which only supports an
             *  exceedingly simplistic pattern of access.
             *
             *  *Important Design Note*:
             *      We have no:
             *          nonvirtual  void    _SetRep (SharedIRepPtr rep);
             *
             *      because allowing a _SetRep() method would complicate the efforts of subclasses of Iterable<T>
             *      to assure that the underlying type is of the appropriate subtype.
             *
             *      For example - see Bag_Array<T>::GetRep_().
             *
             *  *Design Note*:
             *      Why does Iterable<T> contain a GetLength () method?
             *
             *          o   It’s always well defined what GetLength() means (what you would get if you called
             *              MakeIterable() and iterated a bunch of times til the end).
             *
             *          o   Its almost always (and trivial) to perform that computation more efficiently than the
             *              iterate over each element apporach.
             *
             *          The gist of these two consideration means that if you need to find the length of
             *          an Iterable<T>, if it was defined as a method, you can access the trivial implemeantion,
             *          and if it was not defined, you would be forced into the costly implementation.
             *
             *      Adding GetLength () adds no conceptual cost – because its already so well and clearly defined
             *      in terms of its basic operation (iteration). And it provides value (maybe just modest value).
             *
             *  *Design Note*:
             *      Order of Iteration.
             *
             *      Iterables<T> provide no promises about the order of iteration. Specific subclasses (like SortedSet<>)
             *      often will make specific guarantees about order of iteration.
             *
             *      We do NOT even promise you will see the same items, or seem them in the same order as you iterate
             *      (so for example, you can have a "RandomSequence<>" subclass from Iterable<> and return a different
             *      sequence of numbers each time you make an iterate and run.
             *
             *  *Design Note*:
             *      Chose NOT to include an Equals(Iterable<T> rhs) const method here, but instead duplicatively in
             *      each subclass, so that it could more easily be implemented efficiently (not a biggie), but more
             *      importantly because it doesnt appear to me to make sense so say that a Stack<T> == Set<T>, even if
             *      their values were the same.
             *
             *      ((REVISION - 2013-12-21 - SEE NEW SETEUQALS/TALLYEQUALS/EXACTEUALS methods below)
             *
             *  *Important Design Note*:
             *      Probably important - for performance??? - that all these methods are const,
             *      so ??? think through - what this implies- but probably soemthing about not
             *      threading stuff and ???
             *
             *  *Important Design Note*:
             *      The Lifetime of Iterator<T> objects created by an Iterable<T> instance must always be less
             *      than the creating Iterable's lifetime.
             *
             *      This may not be enforced by implementations (but generally will be in debug builds). But
             *      it is a rule!
             *
             *      The reason for this is that the underlying memory referenced by the iterator may be going away.
             *      We could avoid this by adding a shared_ptr<> reference count into each iterator, but that
             *      would make iterator objects significantly more expensive, and with little apparent value added.
             *      Similarly for weak_ptr<> references.
             *
             *  *Design Note*:
             *      Rejected idea:
             *          Add overload of FindFirstThat() that takes Iterator<T> as arugument instead of T
             *          so you can delete the item pointed to by T.
             *
             *          This was rejected because it can easily be done directly with iterators, and seems
             *          a queeryly specific problem. I cannot see any patterns where one would want to do this.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   Iterable : ITERABLEBASE {
            public:
                /**
                 *      \brief  ElementType is just a handly copy of the *T* template type which this
                 *              Iterable<T> parameterizes access to.
                 */
                using   ElementType =    T;

            protected:
                class  _IRep;


            protected:
                /**
                 *  For now, just use the std::shared_ptr<> - but we may want to go back to older Stroika
                 *  SharedPtr<> code - which I believe is more performant (but lacks the weak_ptr feature).
                 *
                 *  To support that possability, be sure to refernece _SharedPtrIRep instead of using shared_ptr<>
                 *  directly.
                 */
                using   _SharedPtrIRep  =   _USING_SHARED_IMPL_<_IRep>;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                Iterable (const Iterable<T>& from) noexcept;

            public:
                /**
                 *  \brief  move CTOR - clears source
                 */
                explicit Iterable (Iterable<T>&& from) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit Iterable (const _SharedPtrIRep& rep) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit Iterable (_SharedPtrIRep&& rep) noexcept;

            public:
                ~Iterable ();

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                nonvirtual  Iterable<T>&    operator= (const Iterable<T>& rhs);

            public:
                /**
                 * \brief Create an iterator object which can be used to traverse the 'Iterable'.
                 *
                 * Create an iterator object which can be used to traverse the 'Iterable' - this object -
                 * and visit each element.
                 */
                nonvirtual Iterator<T>      MakeIterator () const;

            public:
                /**
                 * \brief Returns the number of items contained.
                 *
                 * GetLength () returns the number of elements in this 'Iterable' object. Its defined to be
                 * the same number of elements you would visit if you created an iterator (MakeIterator())
                 * and visited all items. In practice, as the actual number might vary as the underlying
                 * iterable could change while being iterated over.
                 *
                 *  Also note that GetLength () can return a ridiculous number - like numeric_limits<size_t>::max () -
                 *  for logically infinite sequences... like a sequence of random numbers.
                 *
                 *  \em Performance:
                 *      The performance of GetLength() may vary wildly. It could be anywhere from O(1) to O(N)
                 *      depending on the underlying type of Iterable<T>.
                 */
                nonvirtual  size_t          GetLength () const;

            public:
                /**
                 * \brief Returns true iff GetLength() == 0
                 *
                 *  \em Performance:
                 *      The performance of IsEmpty() may vary wildly (@see GetLength) but will nearly always be O(1).
                 */
                nonvirtual  bool    IsEmpty () const;

            public:
                /**
                 *  Apply the (template argument) EQUALS_COMPARER to each element in the Iterable<T> and
                 *  return true iff found. This invokes no virtual methods dependent (except MakeIterable or some such)
                 *  and so gains no performance benefits from the organizaiton of the underlying Iterable<T>. This
                 *  is just a short hand for the direct iteration one would trivially code by hand. Still - its
                 *  easier to call Contains() that to code that loop!
                 *
                 *  And note - subclasses (like Containers::Set<T>) will hide this implementation with a more
                 *  efficient one (that does indirect to the backend).
                 *
                 *  \em Performance:
                 *      This algorithm is O(N).
                 *
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (const T& element) const;

            public:
                /**
                 *  \em EXPERIMENTAL API
                 *
                 *  Apply the function funciton to each element, and return true if it applies to any of them.
                 *
                 *  \em Design Note:
                 *      Considered overloading this with Contains() - but decided against it since I feared
                 *      potential confusion/conflicts (esp if we expand arg to ContainsWith() to be templated FUNCTOR instead
                 *      of std::function<>).
                 */
                nonvirtual  _Deprecated_ (bool ContainsWith (const std::function<bool(const T& item)>& doToElement) const, "Obsolete as of Stroika v2.0a19 - use FindFirstThat");

            public:
                /**
                 *  SetEquals () - very inefficiently - but with constant small memory overhead - returns true if
                 *  each element in the each iterable is contained in the other. They lengths CAN be differnt
                 *  and the two Iterables<> be SetEquals().
                 *
                 *  \em Performance:
                 *      This algorithm is O(N) * O(M) where N and M are the length of the two respective iterables.
                 *
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    SetEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 *  MultiSetEquals () - very inefficiently - but with constant small memory overhead - returns true if
                 *  each element in the each iterable is contained in the other. They lengths CAN be differnt
                 *  and the two Iterables<> be SetEquals().
                 *
                 *  \em Performance:
                 *      This algorithm is N^^3
                 *
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    MultiSetEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 *  ExactEquals () - very measures if iteration over the two containers produces identical sequences
                 *  of elements (identical by compare with EQUALS_COMPARER).
                 *
                 *  \em Performance:
                 *      This algorithm is O(N)
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    ExactEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 *
                 *  begin ()/end() are similar to MakeIterator(), except that they allow for iterators to be
                 *  used in an STL-style, which is critical for using C++ ranged iteration.
                 *
                 *  For example:
                 *
                 *      for (Iterator<T> i = c.begin (); i != c.end (); ++i) {
                 *          if (*i = T ()) {
                 *              break;
                 *          }
                 *      }
                 *
                 * OR
                 *      for (T& i : c) {
                 *          if (*i = T ()) {
                 *              break;
                 *          }
                 *      }
                 *
                 */
                nonvirtual  Iterator<T> begin () const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 */
                static Iterator<T> end ();

            public:
                /**
                 *  \brief  Run the argument function (or lambda) on each element of the container.
                 *
                 *  Take the given function argument, and call it for each element of the container. This
                 *  is equivalent to:
                 *
                 *      for (Iterator<T> i = begin (); i != end (); ++i) {
                 *          (doToElement) (*i);
                 *      }
                 *
                 *  However, in threading scenarios, this maybe preferable, since it counts as an atomic
                 *  operation that will happen to each element without other
                 *  threads intervening to modify the container.
                 *
                 *  Also, note that this function does NOT change any elements of the Iterable.
                 *
                 *  \pre    doToElement != nullptr
                 */
                nonvirtual  void    Apply (const function<void(const T& item)>& doToElement) const;
                nonvirtual  void  _Deprecated_ (ApplyStatic (void(*doToElement)(const T& item)) const, "Obsolete as of Stroika v2.0a19 - use Apply");

            public:
                /**
                 *  \brief  Run the argument bool-returning function (or lambda) on each element of the
                 *          container, and return an iterator pointing at the first element found true.
                 *
                 *  Take the given function argument, and call it for each element of the container. This is
                 *  equivalent to:
                 *
                 *      for (Iterator<T> i = begin (); i != end (); ++i) {
                 *          if ((doToElement) (*i)) {
                 *              return it;
                 *          }
                 *      }
                 *      return end();
                 *
                 *  However, in threading scenarios, this maybe preferable, since it counts as an atomic
                 *  operation that will happen to each element without other
                 *  threads intervening to modify the container.
                 *
                 *  This function returns an iteartor pointing to the element that triggered the abrupt loop
                 *  end (for example the element you were searching for?). It returns the special iterator
                 *  end() to indicate no doToElement() functions returned true.
                 *
                 *  Also, note that this function does NOT change any elements of the Iterable.
                 *
                 *  Note that this used to be called 'ContainsWith' - because it can act the same way (due to
                 *  operator bool () method of Iterator<T>).
                 */
                nonvirtual  Iterator<T>    FindFirstThat (const function<bool (const T& item)>& doToElement) const;
                nonvirtual  _Deprecated_ (Iterator<T>   ApplyUntilTrue (const std::function<bool(const T& item)>& doToElement) const, "Obsolete as of Stroika v2.0a19 - use FindFirstThat");
                nonvirtual  _Deprecated_ (Iterator<T>    ApplyUntilTrueStatic (bool (*doToElement) (const T& item)) const, "Obsolete as of Stroika v2.0a19 - use FindFirstThat");

            public:
                /**
                 *  As<CONTAINER_OF_T> () can be used to easily map an iterable to another container
                 *  (for exmaple STL container) which supports begin/end iterator constructor. This is
                 *  really just a shorthand for
                 *      CONTAINER_OF_T (this->begin (), this->end ());
                 *
                 *  Note - this also works with (nearly all) of the Stroika containers as well
                 *  (e.g. Set<T> x; x.As<Sequence<T>> ());
                 *
                 *  \em Design Note:
                 *      We chose NOT to include an overload taking iterators because there was no connection between
                 *      'this' and the used iterators, so you may as well just directly call CONTAINER_OF_T(it1, it2).
                 */
                template    <typename CONTAINER_OF_T>
                nonvirtual  CONTAINER_OF_T    As () const;

            public:
                /**
                 * \brief STL-ish alias for IsEmpty()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  length () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  size () const;

            public:
                /**
                 * \brief STL-ish alias for ElementType
                 */
                using   value_type   =       ElementType;

            protected:
                /**
                 *  @see Memory::SharedByValue_State
                 *
                 *  Don't call this lightly. This is just meant for low level or debugging, and for subclass optimizaitons
                 *  based on the state of the shared common object.
                 */
                nonvirtual  Memory::SharedByValue_State    _GetSharingState () const;

            protected:
                /**
                 *  _GetRep() is overloaded on const this. If you access the non-const overload, it will
                 *  assure you have a unique refernece to the given underlying rep (and with the const overload
                 *  no breakreferences will be called).
                 *
                 *  In either case, this Ensures the returned pointer is non-null and valid.
                 */
                nonvirtual  typename Iterable<T>::_IRep&         _GetRep ();
                nonvirtual  const typename Iterable<T>::_IRep&   _GetRep () const;

            protected:
                /**
                 *  Use this in preference to _GetRep () as visual documentation that the caller is expecting to
                 *  get a constant refererence. This is otherwise equivilent to the const overload of _GetRep()
                 */
                nonvirtual  const typename Iterable<T>::_IRep&   _ConstGetRep () const;

            private:
                struct  Rep_Cloner_ {
                    inline  static  _USING_SHARED_IMPL_<_IRep>  Copy (const _IRep& t, IteratorOwnerID forIterableEnvelope)
                    {
                        return Iterable<T>::Clone_ (t, forIterableEnvelope);
                    }
                };

            private:
                static  _SharedPtrIRep  Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope);

            private:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored by users).
                 */
                using   SharedByValueRepType_   =       Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, _SharedPtrIRep, Rep_Cloner_>>;

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-02-21
                 */
                using _ReadOnlyIterableIRepReference = typename SharedByValueRepType_::ReadOnlyReference;

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-02-21
                 */
                nonvirtual  _ReadOnlyIterableIRepReference   _GetReadOnlyIterableIRepReference () const;

            private:
                SharedByValueRepType_    fRep_;
            };


            /**
             *  \brief  Implementation detail for iterator implementors.
             *
             *  Abstract class used in subclasses which extend the idea of Iterable.
             *  Most abstract Containers in Stroika subclass of Iterable<T>.
             */
            template    <typename T>
            class   Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            protected:
                using   _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;

            public:
                using   _APPLY_ARGTYPE      =   const function<void(const T& item)>& ;
                using   _APPLYUNTIL_ARGTYPE =   const function<bool(const T& item)>& ;

            public:
                virtual _SharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const              =   0;
                /*
                 *  NB - the suggestedOwnerID argument to MakeIterator() may be used, or ignored by particular subtypes
                 *  of iterator/iterable. There is no gaurantee about the resulting GetOwner() result from the
                 *  iterator returned.
                 *
                 *  \em Design Note
                 *      It might have been better design to make the argument to Iterable<T>::Rep::MakeIterator ()
                 *      be owner instead of suggestedOwner, and then require that it get tracked. But that would
                 *      have imposed a memory (and copying) overhead on each iterator, and the current
                 *      use cases for iterators dont warrant that.
                 *
                 *      I think its good enough that particular subtypes - where tracking an owner makes sense and
                 *      is useful, we be done. And when not useful, it can be optimized away.
                 */
                virtual Iterator<T>         MakeIterator (IteratorOwnerID suggestedOwner) const                         =   0;
                virtual size_t              GetLength () const                                                          =   0;
                virtual bool                IsEmpty () const                                                            =   0;
                virtual void                Apply (_APPLY_ARGTYPE doToElement) const                                    =   0;
                virtual Iterator<T>         FindFirstThat (_APPLYUNTIL_ARGTYPE, IteratorOwnerID suggestedOwner) const   =   0;

            protected:
                /*
                 * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
                 * actual subclasses.
                 */
                nonvirtual  bool        _IsEmpty () const;
                nonvirtual  void        _Apply (_APPLY_ARGTYPE doToElement) const;
                nonvirtual  Iterator<T> _FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Iterable.inl"

#endif  /*_Stroika_Foundation_Traversal_Iterable_h_ */
