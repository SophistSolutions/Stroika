/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
     *                      return Iterable<T>::template MakeSmartPtr<Rep> (*this);
     *                  }
     *              };
     *              MyIterable_ (const Iterator<T>& originalIterator)
     *                  : Iterable<T>{Iterable<T>::template MakeSmartPtr<Rep> (originalIterator)}
     *              {
     *              }
     *          };
     *      \endcode
     *
     *      Note _Rep is an abstract class, and you MUST provide your own Clone () method, and often will
     *  want to override to provide a more efficeint IsEmpty () and GetLength () implementation.
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
            [[NO_UNIQUE_ADDRESS_ATTR]]_ContextObjectType _fContextForEachIterator;

#if qDebug
        protected:
            struct _IteratorTracker {
                shared_ptr<unsigned int> fCountRunning = make_shared<unsigned int> (0);
                ~_IteratorTracker ();
                Iterator<T> MakeDelegatedIterator (const Iterator<T>& sourceIterator);
            };

        private:
            mutable _IteratorTracker fIteratorTracker_;
#endif

        public:
            template <typename K1 = CONTEXT_FOR_EACH_ITERATOR, enable_if_t<is_same_v<K1, void>>* = nullptr>
            _Rep ();

        protected:
            template <typename K1 = CONTEXT_FOR_EACH_ITERATOR, enable_if_t<not is_same_v<K1, void>>* = nullptr>
            _Rep (const _ContextObjectType& contextForEachIterator);

        public:
            virtual Iterator<T> MakeIterator () const override;
            virtual size_t      GetLength () const override;
            virtual bool        IsEmpty () const override;
            virtual void        Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override;
            virtual Iterator<T> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override;
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
