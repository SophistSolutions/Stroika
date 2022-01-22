/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <algorithm>
#include <climits>
#include <string>

#include "../../Containers/Common.h"
#include "../../Math/Common.h"
#include "../../Memory/BlockAllocated.h"
#include "../../Memory/Common.h"

#include "Private/String_BufferedStringRep.h"

#include "String_ExternalMemoryOwnership_ApplicationLifetime.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete;

class String_ExternalMemoryOwnership_ApplicationLifetime::MyRep_ : public String::_IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
private:
    using inherited = String::_IRep;

public:
    MyRep_ (const wchar_t* start, const wchar_t* end)
        : inherited{start, end} // don't copy memory - but copy raw pointers! So they MUST BE (externally promised) 'externally owned for the application lifetime and constant' - like c++ string constants
    {
        // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Require (start + ::wcslen (start) == end);
        Require (*end == '\0' and start + ::wcslen (start) <= end);
    }
    virtual _IterableRepSharedPtr Clone () const override
    {
        AssertNotReached (); // Since String reps now immutable, this should never be called
        return nullptr;
    }
    virtual const wchar_t* c_str_peek () const noexcept override
    {
        // This class ALWAYS constructed with String_ExternalMemoryOwnership_ApplicationLifetime and ALWAYS with NUL-terminated string
        // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Assert (_fStart + ::wcslen (_fStart) == _fEnd);
        Assert (*_fEnd == '\0' and _fStart + ::wcslen (_fStart) <= _fEnd);
        return _fStart;
    }
};

/*
 ********************************************************************************
 ************** String_ExternalMemoryOwnership_ApplicationLifetime **************
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime::String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t* start, const wchar_t* end)
    : inherited{MakeSmartPtr<MyRep_> (start, end)}
{
    // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Require (start + ::wcslen (start) == end);
    Require (*end == '\0' and start + ::wcslen (start) <= end);
}
