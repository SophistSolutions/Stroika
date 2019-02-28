/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/Format.h"

#include "IModbusService.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Modbus;

/*
 ********************************************************************************
 ************************ Frameworks::Modbus::Exception *************************
 ********************************************************************************
 */
Modbus::Exception::Exception (ExceptionCode exceptionCode)
    : Execution::RuntimeErrorException<> (Characters::Format (L"Modbus Exception: %d", exceptionCode))
    , fExceptionCode (exceptionCode)
{
}
