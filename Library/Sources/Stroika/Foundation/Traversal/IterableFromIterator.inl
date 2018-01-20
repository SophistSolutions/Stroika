/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_inl_
#define _Stroika_Foundation_Traversal_IterableFromIterator_inl_

#include "../Debug/Assertions.h"
#include "DelegatedIterator.h"

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

#if qDebug
            namespace Private_ {
                template <typename T>
                IteratorTracker<T>::~IteratorTracker ()
                {
                    Assert (*fCountRunning == 0);
                }
                template <typename T>
                Iterator<T> IteratorTracker<T>::MakeDelegatedIterator (const Iterator<T>& sourceIterator)
                {
                    return DelegatedIterator<T, shared_ptr<unsigned int>> (sourceIterator, fCountRunning);
                }
            }
#endif

            /*
             ********************************************************************************
             * IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep *
             ********************************************************************************
             */
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            inline IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_Rep (const CONTEXT_FOR_EACH_ITERATOR& contextForEachIterator)
                : _fContextForEachIterator (contextForEachIterator)
#if qDebug
                , fIteratorTracker_ ()
#endif
            {
            }
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            Iterator<T> IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::MakeIterator (IteratorOwnerID suggestedOwner) const
            {
#if qDebug
                return fIteratorTracker_.MakeDelegatedIterator (Iterator<T>{Iterator<T>::template MakeSharedPtr<NEW_ITERATOR_REP_TYPE> (_fContextForEachIterator)});
#else
                return Iterator<T>{Iterator<T>::template MakeSharedPtr<NEW_ITERATOR_REP_TYPE> (_fContextForEachIterator)};
#endif
            }
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            size_t IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::GetLength () const
            {
                size_t n = 0;
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    n++;
                }
                return n;
            }
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            bool IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::IsEmpty () const
            {
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    return false;
                }
                return true;
            }
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            void IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::Apply (_APPLY_ARGTYPE doToElement) const
            {
                this->_Apply (doToElement);
            }
            template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
            Iterator<T> IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
            {
                return this->_FindFirstThat (doToElement, suggestedOwner);
            }
#define qNotSureWhyWeNeedExtraTemplateDefsIsItMSFTBugOrMyMisunderstanding 1
#if qNotSureWhyWeNeedExtraTemplateDefsIsItMSFTBugOrMyMisunderstanding
            template <typename T>
            size_t IterableFromIterator<T, void, void>::_Rep::GetLength () const
            {
                size_t n = 0;
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    n++;
                }
                return n;
            }
            template <typename T>
            bool IterableFromIterator<T, void, void>::_Rep::IsEmpty () const
            {
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    return false;
                }
                return true;
            }
            template <typename T>
            void IterableFromIterator<T, void, void>::_Rep::Apply (_APPLY_ARGTYPE doToElement) const
            {
                this->_Apply (doToElement);
            }
            template <typename T>
            Iterator<T> IterableFromIterator<T, void, void>::_Rep::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
            {
                return this->_FindFirstThat (doToElement, suggestedOwner);
            }
#endif

/*
             ********************************************************************************
             **************************** MakeIterableFromIterator **************************
             ********************************************************************************
             */
#if qCompilerAndStdLib_SFINAE_SharedPtr_Buggy
            namespace {
                template <typename T>
                struct MyIterable_ : public Iterable<T> {
                    struct Rep : public IterableFromIterator<T>::_Rep {
                        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
                        DECLARE_USE_BLOCK_ALLOCATION (Rep);
                        Iterator<T> fOriginalIterator;
#if qDebug
                        mutable Private_::IteratorTracker<T> fIteratorTracker_;
#endif
                        Rep (const Iterator<T>& originalIterator)
                            : fOriginalIterator (originalIterator)
#if qDebug
                            , fIteratorTracker_ ()
#endif
                        {
                        }
                        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
#if qDebug
                            return fIteratorTracker_.MakeDelegatedIterator (fOriginalIterator);
#else
                            return fOriginalIterator;
#endif
                        }
                        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                        {
                            return Iterable<T>::template MakeSharedPtr<Rep> (*this);
                        }
                    };
                    MyIterable_ (const Iterator<T>& originalIterator)
                        : Iterable<T> (Iterable<T>::template MakeSharedPtr<Rep> (originalIterator))
                    {
                    }
                };
            }
#endif
            template <typename T>
            Iterable<T> MakeIterableFromIterator (const Iterator<T>& iterator)
            {
#if qCompilerAndStdLib_SFINAE_SharedPtr_Buggy
                return MyIterable_<T> (iterator);
#else
                struct MyIterable_ : public Iterable<T> {
                    struct Rep : public IterableFromIterator<T>::_Rep {
                        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
                        DECLARE_USE_BLOCK_ALLOCATION (Rep);
                        Iterator<T>                          fOriginalIterator;
#if qDebug
                        mutable Private_::IteratorTracker<T> fIteratorTracker_;
#endif
                        Rep (const Iterator<T>& originalIterator)
                            : fOriginalIterator (originalIterator)
#if qDebug
                            , fIteratorTracker_ ()
#endif
                        {
                        }
                        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
#if qDebug
                            return fIteratorTracker_.MakeDelegatedIterator (fOriginalIterator);
#else
                            return fOriginalIterator;
#endif
                        }
                        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                        {
                            return Iterable<T>::template MakeSharedPtr<Rep> (*this);
                        }
                    };
                    MyIterable_ (const Iterator<T>& originalIterator)
                        : Iterable<T> (Iterable<T>::template MakeSharedPtr<Rep> (originalIterator))
                    {
                    }
                };
                return MyIterable_ (iterator);
#endif
            }
        }
    }
}
#endif /* _Stroika_Foundation_Traversal_IterableFromIterator_inl_ */
