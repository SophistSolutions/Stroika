/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessMode_inl_
#define _Stroika_Foundation_IO_FileAccessMode_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ***************************** IO::FileAccessMode *******************************
     ********************************************************************************
     */
    inline FileAccessMode operator& (FileAccessMode l, FileAccessMode r)
    {
        return FileAccessMode (static_cast<int> (l) & static_cast<int> (r));
    }
    inline FileAccessMode operator| (FileAccessMode l, FileAccessMode r)
    {
        return FileAccessMode (static_cast<int> (l) | static_cast<int> (r));
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<IO::FileAccessMode> : EnumNames<IO::FileAccessMode> {
        static constexpr EnumNames<IO::FileAccessMode> k{
            EnumNames<IO::FileAccessMode>::BasicArrayInitializer{
                {
                    {IO::FileAccessMode::eNoAccess, L"No-Access"},
                    {IO::FileAccessMode::eRead, L"Read"},
                    {IO::FileAccessMode::eWrite, L"Write"},
                    {IO::FileAccessMode::eReadWrite, L"Read-Write"},
                }}};
        DefaultNames ()
            : EnumNames<IO::FileAccessMode> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_IO_FileAccessMode_inl_*/
