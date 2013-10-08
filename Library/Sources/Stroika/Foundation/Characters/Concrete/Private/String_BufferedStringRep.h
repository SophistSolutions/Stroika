/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_    1

#include    "../../../StroikaPreComp.h"

#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "String_ReadWriteRep.h"


/**
 *  \file
 *
 *
 * TODO:
 *
 */

////// DOPCUMENT SEMI_PRIVATE


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {

                    /**
                     * This is a utility class to implement most of the basic String::_IRep functionality. This implements functions that change the string, but dont GROW it,
                     * since we don't know in general we can (thats left to subtypes)
                     *
                     *  explain queer wrapper class cuz protected
                     */

                    // Experimental block allocation scheme for strings. We COULD enahce this to have 2 block sizes - say 16 and 32 characters?
                    // But experiment with this a bit first, and see how it goes...
                    //      -- LGP 2011-12-04
#ifndef qUseBlockAllocatedForSmallBufStrings
#define qUseBlockAllocatedForSmallBufStrings        qAllowBlockAllocation
#endif
#if     qUseBlockAllocatedForSmallBufStrings
                    // #define until we have constexpr working
#define kBlockAllocMinSizeInwchars  16
                    struct BufferedStringRepBlock_ {
                        wchar_t data[kBlockAllocMinSizeInwchars];
                    };
#endif

                    struct  BufferedStringRep : String {
                        struct  _Rep : public ReadWriteRep::_Rep {
                        private:
                            typedef ReadWriteRep::_Rep  inherited;
                        public:
                            _Rep (const wchar_t* start, const wchar_t* end)
                                : inherited (nullptr, nullptr)
                                , fCapacity_ (0) {
                                size_t  len     =   end - start;
                                ReserveAtLeast_ (len);
                                if (len != 0) {
                                    AssertNotNull (_PeekStart ());
                                    (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                                    _fEnd = _fStart + len;
                                }
                            }
                            _Rep (const wchar_t* start, const wchar_t* end, size_t reserve)
                                : inherited (nullptr, nullptr)
                                , fCapacity_ (0) {
                                size_t  len     =   end - start;
                                ReserveAtLeast_ (max (len, reserve));
                                if (len != 0) {
                                    AssertNotNull (_PeekStart ());
                                    (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                                    _fEnd = _fStart + len;
                                }
                            }
                            ~_Rep () {
#if     qUseBlockAllocatedForSmallBufStrings
                                if (fCapacity_ == kBlockAllocMinSizeInwchars) {
                                    Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (_PeekStart ());
                                    return;
                                }
#endif
                                delete[] _PeekStart ();
                            }
                            virtual     void    InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override {
                                Require (index >= 0);
                                Require (index <= GetLength ());
                                Require (srcStart < srcEnd);

                                // cannot insert pointer FROM THIS REP! Doing so would be buggy! MAYBE I need to handle this with special case logic - before I do the resize?
                                //      -- LGP 2011-12-04
                                Assert (not (_fStart <= reinterpret_cast<const wchar_t*> (srcStart) and reinterpret_cast<const wchar_t*> (srcStart) <= _fEnd));

                                size_t      origLen     =   GetLength ();
                                size_t      amountToAdd =   (srcEnd - srcStart);
                                SetLength (origLen + amountToAdd);
                                size_t      newLen      =   origLen + amountToAdd;
                                Assert (newLen == GetLength ());
                                wchar_t*    gapStart    =   _PeekStart () + index;
                                // make space for insertion
                                (void)::memmove (gapStart + amountToAdd, gapStart, (origLen - index) * sizeof (wchar_t));
                                // now copy in new characters
                                (void)::memcpy (gapStart, srcStart, amountToAdd * sizeof (wchar_t));
                                Ensure (_fStart <= _fEnd);
                            }
                            virtual     void        SetLength (size_t newLength) override {
                                if (newLength < capacity ()) {
                                    // We COULD shrink the capacity at this point, but not critical right now...
                                }
                                else {
                                    ReserveAtLeast_ (newLength);
                                }
                                _fEnd = _fStart + newLength;    // we don't bother doing anything to added/removed characters
                                Ensure (GetLength () == newLength);
                            }
                            virtual     const wchar_t*  c_str_peek () const noexcept override {
                                size_t  len =   GetLength ();
                                if (len < fCapacity_) {
                                    const_cast<wchar_t*> (_fStart)[len] = '\0';     // Cheaper to always set than to check, and maybe set
                                    return _fStart;
                                }
                                return nullptr;
                            }
                            virtual     const wchar_t*      c_str_change () override {
                                ReserveAtLeast_ (GetLength () + 1);
                                return c_str_peek ();
                            }
                            // size() function defined only so we can use Containers::ReserveSpeedTweekAddN() template
                            nonvirtual  size_t  size () const {
                                return GetLength ();
                            }
                            nonvirtual  size_t  capacity () const {
                                return fCapacity_;
                            }
                            nonvirtual  void    reserve (size_t newCapacity) {
                                if (GetLength () > newCapacity) {
                                    // ignore and return
                                }
                                // Force capacity to match request... even if 'unwise'
                                if (fCapacity_ != newCapacity) {
                                    size_t      len     =   GetLength ();
                                    wchar_t*    newBuf  =   nullptr;
                                    if (newCapacity != 0) {
#if     qUseBlockAllocatedForSmallBufStrings
                                        if (newCapacity <= kBlockAllocMinSizeInwchars) {
                                            newCapacity = kBlockAllocMinSizeInwchars;
                                            static_assert (sizeof (BufferedStringRepBlock_) == sizeof (wchar_t) * kBlockAllocMinSizeInwchars, "sizes should match");
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
#if     qUseBlockAllocatedForSmallBufStrings
                                    if (fCapacity_ == kBlockAllocMinSizeInwchars) {
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
                            nonvirtual  void    ReserveAtLeast_ (size_t newCapacity) {
                                const   size_t  kChunkSize  =   32;
                                size_t          len         =   GetLength ();
                                if (newCapacity == 0 and len == 0) {
                                    reserve (0);
                                }
                                else {
                                    ptrdiff_t   n2Add   =   static_cast<ptrdiff_t> (newCapacity) - static_cast<ptrdiff_t> (len);
                                    if (n2Add > 0) {
                                        // Could be more efficent inlining the reserve code here - cuz we can avoid the call to length()
                                        Containers::ReserveSpeedTweekAddN (*this, static_cast<size_t> (n2Add), kChunkSize);
                                    }
                                }
                            }

                        private:
                            size_t      fCapacity_;
                        };
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String_BufferedStringRep.inl"

#endif  /*_Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_*/
