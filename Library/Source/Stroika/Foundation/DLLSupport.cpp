/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"StroikaPreComp.h"

#include	"Assertions.h"
#include	"Exceptions.h"
#include	"Trace.h"

#include	"DLLSupport.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;

using	Trace::TraceContextBumper;

using	namespace	DLLSupport;
using	namespace	Exceptions;

using	namespace	Trace;



/*
 ********************************************************************************
 **************************** DLLSupport::DLLLoader *****************************
 ********************************************************************************
 */
DLLLoader::DLLLoader (const TCHAR* dllName)
{
	DbgTrace (_T ("DLLLoader - loading DLL %s"), dllName);
	RequireNotNil (dllName);
	ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != NULL);
}

DLLLoader::DLLLoader (const TCHAR* dllName, const vector<tstring>& searchPath)
{
	DbgTrace (_T ("DLLLoader - loading DLL %s (with searchPath)"), dllName);
	RequireNotNil (dllName);
	try {
		ThrowIfFalseGetLastError ((fModule = ::LoadLibrary (dllName)) != NULL);
	}
	catch (...) {
		for (vector<tstring>::const_iterator i = searchPath.begin (); i != searchPath.end (); ++i) {
			if ((fModule = ::LoadLibrary ((*i + _T ("\\") + dllName).c_str ())) != NULL) {
				return;
			}
		}
		Exceptions::DoReThrow ();
	}
}

DLLLoader::~DLLLoader ()
{
	DbgTrace (_T ("DLLLoader - unloading dll"));
	AssertNotNil (fModule);
	::FreeLibrary (fModule);
}

