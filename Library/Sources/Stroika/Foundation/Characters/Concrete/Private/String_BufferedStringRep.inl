/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
         ************************** BufferedStringRep::_Rep *****************************
         ********************************************************************************
         */
        inline size_t BufferedStringRep::_Rep::size () const
        {
            return _GetLength ();
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
            size_t result = Containers::Support::ReserveTweaks::GetScaledUpCapacity (initialCapacity, sizeof (Character));
            Ensure (initialCapacity <= result);
            return result;
        }
        inline pair<wchar_t*, wchar_t*> BufferedStringRep::_Rep::mkBuf_ (size_t length)
        {
            size_t   capacity = AdjustCapacity_ (length + 1); // add one for '\0'
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
            return make_pair (newBuf, newBuf + capacity);
        }
        inline tuple<const wchar_t*, const wchar_t*, size_t> BufferedStringRep::_Rep::mkBuf_ (const TextSpan& t1)
        {
            size_t                   len    = t1.second - t1.first;
            pair<wchar_t*, wchar_t*> result = mkBuf_ (len);
            if (len != 0) {
                (void)::memcpy (result.first, t1.first, len * sizeof (wchar_t));
            }
            result.first[len] = '\0';
            return make_tuple (result.first, result.first + len, result.second - result.first);
        }
        inline tuple<const wchar_t*, const wchar_t*, size_t> BufferedStringRep::_Rep::mkBuf_ (const TextSpan& t1, const TextSpan& t2)
        {
            size_t                   l1     = (t1.second - t1.first);
            size_t                   l2     = (t2.second - t2.first);
            size_t                   len    = l1 + l2;
            pair<wchar_t*, wchar_t*> result = mkBuf_ (len);
            if (l1 != 0) {
                (void)::memcpy (result.first, t1.first, l1 * sizeof (wchar_t));
            }
            if (l2 != 0) {
                (void)::memcpy (result.first + l1, t2.first, l2 * sizeof (wchar_t));
            }
            result.first[len] = '\0';
            return make_tuple (result.first, result.first + len, result.second - result.first);
        }
        inline tuple<const wchar_t*, const wchar_t*, size_t> BufferedStringRep::_Rep::mkBuf_ (const TextSpan& t1, const TextSpan& t2, const TextSpan& t3)
        {
            size_t                   l1     = (t1.second - t1.first);
            size_t                   l2     = (t2.second - t2.first);
            size_t                   l3     = (t3.second - t3.first);
            size_t                   len    = l1 + l2 + l3;
            pair<wchar_t*, wchar_t*> result = mkBuf_ (len);
            if (l1 != 0) {
                (void)::memcpy (result.first, t1.first, l1 * sizeof (wchar_t));
            }
            if (l2 != 0) {
                (void)::memcpy (result.first + l1, t2.first, l2 * sizeof (wchar_t));
            }
            if (l3 != 0) {
                (void)::memcpy (result.first + l1 + l2, t3.first, l3 * sizeof (wchar_t));
            }
            result.first[len] = '\0';
            return make_tuple (result.first, result.first + len, result.second - result.first);
        }
        inline BufferedStringRep::_Rep::_Rep (const tuple<const wchar_t*, const wchar_t*, size_t>& strAndCapacity)
            : inherited{make_pair (get<0> (strAndCapacity), get<1> (strAndCapacity))}
            , fCapacity_{get<2> (strAndCapacity)}
        {
        }
        inline BufferedStringRep::_Rep::_Rep (const TextSpan& t1)
            : _Rep{mkBuf_ (t1)}
        {
        }
        inline BufferedStringRep::_Rep::_Rep (const TextSpan& t1, const TextSpan& t2)
            : _Rep{mkBuf_ (t1, t2)}
        {
        }
        inline BufferedStringRep::_Rep::_Rep (const TextSpan& t1, const TextSpan& t2, const TextSpan& t3)
            : _Rep{mkBuf_ (t1, t2, t3)}
        {
        }
        //theory not tested- but inlined because its important this be fast, it it sb instantiated only in each of the 3 or 4 final DTORs
        inline BufferedStringRep::_Rep::~_Rep ()
        {
            AssertNotNull (_fStart);
            Assert (fCapacity_ == AdjustCapacity_ (this->_GetLength () + 1)); // see mkBuf_ (size_t length) - and possible optimize to not store fCapacity
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
            Assert (fCapacity_ >= kNElts1_);
#endif
            DISABLE_COMPILER_MSC_WARNING_START (4065)
            switch (fCapacity_) {
#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
                case kNElts1_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts1_>*> (const_cast<wchar_t*> (_fStart)), 1);
                } break;
                case kNElts2_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts2_>*> (const_cast<wchar_t*> (_fStart)), 1);
                } break;
                case kNElts3_: {
                    Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts3_>*> (const_cast<wchar_t*> (_fStart)), 1);
                } break;
#endif
                default: {
                    delete[] _fStart;
                } break;
            }
            DISABLE_COMPILER_MSC_WARNING_END (4065)
        }
    }

}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
