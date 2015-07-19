/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_UpdatableIterable_h_
#define _Stroika_Foundation_Containers_UpdatableIterable_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Traversal/Iterable.h"

#if     qPlatform_Windows
#include    "../Execution/Platform/Windows/CriticalSectionMutex.h"
#endif


/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   PROBABLY DEPRECATE THIS FILE/MODULE. No point given new Synchonized<> design - I think...
 *              -- LGP 2015-07-18
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
             *
             *          <<<NOTE - I HTINK ABOVE FIXED BY USE OF SYNCHONZIED>>>
             *
             *      DISABLED THIS FEATURE. USE SYNCHONIZED INSTEAD (externally if updating). Couldn't
             *      Emperically so far didnt need synchonized<> in blcokgin quue code but going to add it now
             *      anyhow cuz logically looks needed.
             *          -- LGP 2015-06-21
             */
#ifndef qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
#define qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_    0
#endif


#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            /**
             *  This appears slightly faster on performance regtests for windows, so go for it...
             *          --LGP 2014-04-19
             */
#ifndef qStroika_Foundation_Containers_UpdatableIterator_UseWinCritSecMutext_
#define qStroika_Foundation_Containers_UpdatableIterator_UseWinCritSecMutext_    qPlatform_Windows
#endif
#endif


            /**
             *
             */
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
#if     qStroika_Foundation_Containers_UpdatableIterator_UseWinCritSecMutext_
            using   UpdatableIterator_Mutex_ = Execution::Platform::Windows::CriticalSectionMutex;
#else
            using   UpdatableIterator_Mutex_ = std::mutex;
#endif
#endif


            /**
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
                UpdatableIterator_Mutex_   fWriteMutex_;
#endif

            protected:
                using _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;

            protected:
                using _SharedByValueRepType = typename Iterable<T>::_SharedByValueRepType;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                UpdatableIterable (const UpdatableIterable<T>& from);

            public:
                /**
                 *  \brief  move CTOR - clears source
                 */
                explicit UpdatableIterable (UpdatableIterable<T>&& from);

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
                nonvirtual  UpdatableIterable<T>&   operator= (const UpdatableIterable<T>& rhs);
                nonvirtual  UpdatableIterable<T>&   operator= (UpdatableIterable<T> && rhs);

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-03-18
                 */
                template    <typename REP_SUB_TYPE>
                class   _SafeReadWriteRepAccessor;
            };


            /**
             *
             */
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            class   UpdatableIterable<T>::_SafeReadWriteRepAccessor  {
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            private:
                lock_guard<UpdatableIterator_Mutex_>       fEnvelopeWriteLock_;
#endif
            private:
                UpdatableIterable<T>*   fIterableEnvelope_;
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                _SharedByValueRepType   fAccessor_;
#else
                REP_SUB_TYPE&           fRef_;
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

            public:
                nonvirtual  void    _UpdateRep (const typename _SharedByValueRepType::shared_ptr_type& sp);
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
