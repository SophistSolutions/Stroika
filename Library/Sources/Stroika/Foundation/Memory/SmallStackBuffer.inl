/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_inl_
#define _Stroika_Foundation_Memory_SmallStackBuffer_inl_    1



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <algorithm>
#include    <cstring>
#include    <type_traits>

#include    "../Debug/Assertions.h"
#include    "Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             ************************* SmallStackBuffer<T, BUF_SIZE> ************************
             ********************************************************************************
             */
            template    <typename   T, size_t BUF_SIZE>
            inline  void    SmallStackBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
            {
                fSize_ = nElements;
                // very rare we'll need to grow past this limit. And we want to keep this routine small so it can be
                // inlined. And put the rare, complex logic in other outofline function
                if (nElements > (NEltsOf (fBuffer_))) {
                    GrowToSize_ (nElements);
                }
                Ensure (GetSize () <= capacity ());
            }
            template    <typename   T, size_t BUF_SIZE>
            void    SmallStackBuffer<T, BUF_SIZE>::GrowToSize_ (size_t nElements)
            {
                Require (nElements > (NEltsOf (fBuffer_)));
                // if we were using buffer, then assume whole thing, and if we malloced, save
                // size in unused buffer
                Assert (sizeof (fBuffer_) >= sizeof (size_t));   // one customer changes the size of the buffer to 1, and wondered why it crashed...
                size_t  oldEltCount =   capacity ();
                if (nElements > oldEltCount) {
                    /*
                    *   If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
                    */
                    nElements = max (nElements, oldEltCount * 2);

                    T*  newPtr = new T [nElements];           // NB: We are careful not to update our size field til this has succeeded (exception safety)

                    // Not totally safe for T with CTOR/DTOR/Op= ... Don't use this class in that case!!!
                    // No idea how many to copy!!! - do worst case(maybe should keep old size if this ever
                    // bus errors???)
                    (void)::memcpy (newPtr, fPointer_, oldEltCount * sizeof (T));
                    if (fPointer_ != fBuffer_) {
                        // we must have used the heap...
                        delete[] fPointer_;
                    }
                    fPointer_ = newPtr;

                    // since we are using the heap, we can store the size in our fBuffer_
                    *(size_t*)&fBuffer_ = nElements;
                }
#if     qDebug
                ValidateGuards_ ();
#endif
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (size_t nElements)
                : fSize_ (0)
                //, fBuffer_ ()
                , fPointer_ (fBuffer_)
            {
#if     qDebug
                memcpy (fGuard1_, kGuard1_, sizeof (kGuard1_));
                memcpy (fGuard2_, kGuard2_, sizeof (kGuard2_));
#endif
#if     !qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy
                static_assert(std::is_trivially_constructible<T>::value, "require T is is_trivially_constructible");
                static_assert(std::is_trivially_destructible<T>::value, "require T is is_trivially_destructible");
                static_assert(std::is_trivially_copyable<T>::value, "require T is is_trivially_copyable");
#endif
                GrowToSize (nElements);
#if     qDebug
                ValidateGuards_ ();
#endif
            }
            template    <typename   T, size_t BUF_SIZE>
            SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from)
                : fSize_ (0)
                //, fBuffer_ (),
                , fPointer_ (fBuffer_)
            {
#if     !qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy
                static_assert(std::is_trivially_constructible<T>::value, "require T is is_trivially_constructible");
                static_assert(std::is_trivially_destructible<T>::value, "require T is is_trivially_destructible");
                static_assert(std::is_trivially_copyable<T>::value, "require T is is_trivially_copyable");
#endif
#if     qDebug
                memcpy (fGuard1_, kGuard1_, sizeof (kGuard1_));
                memcpy (fGuard2_, kGuard2_, sizeof (kGuard2_));
#endif
                GrowToSize (from.fSize_);
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                Memory::Private::VC_BWA_std_copy (from.fPointer_, from.fPointer_ + from.fSize_, fPointer_);
#else
                std::copy (from.fPointer_, from.fPointer_ + from.fSize_, fPointer_);
#endif
#if     qDebug
                ValidateGuards_ ();
#endif
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::~SmallStackBuffer ()
            {
#if     qDebug
                ValidateGuards_ ();
#endif
                if (fPointer_ != fBuffer_) {
                    // we must have used the heap...
                    delete[] fPointer_;
                }
            }
            template    <typename   T, size_t BUF_SIZE>
            SmallStackBuffer<T, BUF_SIZE>&   SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer<T, BUF_SIZE>& rhs)
            {
#if     qDebug
                ValidateGuards_ ();
#endif
                GrowToSize (rhs.fSize_);
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                Memory::Private::VC_BWA_std_copy (rhs.fPointer_, rhs.fPointer_ + rhs.fSize_, fPointer_);
#else
                std::copy (rhs.fPointer_, rhs.fPointer_ + rhs.fSize_, fPointer_);
#endif
#if     qDebug
                ValidateGuards_ ();
#endif
                return *this;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  typename SmallStackBuffer<T, BUF_SIZE>::iterator     SmallStackBuffer<T, BUF_SIZE>::begin ()
            {
                return fPointer_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  typename SmallStackBuffer<T, BUF_SIZE>::iterator     SmallStackBuffer<T, BUF_SIZE>::end ()
            {
                return fPointer_ + fSize_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  typename SmallStackBuffer<T, BUF_SIZE>::const_iterator   SmallStackBuffer<T, BUF_SIZE>::begin () const
            {
                return fPointer_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  typename SmallStackBuffer<T, BUF_SIZE>::const_iterator   SmallStackBuffer<T, BUF_SIZE>::end () const
            {
                return fPointer_ + fSize_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  size_t  SmallStackBuffer<T, BUF_SIZE>::capacity () const
            {
                Assert (NEltsOf(fBuffer_) == BUF_SIZE);
                return (fPointer_ == fBuffer_) ?  NEltsOf (fBuffer_) : *(size_t*)&fBuffer_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  size_t  SmallStackBuffer<T, BUF_SIZE>::GetSize () const
            {
                Ensure (fSize_ <= capacity ());
                return fSize_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  void    SmallStackBuffer<T, BUF_SIZE>::push_back (const T& e)
            {
                size_t  s   =   GetSize ();
                GrowToSize (s + 1);
                fPointer_[s] = e;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::operator T* ()
            {
                AssertNotNull (fPointer_);
                return fPointer_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::operator const T* () const
            {
                AssertNotNull (fPointer_);
                return fPointer_;
            }
#if     qDebug
#if     qCompilerAndStdLib_constexpr_arrays_Buggy
            template    <typename   T, size_t BUF_SIZE>
            const   Byte    SmallStackBuffer<T, BUF_SIZE>::kGuard1_[8]  =   { 0x45, 0x23, 0x12, 0x56, 0x99, 0x76, 0x12, 0x55, };
            template    <typename   T, size_t BUF_SIZE>
            const   Byte    SmallStackBuffer<T, BUF_SIZE>::kGuard2_[8]  =   { 0x15, 0x32, 0xa5, 0x16, 0x15, 0x7a, 0x90, 0x10, };
#else
            template    <typename   T, size_t BUF_SIZE>
            constexpr   Byte    SmallStackBuffer<T, BUF_SIZE>::kGuard1_[8];
            template    <typename   T, size_t BUF_SIZE>
            constexpr   Byte    SmallStackBuffer<T, BUF_SIZE>::kGuard2_[8];
#endif
            template    <typename   T, size_t BUF_SIZE>
            void    SmallStackBuffer<T, BUF_SIZE>::ValidateGuards_ ()
            {
                Assert (memcmp (kGuard1_, fGuard1_, sizeof (kGuard1_)) == 0);
                Assert (memcmp (kGuard2_, fGuard2_, sizeof (kGuard2_)) == 0);
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SmallStackBuffer_inl_*/
