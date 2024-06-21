/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Compression {

    /*
     ********************************************************************************
     ************************ DataExchange::Compression::Ptr ************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& s)
        : shared_ptr<IRep>{s}
    {
    }
    inline Ptr::Ptr (shared_ptr<IRep>&& s) noexcept
        : shared_ptr<IRep>{move (s)}
    {
    }
    inline InputStream::Ptr<byte> Ptr::Transform (const InputStream::Ptr<byte>& src)
    {
        RequireNotNull (get ());
        return get ()->Transform (src);
    }
    inline BLOB Ptr::Transform (const BLOB& src)
    {
        return Transform (src.As<InputStream::Ptr<byte>> ()).ReadAll ();
    }
    inline auto Ptr::GetStats () const -> optional<Stats>
    {
        RequireNotNull (get ());
        return get ()->GetStats ();
    }

}
