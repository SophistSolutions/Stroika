/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_h_
#define _Stroika_Foundation_Memory_SmallStackBuffer_h_ 1

#include "../StroikaPreComp.h"

#include "InlineBuffer.h"

/// <summary>
///  FILE DEPRECATED SINCE 2.1r4
/// </summary>

namespace Stroika::Foundation::Memory {

    using std::byte;

    class [[deprecated ("Since Stroika 2.1r4, use BufferCommon")]] SmallStackBufferCommon
    {
    public:
        using UninitializedConstructorFlag = Memory::UninitializedConstructorFlag;
    };

    template <typename T, size_t BUF_SIZE = ((4096 / sizeof (T)) == 0 ? 1 : (4096 / sizeof (T)))>
    using SmallStackBuffer [[deprecated ("Since Stroika 2.1r4, use Buffer<T,BUF_SIZE> - or perhaps StackBuffer<> if really on the stack")]] = InlineBuffer<T, BUF_SIZE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_h_*/
