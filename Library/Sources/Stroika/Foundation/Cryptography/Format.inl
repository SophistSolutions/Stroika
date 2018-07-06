/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Format_inl_
#define _Stroika_Foundation_Cryptography_Format_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/BLOB.h"

namespace Stroika::Foundation {
    namespace Cryptography {

        namespace Private_ {
            string mkArrayFmt_ (const uint8_t* start, const uint8_t* end);
            string mkFmt_ (unsigned int n);
            string mkFmt_ (unsigned long n);
            string mkFmt_ (unsigned long long n);

            using Characters::String;

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
                return mkArrayFmt_ (b.begin (), b.end ());
            }
            template <typename CRYTO_RESULT_TO_FORMAT_TYPE>
            inline String Format_ (const CRYTO_RESULT_TO_FORMAT_TYPE& arr, const String*)
            {
                return String::FromASCII (Format_ (arr, static_cast<const string*> (nullptr)));
            }
        }

        template <typename AS_RESULT_TYPE, typename CRYTO_RESULT_TO_FORMAT_TYPE>
        inline AS_RESULT_TYPE Format (const CRYTO_RESULT_TO_FORMAT_TYPE& digestResult)
        {
            return Private_::Format_ (digestResult, static_cast<const AS_RESULT_TYPE*> (nullptr));
        }
        template <typename CRYTO_RESULT_TO_FORMAT_TYPE>
        inline string Format (const CRYTO_RESULT_TO_FORMAT_TYPE& digestResult)
        {
            return Format<string, CRYTO_RESULT_TO_FORMAT_TYPE> (digestResult);
        }
    }
}
#endif /*_Stroika_Foundation_Cryptography_Format_inl_*/
