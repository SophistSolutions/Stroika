/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "String_ExternalMemoryOwnership_StackLifetime.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete;

/*
 ********************************************************************************
 ************** String_ExternalMemoryOwnership_StackLifetime ********************
 ********************************************************************************
 */
String_ExternalMemoryOwnership_StackLifetime::String_ExternalMemoryOwnership_StackLifetime (const wchar_t* cString)
    : String (cString)
{
    /* TODO: FIX PERFORMANCE!!!
     *      This implementation conforms to the requirements of the API, so that this class CAN be used safely. However, it does NOT exhibit the performance
     *  advantages the class description promises.
     *
     *      TODO so - it must do its own rep (similar to String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::MyRep_) - except that it must ALSO have an extra method - FREEZE (or some such).
     *  Then in the DTOR for this envelope, we call FREEZE on that rep - causing it to throw away its unsafe pointer. That must ONLY be done if refcount > 1 (in our DTOR).
     */
}
