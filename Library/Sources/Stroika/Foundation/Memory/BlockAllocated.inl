/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_inl_
#define _Stroika_Foundation_Memory_BlockAllocated_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            /*
             ********************************************************************************
             ********************** AutomaticallyBlockAllocated<T> **************************
             ********************************************************************************
             */
            template <typename T>
            inline AutomaticallyBlockAllocated<T>::AutomaticallyBlockAllocated ()
                : fValue_ ()
            {
            }
            template <typename T>
            inline AutomaticallyBlockAllocated<T>::AutomaticallyBlockAllocated (const AutomaticallyBlockAllocated<T>& t)
                : fValue_ (t)
            {
            }
            template <typename T>
            inline AutomaticallyBlockAllocated<T>::AutomaticallyBlockAllocated (const T& t)
                : fValue_ (t)
            {
            }
            template <typename T>
            inline AutomaticallyBlockAllocated<T>::AutomaticallyBlockAllocated (T&& t)
                : fValue_ (std::move (t))
            {
            }
            template <typename T>
            inline const AutomaticallyBlockAllocated<T>& AutomaticallyBlockAllocated<T>::operator= (const AutomaticallyBlockAllocated<T>& t)
            {
                fValue_ = t.fValue_;
                return *this;
            }
            template <typename T>
            inline const AutomaticallyBlockAllocated<T>& AutomaticallyBlockAllocated<T>::operator= (const T& t)
            {
                fValue_ = t;
                return *this;
            }
            template <typename T>
            inline AutomaticallyBlockAllocated<T>::operator T () const
            {
                return fValue_;
            }
            template <typename T>
            inline T* AutomaticallyBlockAllocated<T>::get ()
            {
                return &fValue_;
            }

            /*
             ********************************************************************************
             *************************** ManuallyBlockAllocated<T> **************************
             ********************************************************************************
             */
            template <typename T>
            template <typename... ARGS>
            inline T* ManuallyBlockAllocated<T>::New (ARGS&&... args)
            {
#if qAllowBlockAllocation
                return new (BlockAllocator<T>::Allocate (sizeof (T))) T (forward<ARGS> (args)...);
#else
                return new T (forward<ARGS> (args)...);
#endif
            }
            template <typename T>
            inline void ManuallyBlockAllocated<T>::Delete (T* p) noexcept
            {
#if qAllowBlockAllocation
                if (p != nullptr) {
                    (p)->~T ();
                    BlockAllocator<T>::Deallocate (p);
                }
#else
                delete p;
#endif
            }
        }
    }
}
#endif /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
