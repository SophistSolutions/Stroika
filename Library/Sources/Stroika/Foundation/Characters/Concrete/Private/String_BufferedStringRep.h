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



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


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


                    /**
                     * This is a utility class to implement most of the basic String::_IRep functionality. This implements functions that change the string, but dont GROW it,
                     * since we don't know in general we can (thats left to subtypes)
                     *
                     *  explain queer wrapper class cuz protected
                     */
                    struct  BufferedStringRep : String {
                        struct  _Rep : public ReadWriteRep::_Rep {
                        private:
                            typedef ReadWriteRep::_Rep  inherited;

                        protected:
                            NO_DEFAULT_CONSTRUCTOR(_Rep);
                            NO_COPY_CONSTRUCTOR(_Rep);
                            NO_ASSIGNMENT_OPERATOR(_Rep);

                        public:
                            _Rep (const wchar_t* start, const wchar_t* end);
                            _Rep (const wchar_t* start, const wchar_t* end, size_t reserve);
                            ~_Rep ();

                        public:
                            virtual     void            InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
                            virtual     void            SetLength (size_t newLength) override;
                            virtual     const wchar_t*  c_str_peek () const noexcept override;
                            virtual     const wchar_t*  c_str_change () override;


                            //maybe private below?
                        public:
                            // size() function defined only so we can use Containers::ReserveSpeedTweekAddN() template
                            nonvirtual  size_t  size () const {
                                return _GetLength ();
                            }
                            nonvirtual  size_t  capacity () const {
                                return fCapacity_;
                            }
                            nonvirtual  void    reserve (size_t newCapacity) {
                                if (_GetLength () > newCapacity) {
                                    // ignore and return
                                }
                                // Force capacity to match request... even if 'unwise'
                                if (fCapacity_ != newCapacity) {
                                    size_t      len     =   _GetLength ();
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
                        private:
                            nonvirtual  void    ReserveAtLeast_ (size_t newCapacity) {
                                const   size_t  kChunkSize  =   32;
                                size_t          len         =   _GetLength ();
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
