/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Debug/Demangle.h"
#include    "../Execution/StringException.h"

#include    "String_Constant.h"

#include    "ToString.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;




/*
 ********************************************************************************
 *********************** Private_::ToString_exception_ptr ***********************
 ********************************************************************************
 */
String  Private_::ToString_exception_ptr (const exception_ptr& e)
{
    static  const   String_Constant kExceptPefix_   {  L"Exception: " };
    static  const   String_Constant kUnknown_       {  L"Unknown Exception" };
    try {
        std::rethrow_exception (e);
    }
    catch (const Execution::StringException& e) {
        //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
        //return kExceptPefix_ + e.As<String> ();
        return e.As<String> ();
    }
    catch (const std::exception& e) {
        return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
    }
    catch (...) {
        // fall through
    }
    return kUnknown_;
}


/*
 ********************************************************************************
 ***************************** Private_::ToString_ ******************************
 ********************************************************************************
 */
template    <>
String  Private_::ToString_ (const type_info& t, typename enable_if<has_name<type_info>::value>::type*)
{
    //nb: demangle needed for gcc, but not msvc (but harmless there)
    return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
}

template    <>
String  Private_::ToString_ (const type_index& t, typename enable_if<has_name<type_index>::value>::type*)
{
    //nb: demangle needed for gcc, but not msvc (but harmless there)
    return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
}
