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
	DbgTrace (_T ("DLLLoader - loading DLL %s"), dllName);
	RequireNotNil (dllName);
#if		qPlatform_Windows
	ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != NULL);
#else
	fModule = LoadDLL (dllName);
#endif
}

DLLLoader::DLLLoader (const TChar* dllName, const vector<TString>& searchPath)
{
	DbgTrace (_T ("DLLLoader - loading DLL %s (with searchPath)"), dllName);
	RequireNotNil (dllName);
	try {
#if		qPlatform_Windows
		ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != NULL);
#else
		fModule = LoadDLL (dllName);
#endif
	}
	catch (...) {
		for (vector<TString>::const_iterator i = searchPath.begin (); i != searchPath.end (); ++i) {
			TString modulePath = *i + _T ("\\") + dllName;
#if		qPlatform_Windows
			fModule = ::LoadLibrary (modulePath.c_str ());
#else
			IgnoreExceptionsForCall (fModule = LoadDLL (modulePath.c_str ()));
#endif
			if (fModule != NULL) {
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

	if (module == NULL) {
		// either main module or not found
		const char*	err = dlerror ();
		if (err != NULL) {
			throw DLLException (err);
		}
	}
	return module;
}
#endif

DLLLoader::~DLLLoader ()
{
	DbgTrace (_T ("DLLLoader - unloading dll"));
	AssertNotNil (fModule);
#if		qPlatform_Windows
	::FreeLibrary (fModule);
#else
	if (dlclose (fModule) != 0) {
		const char*	err = dlerror ();
		if (err != NULL) {
			throw DLLException (err);
		}
	}
#endif
}

