/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_UpdatableIterable_h_
#define _Stroika_Foundation_Containers_UpdatableIterable_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Traversal/Iterable.h"



/*
 *  \file
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;


            /**
             *  EXPERIMENTAL AS OF v2.0a22x
             *
             *  NOT really CORRECT.
             *  @todo FIND BETTER WAY/AND OR AT LEAST REVIEW
             *
             *  Not sure this does the right thing in the presence of simulatinous access by safe readers and writers. Maybe OK
             *  for most, but what about Iterators?
             *
             *  Also - this maybe needlessly costly for String class (which doesnt use WriteUpdater).
             *
             *  Note - without this 'hack'/fix, we get a failure with the blocking queue regest.
             *
             *  RegressionTest10_BlockingQueue_ ();
             *
             *  @todo - I THINK one fix is to move this writelock support OUT of Iterable and into utility
             *          between Iterable and all our containers. Maybe introduce an in-between class? That intoruces
             *          the mutex and the write helper!
             */
#ifndef qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
#define qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_    1
#endif


            /*
             *  @todo DESCRIBE
             *
             *  Basic motivator was to have mutex code in containers and not effect string (qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_)
             *  But also makes some logical sense in that these iterables have this ability (though no apI) to be updated in place while preserving iterators.
             *
             *  THINKOUT AND DOCUMENT BETTER
             */
            template    <typename T>
            class   UpdatableIterable : public Iterable<T> {
            private:
                using inherited = Iterable<T>;

#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            private:
                mutex   fWriteMutex_;
#endif

            protected:
                using _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;

                using SharedByValueRepType_ = typename Iterable<T>::SharedByValueRepType_;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                UpdatableIterable (const UpdatableIterable<T>& from) noexcept;

            public:
                /**
                 *  \brief  move CTOR - clears source
                 */
                explicit UpdatableIterable (UpdatableIterable<T>&& from) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit UpdatableIterable (const _SharedPtrIRep& rep) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit UpdatableIterable (_SharedPtrIRep&& rep) noexcept;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                nonvirtual  UpdatableIterable<T>&    operator= (const UpdatableIterable<T>& rhs);

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-03-18
                 */
                template    <typename REP_SUB_TYPE>
                class   _SafeReadWriteRepAccessor;
            };


            template    <typename T>
            template    <typename REP_SUB_TYPE>
            class   UpdatableIterable<T>::_SafeReadWriteRepAccessor  {
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            private:
                lock_guard<mutex>       fEnvelopeWriteLock_;
#endif
            private:
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                SharedByValueRepType_   fAccessor_;
                UpdatableIterable<T>*   fIterableEnvelope;
#else
                REP_SUB_TYPE& fRef_;
#endif

            public:
                _SafeReadWriteRepAccessor () = delete;
                _SafeReadWriteRepAccessor (const _SafeReadWriteRepAccessor&) = delete;
                _SafeReadWriteRepAccessor (UpdatableIterable<T>* iterableEnvelope);

            public:
                ~_SafeReadWriteRepAccessor ();

            public:
                nonvirtual  _SafeReadWriteRepAccessor& operator= (const _SafeReadWriteRepAccessor&) = delete;

            public:
                nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const;

            public:
                nonvirtual  REP_SUB_TYPE&    _GetWriteableRep ();
            };



        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "UpdatableIterable.inl"

#endif  /*_Stroika_Foundation_Containers_UpdatableIterable_h_ */
