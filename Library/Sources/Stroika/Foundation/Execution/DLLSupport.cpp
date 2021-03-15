/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

#include "Throw.h"
#if qPlatform_Windows
#include "Platform/Windows/Exception.h"
#endif

#include "DLLSupport.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Debug::TraceContextBumper;

/*
 ********************************************************************************
 **************************** DLLSupport::DLLLoader *****************************
 ********************************************************************************
 */
namespace {
#if qPlatform_POSIX
    // CAN RETURN NULL
    DLLHandle LoadDLL_ (const SDKChar* dllName, int flags)
    {
#if qTargetPlatformSDKUseswchar_t
        return ::dlopen (Characters::WideStringToUTF8 (dllName).c_str (), flags);
#else
        return ::dlopen (dllName, flags);
#endif
    }
#endif
#if qPlatform_Windows
    // CAN RETURN NULL
    DLLHandle LoadDLL_ (const SDKChar* dllName)
    {
        return ::LoadLibrary (dllName);
    }
#endif

    void ThrowLoadErr_ ()
    {
        // @todo should do something to record the original DLL name
#if qPlatform_POSIX
        // either main module or not found
        const char* err = dlerror ();
        if (err != nullptr) [[UNLIKELY_ATTR]] {
            Execution::Throw (DLLException{err});
        }
#endif
        Execution::ThrowSystemErrNo ();
    }
}

DLLLoader::DLLLoader (const SDKChar* dllName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (SDKSTR ("DLLLoader - loading DLL %s"), dllName);
#endif
    RequireNotNull (dllName);
#if qPlatform_POSIX
    fModule_ = LoadDLL_ (dllName, RTLD_NOW);
#elif qPlatform_Windows
    fModule_ = LoadDLL_ (dllName);
#endif
    if (fModule_ == nullptr) {
        ThrowLoadErr_ ();
    }
}

DLLLoader::DLLLoader (const SDKChar* dllName, const vector<filesystem::path>& searchPath)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (SDKSTR ("DLLLoader - loading DLL %s (with searchPath)"), dllName);
#endif
    RequireNotNull (dllName);
#if qPlatform_POSIX
    fModule_ = LoadDLL_ (dllName, RTLD_NOW);
#else
    fModule_ = LoadDLL_ (dllName);
#endif
    if (fModule_ == nullptr) {
        for (auto i = searchPath.begin (); i != searchPath.end (); ++i) {
            filesystem::path modulePath = *i / dllName;
#if qPlatform_POSIX
            fModule_ = LoadDLL_ (modulePath.c_str (), RTLD_NOW);
#else
            fModule_ = LoadDLL_ (modulePath.c_str ());
#endif
            if (fModule_ != nullptr) {
                return;
            }
        }
    }
    if (fModule_ == nullptr) {
        ThrowLoadErr_ ();
    }
}

#if qPlatform_POSIX
DLLLoader::DLLLoader (const SDKChar* dllName, int flags)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (SDKSTR ("DLLLoader - loading DLL %s, flags=0x%x"), dllName, flags);
#endif
    RequireNotNull (dllName);
    fModule_ = LoadDLL_ (dllName, RTLD_NOW);
    if (fModule_ == nullptr) {
        ThrowLoadErr_ ();
    }
}

DLLLoader::DLLLoader (const SDKChar* dllName, const vector<filesystem::path>& searchPath, int flags)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (SDKSTR ("DLLLoader/3 - loading DLL %s, flags=0x%x"), dllName, flags);
#endif
#if qPlatform_POSIX
    fModule_ = LoadDLL_ (dllName, flags);
#else
    fModule_ = LoadDLL_ (dllName);
#endif
    if (fModule_ == nullptr) {
        for (auto i = searchPath.begin (); i != searchPath.end (); ++i) {
            filesystem::path modulePath = *i / dllName;
#if qPlatform_POSIX
            fModule_ = LoadDLL_ (modulePath.c_str (), flags);
#else
            fModule_ = LoadDLL_ (modulePath.c_str ());
#endif
            if (fModule_ != nullptr) {
                return;
            }
        }
    }
    if (fModule_ == nullptr) {
        ThrowLoadErr_ ();
    }
}
#endif

#if !qPlatform_Windows
DLLHandle DLLLoader::LoadDLL (const SDKChar* dllName, int flags) /// *** DEPRECATED****
{
#if qTargetPlatformSDKUseswchar_t
    DLLHandle module = ::dlopen (Characters::WideStringToUTF8 (dllName).c_str (), flags);
#else
    DLLHandle module = ::dlopen (dllName, flags);
#endif

    if (module == nullptr) {
        // either main module or not found
        const char* err = ::dlerror ();
        if (err != nullptr) [[UNLIKELY_ATTR]] {
            Execution::Throw (DLLException{err});
        }
    }
    return module;
}
#endif

DLLLoader::~DLLLoader ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("DLLLoader - unloading dll 0x%p", fModule_);
#endif
    AssertNotNull (fModule_);
#if qPlatform_Windows
    ::FreeLibrary (fModule_);
#else
    if (::dlclose (fModule_) != 0) {
        const char* err = ::dlerror ();
        if (err != nullptr) [[UNLIKELY_ATTR]] {
            Execution::Throw (DLLException{err});
        }
    }
#endif
}
