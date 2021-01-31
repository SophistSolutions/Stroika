/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************* HTTP::Headers ********************************
     ********************************************************************************
     */
    template <>
    inline Iterable<KeyValuePair<String, String>> Headers::As () const
    {
        return As<Collection<KeyValuePair<String, String>>> ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline strong_ordering Headers::operator<=> (const Headers& rhs) const
    {
        using ExtrasComparer = Collection<KeyValuePair<String, String>>::SequentialThreeWayComparer<>;
#if qCompilerAndStdLib_stdOptionalThreeWayCompare_Buggy
        if (auto cmp = ExtrasComparer{}(fExtraHeaders_, rhs.fExtraHeaders_); cmp != Common::kEqual) {
            return cmp;
        }
        if (auto cmp = Common::ThreeWayCompare (fCacheControl_, rhs.fCacheControl_); cmp != Common::kEqual) {
            return cmp;
        }
        if (auto cmp = Common::ThreeWayCompare (fContentLength_, rhs.fContentLength_); cmp != Common::kEqual) {
            return cmp;
        }
        if (auto cmp = Common::ThreeWayCompare (fContentType_, rhs.fContentType_); cmp != Common::kEqual) {
            return cmp;
        }
        if (auto cmp = Common::ThreeWayCompare (fETag_, rhs.fETag_); cmp != Common::kEqual) {
            return cmp;
        }
        if (auto cmp = Common::ThreeWayCompare (fHost_, rhs.fHost_); cmp != Common::kEqual) {
            return cmp;
        }
        return Common::ThreeWayCompare (fIfNoneMatch_, rhs.fIfNoneMatch_);
#else
        if (auto cmp = ExtrasComparer{}(fExtraHeaders_, rhs.fExtraHeaders_); cmp != 0) {
            return cmp;
        }
        if (auto cmp = fCacheControl_ <=> rhs.fCacheControl_; cmp != 0) {
            return cmp;
        }
        if (auto cmp = fContentLength_ <=> rhs.fContentLength_; cmp != 0) {
            return cmp;
        }
        if (auto cmp = fContentType_ <=> rhs.fContentType_; cmp != 0) {
            return cmp;
        }
        if (auto cmp = fETag_ <=> rhs.fETag_; cmp != 0) {
            return cmp;
        }
        if (auto cmp = fHost_ <=> rhs.fHost_; cmp != 0) {
            return cmp;
        }
        return fIfNoneMatch_ <=> rhs.fIfNoneMatch_;
#endif
    }
    inline bool Headers::operator== (const Headers& rhs) const
    {
        using ExtrasComparer = Collection<KeyValuePair<String, String>>::SequentialEqualsComparer<>;
        return ExtrasComparer{}(fExtraHeaders_, rhs.fExtraHeaders_) and fCacheControl_ == rhs.fCacheControl_ and fContentLength_ == rhs.fContentLength_ and fContentType_ == rhs.fContentType_ and fETag_ == rhs.fETag_ and fHost_ == rhs.fHost_ and fIfNoneMatch_ == rhs.fIfNoneMatch_;
    }
#endif
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const Headers& lhs, const Headers& rhs)
    {
        using ExtrasComparer = Collection<KeyValuePair<String, String>>::SequentialEqualsComparer<>;
        return ExtrasComparer{}(lhs.fExtraHeaders_, rhs.fExtraHeaders_) and lhs.fCacheControl_ == rhs.fCacheControl_ and lhs.fContentLength_ == rhs.fContentLength_ and lhs.fContentType_ == rhs.fContentType_ and lhs.fETag_ == rhs.fETag_ and lhs.fHost_ == rhs.fHost_ and lhs.fIfNoneMatch_ == rhs.fIfNoneMatch_;
    }
    inline bool operator!= (const Headers& lhs, const Headers& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_inl_*/
