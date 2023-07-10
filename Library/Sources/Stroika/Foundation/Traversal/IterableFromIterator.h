/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_h_
#define _Stroika_Foundation_Traversal_IterableFromIterator_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Memory/SharedByValue.h"

#include "DelegatedIterator.h"
#include "Iterable.h"
#include "Iterator.h"

/**
 *  \file
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Helper class to make it a little easier to wrap an Iterable<> around an Iterator class.
     *
     *  Template Paraemters:
     *      T                           is the type of Iterator/Iterable (thing iterated
     *                                  over). Parameter
     *      CONTEXT_FOR_EACH_ITERATOR   is an optional context object you can provide in the
     *                                  constructor for the iterable, a copy of which is
     *                                  passed to each constructed iterator.
     *      NEW_ITERATOR_REP_TYPE       is the (OPTIONAL) type of the iterator rep to construct
     *                                  in the provided Iterable<T>::MakeIterator() method.
     *                                  Note - if you leave this to the default of void,
     *                                  then your subclass must explicitly override the
     *                                  Iterator<T>::IRep_::MakeIterator() method.
     *
     *  \par Example Usage
     *      \code
     *          template    <typename T>
     *          struct   MyIterable_ : public Iterable<T> {
     *              struct   Rep : public IterableFromIterator<T>::_Rep {
     *                  Iterator<T>   fOriginalIterator;
     *                  Rep (const Iterator<T>& originalIterator)
     *                      : fOriginalIterator{originalIterator}
     *                  {
     *                  }
     *                  virtual Iterator<T>     MakeIterator () const override
     *                  {
     *                      return fOriginalIterator;
     *                  }
     *                  virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
     *                  {
     *                      return Memory::MakeSharedPtr<Rep> (*this);
     *                  }
     *              };
     *              MyIterable_ (const Iterator<T>& originalIterator)
     *                  : Iterable<T>{Memory::MakeSharedPtr<Rep> (originalIterator)}
     *              {
     *              }
     *          };
     *      \endcode
     *
     *      Note _Rep is an abstract class, and you MUST provide your own Clone () method, and often will
     *  want to override to provide a more efficient empty () and size () implementation.
     *
     *  @see MakeIterableFromIterator
     *
     */
    template <typename T, typename NEW_ITERATOR_REP_TYPE = void, typename CONTEXT_FOR_EACH_ITERATOR = void>
    class IterableFromIterator : public Iterable<T> {
    public:
        using value_type = typename Iterable<T>::value_type;

    public:
        class _Rep : public Iterable<T>::_IRep {
        private:
            using inherited = typename Iterable<T>::_IRep;

        protected:
            using _ContextObjectType = conditional_t<is_same_v<CONTEXT_FOR_EACH_ITERATOR, void>, Configuration::Empty, CONTEXT_FOR_EACH_ITERATOR>;

        protected:
            [[no_unique_address]] _ContextObjectType _fContextForEachIterator;

#if qDebug
        protected:
            // @todo clarify, but I think this is just a debugging hack to make sure the underlying itertor
            // lifetime is long enough for any iterators generated from this iterable.
            // But I'm not clear this makes sense anymore after all the recent (--LGP 2021-11-23) iterable threading/lifetime
            // changes. Not sure this really does anything useful, but we can revisit later since it doesn't appear to cause any harm either (besides
            // possibly pointless complexity and a little rare speed in the debug code).
            struct _IteratorTracker {
                shared_ptr<unsigned int> fCountRunning = make_shared<unsigned int> (0);
                ~_IteratorTracker ();
                Iterator<T> MakeDelegatedIterator (const Iterator<T>& sourceIterator);
            };

        private:
            mutable _IteratorTracker fIteratorTracker_;
#endif

        public:
            _Rep ()
                requires (is_same_v<CONTEXT_FOR_EACH_ITERATOR, void>)
            = default;

        protected:
            _Rep (const _ContextObjectType& contextForEachIterator)
                requires (same_as<void, CONTEXT_FOR_EACH_ITERATOR>);

        public:
            virtual Iterator<T> MakeIterator () const override;
        };
    };

    /**
     *  This makes a copy of the given iterator, and wraps it in an iterable. That iterable then makes
     *  additional copies of that (argument) iterator as needed.
     *
     *  \em Important Note
     *      The original Iterator<T> will have lifetime == the constructed Iterable<> - so be careful - that
     *      the thing the Iterator<T> came from has long enough lifetime!
     *
     *  @see IterableFromIterator
     */
    template <typename T>
    Iterable<T> MakeIterableFromIterator (const Iterator<T>& iterator);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "IterableFromIterator.inl"

#endif /*_Stroika_Foundation_Traversal_IterableFromIterator_h_ */
