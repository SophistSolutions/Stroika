/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant::INI {

    /*
     ********************************************************************************
     ********************************** INI::Reader *********************************
     ********************************************************************************
     */
    inline Profile Reader::ReadProfile (const Streams::InputStream::Ptr<byte>& in) const
    {
        return Convert (Read (in));
    }
    inline Profile Reader::ReadProfile (const Streams::InputStream::Ptr<Characters::Character>& in) const
    {
        return Convert (Read (in));
    }
    inline Profile Reader::ReadProfile (const Traversal::Iterable<Characters::Character>& in) const
    {
        return Convert (Read (in));
    }
    inline Profile Reader::ReadProfile (istream& in) const
    {
        return Convert (Read (in));
    }
    inline Profile Reader::ReadProfile (wistream& in) const
    {
        return Convert (Read (in));
    }

}
