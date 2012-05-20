/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_inl_
#define _Stroika_Foundation_Memory_SmallStackBuffer_inl_    1



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <string.h>

#include    "../Debug/Assertions.h"
#include    "Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {
            //  class   SmallStackBuffer<T,BUF_SIZE>
            template    <typename   T, size_t BUF_SIZE>
            inline  void    SmallStackBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
            {
                fSize_ = nElements;
                // very rare we'll need to grow past this limit. And we want to keep this routine small so it can be
                // inlined. And put the rare, complex logic in other outofline function
                if (nElements > (NEltsOf (fBuffer_))) {
                    GrowToSize_ (nElements);
                }
            }
            template    <typename   T, size_t BUF_SIZE>
            void    SmallStackBuffer<T, BUF_SIZE>::GrowToSize_ (size_t nElements)
            {
#if     qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
#pragma push
#pragma warn -8008
#pragma warn -8066
#endif
                Require (nElements > (NEltsOf (fBuffer_)));
                // if we were using buffer, then assume whole thing, and if we malloced, save
                // size in unused buffer
                Assert (sizeof (fBuffer_) >= sizeof (size_t));   // one customer changes the size of the buffer to 1, and wondered why it crashed...
                size_t  oldEltCount =   (fPointer_ == fBuffer_) ?
                                        NEltsOf (fBuffer_) :
                                        *(size_t*)&fBuffer_;
#if     qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
#pragma pop
#endif
                if (nElements > oldEltCount) {
                    /*
                    *   If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
                    */
                    nElements = max (nElements, oldEltCount * 2);

#if     qVERYLargeOpNewFailsToFail
                    if (nElements > 512 * 1024 * 1024) {
                        ThrowOutOfMemoryException ();
                    }
#endif

                    T*  newPtr = DEBUG_NEW T [nElements];           // NB: We are careful not to update our size field til this has succeeded (exception safety)

                    // Not totally safe for T with CTOR/DTOR/Op= ... Don't use this class in that case!!!
                    // No idea how many to copy!!! - do worst case(maybe should keep old size if this ever
                    // bus errors???)
                    ::memcpy (newPtr, fPointer_, oldEltCount * sizeof (T));
                    if (fPointer_ != fBuffer_) {
                        // we must have used the heap...
                        delete[] fPointer_;
                    }
                    fPointer_ = newPtr;

                    // since we are using the heap, we can store the size in our fBuffer_
                    *(size_t*)&fBuffer_ = nElements;
                }
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (size_t nElements)
                : fSize_ (0)
                //, fBuffer_ ()
                , fPointer_ (fBuffer_)
            {
                GrowToSize (nElements);
            }
            template    <typename   T, size_t BUF_SIZE>
            SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from)
                : fSize_ (0),
                //, fBuffer_ (),
                , fPointer_ (fBuffer_)
            {
                GrowToSize (from.fSize_);
                std::copy (from.fPointer_, from.fPointer_ + from.fSize_, fPointer_);
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::~SmallStackBuffer ()
            {
                if (fPointer_ != fBuffer_) {
                    // we must have used the heap...
                    delete[] fPointer_;
                }
            }
            template    <typename   T, size_t BUF_SIZE>
            SmallStackBuffer<T, BUF_SIZE>&   SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer<T, BUF_SIZE>& rhs)
            {
                GrowToSize (rhs.fSize_);
#pragma warning (push)
#pragma warning (4 : 4996)      // MSVC unneeded warning about std::copy
                std::copy (rhs.fPointer_, rhs.fPointer_ + rhs.fSize_, fPointer_);
#pragma warning (pop)
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
            inline  size_t  SmallStackBuffer<T, BUF_SIZE>::GetSize () const
            {
                return fSize_;
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  void    SmallStackBuffer<T, BUF_SIZE>::push_back (const T& e)
            {
                size_t  s   =   GetSize ();
                GrowToSize (s + 1);
                fPointer_[s] = e;
            }
#if     qCompilerBuggyOverloadingConstOperators
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::operator T* () const
            {
                AssertNotNull (fPointer_);
                return (const_cast<T*> (fPointer_));
            }
#else
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::operator T* ()
            {
                AssertNotNull (fPointer_);
                return (fPointer_);
            }
            template    <typename   T, size_t BUF_SIZE>
            inline  SmallStackBuffer<T, BUF_SIZE>::operator const T* () const
            {
                AssertNotNull (fPointer_);
                return (fPointer_);
            }
#endif
        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SmallStackBuffer_inl_*/
