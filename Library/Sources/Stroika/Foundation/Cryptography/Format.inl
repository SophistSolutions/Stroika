/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Format_inl_
#define _Stroika_Foundation_Cryptography_Format_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::Cryptography {

    namespace Private_ {
        string        mkArrayFmt_ (const uint8_t* start, const uint8_t* end);
        inline string mkArrayFmt_ (const byte* start, const byte* end)
        {
            return mkArrayFmt_ (reinterpret_cast<const uint8_t*> (start), reinterpret_cast<const uint8_t*> (end));
        }
        string mkFmt_ (unsigned int n);
        string mkFmt_ (unsigned long n);
        string mkFmt_ (unsigned long long n);

        using Characters::String;

        template <size_t N>
        inline string Format_ (const array<byte, N>& arr, const string*)
        {
            static_assert (N >= 1, "N >= 1");
            return mkArrayFmt_ (Traversal::Iterator2Pointer (arr.begin ()), Traversal::Iterator2Pointer (arr.begin ()) + N);
        }
        template <size_t N>
        inline string Format_ (const array<uint8_t, N>& arr, const string*)
        {
            static_assert (N >= 1, "N >= 1");
            return mkArrayFmt_ (Traversal::Iterator2Pointer (arr.begin ()), Traversal::Iterator2Pointer (arr.begin ()) + N);
        }
        inline string Format_ (unsigned int n, const string*)
        {
            return mkFmt_ (n);
        }
        inline string Format_ (unsigned long n, const string*)
        {
            return mkFmt_ (n);
        }
        inline string Format_ (unsigned long long n, const string*)
        {
            return mkFmt_ (n);
        }
        inline string Format_ (const Memory::BLOB& b, const string*)
        {
            return mkArrayFmt_ (reinterpret_cast<const uint8_t*> (b.begin ()), reinterpret_cast<const uint8_t*> (b.end ()));
        }
        template <typename CRYTO_RESULT_TO_FORMAT_TYPE>
        inline String Format_ (const CRYTO_RESULT_TO_FORMAT_TYPE& arr, const String*)
        {
            return String{Format_ (arr, static_cast<const string*> (nullptr))};
        }

        template <typename CRYTO_RESULT_TO_FORMAT_TYPE>
        Common::GUID Format_ (const CRYTO_RESULT_TO_FORMAT_TYPE& arr, const Common::GUID*)
        {
            string                  tmp = Format_ (arr, static_cast<const string*> (nullptr));
            std::array<uint8_t, 16> data{};
            size_t                  i = 0;
            for (auto c : tmp) {
                data[i] += (uint8_t)c;
                ++i;
                if (i >= 16) {
                    i = 0;
                }
            }
            return Common::GUID{data};
        }

    }

    template <typename AS_RESULT_TYPE, typename CRYTO_RESULT_TO_FORMAT_TYPE>
    inline AS_RESULT_TYPE Format (const CRYTO_RESULT_TO_FORMAT_TYPE& digestResult)
    {
        return Private_::Format_ (digestResult, static_cast<const AS_RESULT_TYPE*> (nullptr));
    }

}

#endif /*_Stroika_Foundation_Cryptography_Format_inl_*/
