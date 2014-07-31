/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Format_inl_
#define _Stroika_Foundation_Cryptography_Digest_Format_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {

                namespace Private_ {
                    string  mkArrayFmt_ (const uint8_t* start, const uint8_t* end);
                    string  mkFmt_ (unsigned int n);
                    string  mkFmt_ (unsigned long n);
                    string  mkFmt_ (unsigned long long n);

                    using   Characters::String;

                    template    <size_t N>
                    inline  string      Format_ (const array<uint8_t, N>& arr, const string*)
                    {
						static_assert (N >= 1, "N >= 1");
                        return mkArrayFmt_ (&*arr.begin (), &*arr.begin () + N);
                    }
                    inline  string      Format_ (unsigned int n, const string*)
                    {
                        return mkFmt_ (n);
                    }
                    inline  string      Format_ (unsigned long n, const string*)
                    {
                        return mkFmt_ (n);
                    }
                    inline  string      Format_ (unsigned long long n, const string*)
                    {
                        return mkFmt_ (n);
                    }
                    template    <typename DIGEST_RESULT_TYPE>
                    inline  String      Format_ (const DIGEST_RESULT_TYPE& arr, const String*)
                    {
                        return String::FromAscii (Format_ (arr, static_cast<const string*> (nullptr)));
                    }
                }


                template    <typename DIGEST_RESULT_TYPE, typename AS_RESULT_TYPE>
                inline  AS_RESULT_TYPE  Format (const DIGEST_RESULT_TYPE& digestResult)
                {
                    return Private_::Format_ (digestResult, static_cast<const AS_RESULT_TYPE*> (nullptr));
                }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Digest_Format_inl_*/
