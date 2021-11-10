/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../Containers/Support/ReserveTweaks.h"
#include "../../../Debug/Assertions.h"
#include "../../../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Characters::Concrete {
    namespace Private {

#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
        template <size_t SZ>
        struct BufferedStringRepBlock_ {
            static constexpr size_t kNElts = SZ;
            wchar_t                 data[kNElts];
        };
#endif

        /*
         ********************************************************************************
         *********************** BufferedStringRep::_Rep ********************************
         ********************************************************************************
         */
        inline wchar_t* BufferedStringRep::_Rep::_PeekStart ()
        {
            return const_cast<wchar_t*> (_fStart);
        }
        inline size_t BufferedStringRep::_Rep::size () const
        {
            return _GetLength ();
        }
        inline void BufferedStringRep::_Rep::reserve_ (size_t newCapacity)
        {
            Require (newCapacity > 0); // always must be nul-terminated
            Require (newCapacity > _GetLength ());
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
            Require (newCapacity >= kNElts1_ or newCapacity >= kNElts2_ or newCapacity >= kNElts3_);
#endif
            if (fCapacity_ != newCapacity) {
                size_t len = _GetLength ();
                Assert (len <= newCapacity);
                wchar_t* newBuf = nullptr;
                DISABLE_COMPILER_MSC_WARNING_START (4065)
                {
                    switch (newCapacity) {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                        case kNElts1_:
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts1_>) == sizeof (wchar_t) * kNElts1_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.allocate (1));
                            break;
                        case kNElts2_:
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts2_>) == sizeof (wchar_t) * kNElts2_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.allocate (1));
                            break;
                        case kNElts3_:
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts3_>) == sizeof (wchar_t) * kNElts3_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.allocate (1));
                            break;
#endif
                        default:
                            newBuf = new wchar_t[newCapacity];
                    }
                    (void)::memcpy (newBuf, _fStart, (len + 1) * sizeof (wchar_t)); // copy data + nul-term
                }
                switch (fCapacity_) {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                    case kNElts1_:
                        Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts1_>*> (_PeekStart ()), 1);
                        break;
                    case kNElts2_:
                        Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts2_>*> (_PeekStart ()), 1);
                        break;
                    case kNElts3_:
                        Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts3_>*> (_PeekStart ()), 1);
                        break;
#endif
                    default:
                        delete[] _PeekStart ();
                        break;
                }
                DISABLE_COMPILER_MSC_WARNING_END (4065)
                _SetData (newBuf, newBuf + len);
                fCapacity_ = newCapacity;
            }
        }
        inline size_t BufferedStringRep::_Rep::AdjustCapacity_ (size_t initialCapacity)
        {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
            static_assert (kNElts1_ <= kNElts2_ and kNElts2_ <= kNElts3_);
            if (initialCapacity <= kNElts1_) {
                return kNElts1_;
            }
            else if (initialCapacity <= kNElts2_) {
                return kNElts2_;
            }
            else if (initialCapacity <= kNElts3_) {
                return kNElts3_;
            }
#endif
            constexpr size_t kChunkSize_ = 32;
            Ensure (initialCapacity <= Containers::Support::ReserveTweaks::GetScaledUpCapacity (initialCapacity, sizeof (Character), kChunkSize_));
            return Containers::Support::ReserveTweaks::GetScaledUpCapacity (initialCapacity, sizeof (Character), kChunkSize_);
        }
        inline void BufferedStringRep::_Rep::ReserveAtLeast_ (size_t newCapacity)
        {
            size_t len    = _GetLength ();
            size_t newCap = max (newCapacity, len + 1);
            if (newCap > fCapacity_) {
                reserve_ (AdjustCapacity_ (newCap));
            }
        }
        inline BufferedStringRep::_Rep::_Rep (const wchar_t* start, const wchar_t* end, size_t reserveExtraCharacters)
            : inherited{nullptr, nullptr}
        {
            size_t   len      = end - start;
            size_t   capacity = AdjustCapacity_ (len + 1 + reserveExtraCharacters);
            wchar_t* newBuf   = nullptr;
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
            Assert (capacity >= kNElts1_ or capacity >= kNElts2_ or capacity >= kNElts3_);
#endif
            DISABLE_COMPILER_MSC_WARNING_START (4065)
            switch (capacity) {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                case kNElts1_: {
                    static_assert (sizeof (BufferedStringRepBlock_<kNElts1_>) == sizeof (wchar_t) * kNElts1_, "sizes should match");
                    newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.allocate (1));
                } break;
                case kNElts2_: {
                    static_assert (sizeof (BufferedStringRepBlock_<kNElts2_>) == sizeof (wchar_t) * kNElts2_, "sizes should match");
                    newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.allocate (1));
                } break;
                case kNElts3_: {
                    static_assert (sizeof (BufferedStringRepBlock_<kNElts3_>) == sizeof (wchar_t) * kNElts3_, "sizes should match");
                    newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.allocate (1));
                } break;
#endif
                default: {
                    newBuf = new wchar_t[capacity];
                } break;
            }
            DISABLE_COMPILER_MSC_WARNING_END (4065)
            (void)::memcpy (newBuf, start, len * sizeof (wchar_t));
            newBuf[len] = '\0';
            fCapacity_  = capacity;
            _SetData (newBuf, newBuf + len);
        }
        //theory not tested- but inlined because its important this be fast, it it sb instantiated only in each of the 3 or 4 final DTORs
        inline BufferedStringRep::_Rep::~_Rep ()
        {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
            Assert (fCapacity_ >= kNElts1_);
#endif
            DISABLE_COMPILER_MSC_WARNING_START (4065)
            switch (fCapacity_) {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                case kNElts1_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts1_>*> (_PeekStart ()), 1);
                } break;
                case kNElts2_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts2_>*> (_PeekStart ()), 1);
                } break;
                case kNElts3_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts3_>*> (_PeekStart ()), 1);
                } break;
#endif
                default: {
                    delete[] _PeekStart ();
                } break;
            }
            DISABLE_COMPILER_MSC_WARNING_END (4065)
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
