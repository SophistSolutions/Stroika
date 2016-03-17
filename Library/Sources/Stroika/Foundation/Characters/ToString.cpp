/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

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
        return kExceptPefix_ + e.As<String> ();
    }
    catch (const std::exception& e) {
        return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
    }
    catch (...) {
        // fall through
    }
    return kUnknown_;
}
