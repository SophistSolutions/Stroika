/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_AccessMode_inl_
#define _Stroika_Foundation_IO_AccessMode_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ********************************* IO::AccessMode *******************************
     ********************************************************************************
     */
    constexpr AccessMode operator& (AccessMode l, AccessMode r)
    {
        return AccessMode (static_cast<int> (l) & static_cast<int> (r));
    }
    constexpr AccessMode operator| (AccessMode l, AccessMode r)
    {
        return AccessMode (static_cast<int> (l) | static_cast<int> (r));
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<IO::AccessMode> : EnumNames<IO::AccessMode> {
        static constexpr EnumNames<IO::AccessMode> k{
            EnumNames<IO::AccessMode>::BasicArrayInitializer{
                {
                    {IO::AccessMode::eNoAccess, L"No-Access"},
                    {IO::AccessMode::eRead, L"Read"},
                    {IO::AccessMode::eWrite, L"Write"},
                    {IO::AccessMode::eReadWrite, L"Read-Write"},
                }}};
        constexpr DefaultNames ()
            : EnumNames<IO::AccessMode> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_IO_AccessMode_inl_*/
