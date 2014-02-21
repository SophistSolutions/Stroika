/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_inl_
#define _Stroika_Foundation_Memory_BlockAllocated_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             ******************************* BlockAllocated<T> ******************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  BlockAllocated<T>::BlockAllocated ()
                : fValue_ ()
            {
            }
            template    <typename   T>
            inline  BlockAllocated<T>::BlockAllocated (const BlockAllocated<T>& t)
                : fValue_ (t)
            {
            }
            template    <typename   T>
            inline  BlockAllocated<T>::BlockAllocated (const T& t)
                : fValue_ (t)
            {
            }
            template    <typename   T>
            inline  BlockAllocated<T>::BlockAllocated (T&&  t)
                : fValue_ (std::move (t))
            {
            }
            template    <typename   T>
            inline   const BlockAllocated<T>& BlockAllocated<T>::operator= (const BlockAllocated<T>& t)
            {
                fValue_ = t.fValue_;
                return *this;
            }
            template    <typename   T>
            inline   const BlockAllocated<T>& BlockAllocated<T>::operator= (const T& t)
            {
                fValue_ = t;
                return *this;
            }
            template    <typename   T>
            inline    BlockAllocated<T>::operator T () const
            {
                return fValue_;
            }
            template    <typename   T>
            inline    T* BlockAllocated<T>::get ()
            {
                return &fValue_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
