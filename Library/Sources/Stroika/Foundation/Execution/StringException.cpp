/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "StringException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ******************************** StringException *******************************
 ********************************************************************************
 */
#if 0
namespace {
    class UnspecifiedErrorCategory_ : public error_category
    {   // categorize an error
    public:
        virtual const char *name () const noexcept
        {
            return "Unspecified Error Category";
        }
        virtual string message (int _Errval) const
        {
            return "Unspecified Error";
        }
    };
}

const error_code StringException::kDefaultErrorCode_{1, UnspecifiedErrorCategory_ ()}; // for now - later improve to capture string somehow
#endif