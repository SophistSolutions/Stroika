/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_StackBuffer_h_
#define _Stroika_Foundation_Memory_StackBuffer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "InlineBuffer.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Memory {

    /**
     *  On Windows, there is _chkstk which shows up in alot of profiles. Perhaps something similar for UNIX? Or just kernel does this automatically?
     *  Anyhow - target number we try - for performance reasons - to avoid more than this much in a stack frame.
     * 
     *      https://www.codeguru.com/visual-studio/adventures-with-_chkstk/
     */
    constexpr size_t kStackBuffer_SizeIfLargerStackGuardCalled = qPlatform_Windows ? (sizeof (int) == 4 ? 4 : 8) * 1024 : 16 * 1024;

    /**
     *  \note good to keep this small (around 2k) for Windows, cuz else _chkstack calls end up litering profiles in alot of functions
     *        even if along paths not actually used. COULD optimize those paths with specific value in usages, but seems reasonable to keep
     *        to 2k for now --LGP 2023-09-12
     */
    constexpr size_t kStackBuffer_TargetInlineByteBufferSize = qPlatform_Windows ? 2 * 1024 : 4 * 1024;

    /**
     */
    template <typename T = byte>
    constexpr size_t StackBuffer_DefaultInlineSize ()
    {
        // note must be defined here, not in inl file, due to use as default template argument
        auto r = ((kStackBuffer_TargetInlineByteBufferSize / sizeof (T)) == 0 ? 1 : (kStackBuffer_TargetInlineByteBufferSize / sizeof (T)));
        Ensure (r >= 1);
        return r;
    }

    /**
     *  \brief Store variable sized (BUF_SIZE elements) array on the stack (\see also InlineBuffer<T,BUF_SIZE>), and on heap if it grows if needed
     * 
     *  Typically, StackBuffer<> combines the performance of using a stack buffer (inline array on stack) to store arrays with
     *  the safety and flexability of using the free store (malloc).
     *
     *  \note we used to have separate InlineBuffer and StackBuffer, but they did exactly the same thing. The only difference was
     *        the IDEA that StackBuffer might someday be re-implemented using alloca. I dont think thats plausible any longer, but something akin
     *        to it might be possible, so maintain the API difference for now.
     * 
     *
     */
    template <typename T = byte, size_t BUF_SIZE = StackBuffer_DefaultInlineSize<T> ()>
    using StackBuffer = InlineBuffer<T, BUF_SIZE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Memory_StackBuffer_h_*/
