/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"
#include    "../../../Memory/BlockAllocated.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


                    // Experimental block allocation scheme for strings. We COULD enahce this to have 2 block sizes - say 16 and 32 characters?
                    // But experiment with this a bit first, and see how it goes...
                    //      -- LGP 2011-12-04
#ifndef qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
#define qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings        qAllowBlockAllocation
#endif


#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                    struct BufferedStringRepBlock_ {
                        DEFINE_CONSTEXPR_CONSTANT (size_t, kNElts, 16);
                        wchar_t data[kNElts];
                    };
#endif


                    /*
                     ********************************************************************************
                     *********************** BufferedStringRep::_Rep ********************************
                     ********************************************************************************
                     */
                    inline size_t  BufferedStringRep::_Rep::size () const
                    {
                        return _GetLength ();
                    }
                    inline  size_t  BufferedStringRep::_Rep::capacity () const
                    {
                        return fCapacity_;
                    }
                    inline  void    BufferedStringRep::_Rep::reserve (size_t newCapacity)
                    {
                        if (_GetLength () > newCapacity) {
                            // ignore and return
                        }
                        // Force capacity to match request... even if 'unwise'
                        if (fCapacity_ != newCapacity) {
                            size_t      len     =   _GetLength ();
                            wchar_t*    newBuf  =   nullptr;
                            if (newCapacity != 0) {
#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                                if (newCapacity <= BufferedStringRepBlock_::kNElts) {
                                    newCapacity = BufferedStringRepBlock_::kNElts;
                                    static_assert (sizeof (BufferedStringRepBlock_) == sizeof (wchar_t) * BufferedStringRepBlock_::kNElts, "sizes should match");
                                    newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocated<BufferedStringRepBlock_>::operator new (sizeof (BufferedStringRepBlock_)));
                                }
#endif
                                if (newBuf == nullptr) {
                                    newBuf = DEBUG_NEW wchar_t [newCapacity];
                                }
                                if (len != 0) {
                                    (void)::memcpy (newBuf, _fStart, len * sizeof (wchar_t));
                                }
                            }
#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                            if (fCapacity_ == BufferedStringRepBlock_::kNElts) {
                                Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (_PeekStart ());
                            }
                            else {
                                delete[] _PeekStart ();
                            }
#else
                            delete[] _PeekStart ();
#endif
                            _SetData (newBuf, newBuf + len);
                            fCapacity_ = newCapacity;
                        }
                    }
                    inline  void    BufferedStringRep::_Rep::ReserveAtLeast_ (size_t newCapacity)
                    {
#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                        // ANY size for kChunkSize would be legal/reasonable, but if we use a size < BufferedStringRepBlock_::kNElts,
                        // then we can never allocate BufferedStringRepBlock_ blocks! Or at least not when we call reserveatleast
                        const   size_t  kChunkSize_  =   (newCapacity < BufferedStringRepBlock_::kNElts) ? BufferedStringRepBlock_::kNElts : 32;
#else
                        const   size_t  kChunkSize_  =   32;
#endif
                        size_t          len         =   _GetLength ();
                        if (newCapacity == 0 and len == 0) {
                            reserve (0);
                        }
                        else {
                            ptrdiff_t   n2Add   =   static_cast<ptrdiff_t> (newCapacity) - static_cast<ptrdiff_t> (len);
                            if (n2Add > 0) {
                                // Could be more efficent inlining the reserve code here - cuz we can avoid the call to length()
                                Containers::ReserveSpeedTweekAddN (*this, static_cast<size_t> (n2Add), kChunkSize_);
                            }
                        }
                    }
                    inline   BufferedStringRep::_Rep::_Rep (const wchar_t* start, const wchar_t* end)
                        : inherited (nullptr, nullptr)
                        , fCapacity_ (0)
                    {
                        size_t  len     =   end - start;
                        ReserveAtLeast_ (len);
                        if (len != 0) {
                            AssertNotNull (_PeekStart ());
                            (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                            _fEnd = _fStart + len;
                        }
                    }
                    inline     BufferedStringRep::_Rep:: _Rep (const wchar_t* start, const wchar_t* end, size_t reserve)
                        : inherited (nullptr, nullptr)
                        , fCapacity_ (0)
                    {
                        size_t  len     =   end - start;
                        ReserveAtLeast_ (max (len, reserve));
                        if (len != 0) {
                            AssertNotNull (_PeekStart ());
                            (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                            _fEnd = _fStart + len;
                        }
                    }
                    //theory not tested- but inlined because its important this be fast, it it sb instantiated only in each of the 3 or 4 final DTORs
                    inline  BufferedStringRep::_Rep::~_Rep ()
                    {
#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                        if (fCapacity_ == BufferedStringRepBlock_::kNElts) {
                            Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (_PeekStart ());
                            return;
                        }
#endif
                        delete[] _PeekStart ();
                    }


                }
            }
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
