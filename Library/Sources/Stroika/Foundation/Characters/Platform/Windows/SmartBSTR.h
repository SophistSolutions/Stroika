/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Platform_Windows_SmartBSTR_h_
#define _Stroika_Foundation_Characters_Platform_Windows_SmartBSTR_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>

#include <OAIdl.h>
#include <wtypes.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"

#include "../../CodePage.h"

/**
 * TODO:
 *
 *  @todo   Cleanup - moving IMPL to INL file, and perhaps adding move CTOR etc...
 *          AND throw in ThrowIfNull calls if allocs fail, and fix Require on const wchar_t* so we require before dereference!
 *
 */

namespace Stroika::Foundation::Characters::Platform::Windows {

    /**
     *  Avoid dependency on CComBSTR since its part of ATL, and MSFT doesn't distribute that in Visual Studio Express.
     *  Avoid _bstr_t since I'm not sure thats portable to other SDK implementations (e.g. Mingw?? not sure)
     */
    class SmartBSTR {
    public:
        SmartBSTR () = default;
        SmartBSTR (nullptr_t) {}
        SmartBSTR (const wchar_t* from)
            : fStr_ (::SysAllocString (from))
        {
            RequireNotNull (from);
        }
        ~SmartBSTR ()
        {
            if (fStr_ != nullptr) {
                ::SysFreeString (fStr_);
            }
        }
        SmartBSTR& operator= (const SmartBSTR& rhs)
        {
            if (fStr_ != nullptr) {
                ::SysFreeString (fStr_);
                fStr_ = NULL;
            }
            if (rhs.fStr_ != nullptr) {
                fStr_ = ::SysAllocString (rhs.fStr_);
            }
            return *this;
        }
        operator BSTR () const noexcept
        {
            return fStr_;
        }
        unsigned int Length () const noexcept
        {
            return ::SysStringLen (fStr_);
        }

    private:
        BSTR fStr_ = nullptr;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "CodePage.inl"

#endif /*_Stroika_Foundation_Characters_Platform_Windows_SmartBSTR_h_*/
