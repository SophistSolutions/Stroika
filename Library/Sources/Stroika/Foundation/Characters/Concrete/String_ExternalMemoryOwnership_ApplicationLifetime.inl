/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_inl_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Characters::Concrete {

    /*
     ********************************************************************************
     ************* String_ExternalMemoryOwnership_ApplicationLifetime ***************
     ********************************************************************************
     */
    template <size_t SIZE>
    inline String_ExternalMemoryOwnership_ApplicationLifetime::String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t (&cString)[SIZE])
        : String_ExternalMemoryOwnership_ApplicationLifetime (&cString[0], &cString[SIZE - 1])
    {
    }

}
#endif // _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_inl_
