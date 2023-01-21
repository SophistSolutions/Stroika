/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdarg>

#include "../Characters/Format.h"
#include "../Characters/String.h"
#include "../Characters/UTFConvert.h"

#include "OutputStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ******************** Streams::OutputStream<ELEMENT_TYPE>::Ptr ******************
 ********************************************************************************
 */
template <>
template <>
void OutputStream<Characters::Character>::Ptr::Write (const Characters::String& s) const
{
    // @todo performance tweek so uses Peek
    Memory::StackBuffer<wchar_t> ignored;
    span<const wchar_t>          sp = s.GetData<wchar_t> (&ignored);
    Write (sp.data (), sp.data () + sp.size ());
}

template <>
template <>
void OutputStream<Characters::Character>::Ptr::Write (const wchar_t* start, const wchar_t* end) const
{
    if constexpr (sizeof (wchar_t) == sizeof (Characters::Character)) {
        Write (reinterpret_cast<const Characters::Character*> (start), reinterpret_cast<const Characters::Character*> (end));
    }
    else {
        Memory::StackBuffer<Characters::Character> buf{
            Memory::eUninitialized, Characters::UTFConverter::ComputeTargetBufferSize<Characters::Character> (span{start, end})};
#if qCompilerAndStdLib_spanOfContainer_Buggy
        span<Characters::Character> writeSpan = Characters::UTFConverter::kThe.ConvertSpan (span{start, end}, span{buf.data (), buf.size ()});
#else
        span<Characters::Character> writeSpan = Characters::UTFConverter::kThe.ConvertSpan (span{start, end}, span{buf});
#endif
        Write (writeSpan.data (), writeSpan.data () + writeSpan.size ());
    }
}

namespace Stroika::Foundation::Streams {
    template <>
    template <>
    void OutputStream<Characters::Character>::Ptr::PrintF (const wchar_t* format, ...)
    {
        RequireNotNull (format);
        va_list argsList;
        va_start (argsList, format);
        Write (Characters::FormatV (format, argsList));
        va_end (argsList);
    }
}
