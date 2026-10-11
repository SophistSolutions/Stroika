/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    namespace Private_ {
        // the main executable's read-only image, as address ranges [start, end); fKnown false where Stroika cannot tell
        struct ReadOnlyImage_ {
            static constexpr size_t                       kMaxRanges = 16;
            bool                                          fKnown{};
            size_t                                        fCount{};
            array<pair<uintptr_t, uintptr_t>, kMaxRanges> fRanges{};
        };
        const ReadOnlyImage_& GetReadOnlyImage_ (); // examined once, at first use
    }

    /*
     ********************************************************************************
     ****************** Execution::IsAddressReadOnlyAppLifetime *********************
     ********************************************************************************
     */
    inline optional<bool> IsAddressReadOnlyAppLifetime (span<const byte> s)
    {
        // inline, with the image kept by reference: String asks this of every string view it is constructed from
        static const Private_::ReadOnlyImage_& sImage_ = Private_::GetReadOnlyImage_ ();
        if (not sImage_.fKnown) {
            return nullopt;
        }
        if (s.empty ()) {
            return false;
        }
        uintptr_t start = reinterpret_cast<uintptr_t> (s.data ());
        for (size_t i = 0; i < sImage_.fCount; ++i) {
            const auto& [b, e] = sImage_.fRanges[i];
            if (b <= start and start < e and s.size () <= e - start) {
                return true;
            }
        }
        return false;
    }

}
