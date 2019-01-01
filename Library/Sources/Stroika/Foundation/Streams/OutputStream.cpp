/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdarg>

#include "../Characters/Format.h"
#include "../Characters/String.h"

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
    Write (s.c_str ());
}

template <>
template <>
void OutputStream<Characters::Character>::Ptr::Write (const wchar_t* start, const wchar_t* end) const
{
    static_assert (sizeof (wchar_t) == sizeof (Characters::Character), "This cast assumes the types are the same");
    Write (reinterpret_cast<const Characters::Character*> (start), reinterpret_cast<const Characters::Character*> (end));
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
