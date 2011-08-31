/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Debug/Assertions.h"
#include	"../Debug/Trace.h"

#include	"Exceptions.h"

#include	"DLLSupport.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;

using	Debug::TraceContextBumper;





/*
 ********************************************************************************
 **************************** DLLSupport::DLLLoader *****************************
 ********************************************************************************
 */
DLLLoader::DLLLoader (const TChar* dllName)
{
	DbgTrace (TSTR ("DLLLoader - loading DLL %s"), dllName);
	RequireNotNull (dllName);
#if		qPlatform_Windows
	ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != nullptr);
#else
	fModule = LoadDLL (dllName);
#endif
}

DLLLoader::DLLLoader (const TChar* dllName, const vector<TString>& searchPath)
{
	DbgTrace (TSTR ("DLLLoader - loading DLL %s (with searchPath)"), dllName);
	RequireNotNull (dllName);
	try {
#if		qPlatform_Windows
		ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != nullptr);
#else
		fModule = LoadDLL (dllName);
#endif
	}
	catch (...) {
		for (vector<TString>::const_iterator i = searchPath.begin (); i != searchPath.end (); ++i) {
			TString modulePath = *i + TSTR ("\\") + dllName;
#if		qPlatform_Windows
			fModule = ::LoadLibrary (modulePath.c_str ());
#else
			IgnoreExceptionsForCall (fModule = LoadDLL (modulePath.c_str ()));
#endif
			if (fModule != nullptr) {
				return;
			}
		}
		Execution::DoReThrow ();
	}
}

#if		!qPlatform_Windows
DLLHandle	DLLLoader::LoadDLL (const TChar* dllName, int flags)
{
#if qTargetPlatformSDKUseswchar_t
	DLLHandle module = dlopen (Characters::WideStringToUTF8 (dllName).c_str (), flags);
#else
	DLLHandle module = dlopen (dllName, flags);
#endif

	if (module == nullptr) {
		// either main module or not found
		const char*	err = dlerror ();
		if (err != nullptr) {
			throw DLLException (err);
		}
	}
	return module;
}
#endif

DLLLoader::~DLLLoader ()
{
	DbgTrace (TSTR ("DLLLoader - unloading dll"));
	AssertNotNull (fModule);
#if		qPlatform_Windows
	::FreeLibrary (fModule);
#else
	if (dlclose (fModule) != 0) {
		const char*	err = dlerror ();
		if (err != nullptr) {
			throw DLLException (err);
		}
	}
#endif
}

