/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Demangle.h"
#include "../Execution/Exceptions.h"
#include "../Execution/Thread.h"

#include "ToString.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

#if defined(__GNUC__)
template <>
String Characters::UnoverloadedToString (const long int&); // @todo experiment to see if this works around bug with g++-13 LTO
#endif

/*
 ********************************************************************************
 **************** Characters::ToStringDefaults::ToString ************************
 ********************************************************************************
 */
String Characters::ToStringDefaults::ToString (const exception_ptr& e)
{
    static const String kExceptPefix_{"Exception: "sv};
    static const String kUnknown_{"Unknown Exception"sv};
    if (e == nullptr) {
        return "nullptr"sv;
    }
    try {
        rethrow_exception (e);
    }
    catch (const Execution::ExceptionStringHelper& e) {
        //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
        //return kExceptPefix_ + e.As<String> ();
        return e.As<String> ();
    }
    catch (const exception& e) {
        return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
    }
    catch (...) {
        // fall through
    }
    return kUnknown_;
}

String Characters::ToStringDefaults::ToString (const std::exception& t)
{
    if (auto p = dynamic_cast<const Execution::ExceptionStringHelper*> (&t)) {
        return p->As<String> ();
    }
    return String::FromNarrowSDKString (t.what ());
}

String Characters::ToStringDefaults::ToString (const type_info& t)
{
    //nb: demangle needed for gcc, but not msvc (but harmless there)
    return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
}

String Characters::ToStringDefaults::ToString (const type_index& t)
{
    //nb: demangle needed for gcc, but not msvc (but harmless there)
    return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
}

String Characters::ToStringDefaults::ToString (const thread::id& t)
{
    return String{Execution::Thread::FormatThreadID_A (t)};
}

String Characters::ToStringDefaults::ToString (bool t)
{
    static const String kTrue_{"true"sv};
    static const String kFalse{"false"sv};
    return t ? kTrue_ : kFalse;
}
