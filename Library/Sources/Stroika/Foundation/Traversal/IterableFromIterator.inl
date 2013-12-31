/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_inl_
#define _Stroika_Foundation_Traversal_IterableFromIterator_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             ****** IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep *********
             ********************************************************************************
             */
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            inline  IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::_Rep (const DATA_BLOB& dataBLOB)
                : _fDataBlob (dataBLOB)
            {
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            Iterator<T>   IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::MakeIterator () const
            {
                return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new NEW_ITERATOR_REP_TYPE (_fDataBlob)));
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            size_t     IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::GetLength () const
            {
                size_t  n = 0;
                for (auto i = MakeIterator (); not i.Done (); ++i) {
                    n++;
                }
                return n;
            }
            template    <typename T, typename NEW_ITERATOR_REP_TYPE, typename DATA_BLOB>
            bool  IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, DATA_BLOB>::_Rep::IsEmpty () const
            {
                for (auto i = MakeIterator (); not i.Done (); ++i) {
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


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_IterableFromIterator_inl_ */
