/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Characters/StringUtils.h"
#include	"../../../Configuration/Common.h"
#include	"../../../Containers/Common.h"
#include	"../../../Debug/Trace.h"
#include	"../../../Time/Realtime.h"

#include	"ResourceAccessor.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Execution::Platform;
using	namespace	Stroika::Foundation::Execution::Platform::Windows;


/*
 ********************************************************************************
 *************************** ResourceAccessor ***********************************
 ********************************************************************************
 */
ResourceAccessor::ResourceAccessor (HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType):
	fDataStart (NULL),
	fDataEnd (NULL)
{
	HRSRC	hres	=	::FindResource (hModule, lpName, lpType);
	if (hres != NULL) {
		HGLOBAL	lglbl	=	::LoadResource (hModule, hres);
		if (lglbl != NULL) {
			const void*	lr	=	::LockResource (lglbl);
			AssertNotNil (lr);
			fDataStart = reinterpret_cast<const Byte*> (lr);
			fDataEnd = fDataStart + ::SizeofResource (hModule, hres);
		}
	}
}
