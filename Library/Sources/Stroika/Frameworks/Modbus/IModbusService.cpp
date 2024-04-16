/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/Format.h"

#include "IModbusService.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Modbus;

/*
 ********************************************************************************
 ************************ Frameworks::Modbus::Exception *************************
 ********************************************************************************
 */
Modbus::Exception::Exception (ExceptionCode exceptionCode)
    : Execution::RuntimeErrorException<>{Characters::Format ("Modbus Exception: {}"_f, static_cast<int> (exceptionCode))}
    , fExceptionCode{exceptionCode}
{
}
