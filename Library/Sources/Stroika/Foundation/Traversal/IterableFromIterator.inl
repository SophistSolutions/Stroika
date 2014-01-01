/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_inl_
#define _Stroika_Foundation_Traversal_IterableFromIterator_inl_

#include    "../Debug/Assertions.h"
#include    "DelegatedIterator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


#if     qDebug
            namespace  Private_ {
                template    <typename T>
                IteratorTracker<T>::~IteratorTracker ()
                {
                    Assert (*fCountRunning == 0);
                }
                template    <typename T>
                Iterator<T>     IteratorTracker<T>::MakeDelegatedIterator (const Iterator<T>& sourceIterator)
                {
                    return DelegatedIterator<T, shared_ptr<unsigned int>> (sourceIterator, fCountRunning);
                }
            }
#endif


            /*
             ********************************************************************************
             ****** IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep *********
             ********************************************************************************
             */
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            inline  IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::_Rep (const DATA_BLOB& dataBLOB)
                : _fDataBlob (dataBLOB)
#if     qDebug
                , fIteratorTracker_ ()
#endif
            {
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            Iterator<T>   IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::MakeIterator () const
            {
#if     qDebug
                return fIteratorTracker_.MakeDelegatedIterator (Iterator<T> (typename Iterator<T>::SharedIRepPtr (new NEW_ITERATOR_REP_TYPE (_fDataBlob))));
#else
                return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new NEW_ITERATOR_REP_TYPE (_fDataBlob)));
#endif
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            size_t     IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::GetLength () const
            {
                size_t  n = 0;
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    n++;
                }
                return n;
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            bool  IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::IsEmpty () const
            {
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    return false;
                }
                return true;
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            void  IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::Apply (typename _Rep::_APPLY_ARGTYPE doToElement) const
            {
                this->_Apply (doToElement);
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            Iterator<T>   IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::ApplyUntilTrue (typename _Rep::_APPLYUNTIL_ARGTYPE doToElement) const
            {
                return this->_ApplyUntilTrue (doToElement);
            }
#define qNotSureWhyWeNeedExtraTemplateDefsIsItMSFTBugOrMyMisunderstanding   1
#if     qNotSureWhyWeNeedExtraTemplateDefsIsItMSFTBugOrMyMisunderstanding
            template    <typename T>
            size_t     IterableFromIterator<T, void, void>::_Rep::GetLength () const
            {
                size_t  n = 0;
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    n++;
                }
                return n;
            }
            template    <typename T>
            bool  IterableFromIterator<T, void, void>::_Rep::IsEmpty () const
            {
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    return false;
                }
                return true;
            }
            template    <typename T>
            void  IterableFromIterator<T, void, void>::_Rep::Apply (typename _Rep::_APPLY_ARGTYPE doToElement) const
            {
                this->_Apply (doToElement);
            }
            template    <typename T>
            Iterator<T>   IterableFromIterator<T, void, void>::_Rep::ApplyUntilTrue (typename _Rep::_APPLYUNTIL_ARGTYPE doToElement) const
            {
                return this->_ApplyUntilTrue (doToElement);
            }
#endif


            /*
             ********************************************************************************
             **************************** MakeIterableFromIterator **************************
             ********************************************************************************
             */
            template    <typename   T>
            Iterable<T> MakeIterableFromIterator (const Iterator<T>& iterator)
            {
                struct   MyIterable_ : public Iterable<T> {
                    struct   Rep : public IterableFromIterator<T>::_Rep {
                        Iterator<T>   fOriginalIterator;
#if     qDebug
                        mutable Private_::IteratorTracker<T>    fIteratorTracker_;
#endif
                        Rep (const Iterator<T>& originalIterator)
                            : fOriginalIterator (originalIterator)
#if     qDebug
                            , fIteratorTracker_ ()
#endif
                        {
                        }
                        virtual Iterator<T>     MakeIterator () const override
                        {
#if     qDebug
                            return fIteratorTracker_.MakeDelegatedIterator (fOriginalIterator);
#else
                            return fOriginalIterator;
#endif
                        }
                        virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
                        {
                            return typename Iterable<T>::_SharedPtrIRep (new Rep (*this));
                        }
                    };
                    MyIterable_ (const Iterator<T>& originalIterator)
                        : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new Rep (originalIterator)))
                    {
                    }
                };
                return MyIterable_ (iterator);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_IterableFromIterator_inl_ */
