/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "String_BufferedStringRep.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete::Private;

/*
 ********************************************************************************
 ********************** BufferedStringRep ::_Rep ********************************
 ********************************************************************************
 */
const wchar_t* BufferedStringRep::_Rep::c_str_peek () const noexcept
{
    [[maybe_unused]] size_t len = _GetLength ();
    Ensure (_fStart[len] == '\0');
    return _fStart;
}
