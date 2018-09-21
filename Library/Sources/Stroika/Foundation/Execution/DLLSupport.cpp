/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

#include "Exceptions.h"
#if qPlatform_Windows
#include "Platform/Windows/Exception.h"
#endif

#include "DLLSupport.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Debug::TraceContextBumper;

/*
 ********************************************************************************
 **************************** DLLSupport::DLLLoader *****************************
 ********************************************************************************
 */
DLLLoader::DLLLoader (const SDKChar* dllName)
{
    DbgTrace (SDKSTR ("DLLLoader - loading DLL %s"), dllName);
    RequireNotNull (dllName);
#if qPlatform_Windows
    Execution::Platform::Windows::ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != nullptr);
#else
    fModule = LoadDLL (dllName);
#endif
}

DLLLoader::DLLLoader (const SDKChar* dllName, const vector<SDKString>& searchPath)
{
    DbgTrace (SDKSTR ("DLLLoader - loading DLL %s (with searchPath)"), dllName);
    RequireNotNull (dllName);
    try {
#if qPlatform_Windows
        Execution::Platform::Windows::ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != nullptr);
#else
        fModule = LoadDLL (dllName);
#endif
    }
    catch (...) {
        for (auto i = searchPath.begin (); i != searchPath.end (); ++i) {
            SDKString modulePath = *i + SDKSTR ("\\") + dllName;
#if qPlatform_Windows
            fModule = ::LoadLibrary (modulePath.c_str ());
#else
            IgnoreExceptionsForCall (fModule = LoadDLL (modulePath.c_str ()));
#endif
            if (fModule != nullptr) {
                return;
            }
        }
        Execution::ReThrow ();
    }
}

#if !qPlatform_Windows
DLLHandle DLLLoader::LoadDLL (const SDKChar* dllName, int flags)
{
#if qTargetPlatformSDKUseswchar_t
    DLLHandle module = dlopen (Characters::WideStringToUTF8 (dllName).c_str (), flags);
#else
    DLLHandle module = dlopen (dllName, flags);
#endif

    if (module == nullptr) {
        // either main module or not found
        const char* err = dlerror ();
        if (err != nullptr)
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (DLLException (err));
            }
    }
    return module;
}
#endif

DLLLoader::~DLLLoader ()
{
    DbgTrace (SDKSTR ("DLLLoader - unloading dll"));
    AssertNotNull (fModule);
#if qPlatform_Windows
    ::FreeLibrary (fModule);
#else
    if (dlclose (fModule) != 0) {
        const char* err = dlerror ();
        if (err != nullptr)
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (DLLException (err));
            }
    }
#endif
}
