/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Characters/Format.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileAccessException.h"
#include	"../IO/FileSystem/FileUtils.h"

#include	"Exceptions.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;




using	namespace	Characters;
using	namespace	Execution;

using	Debug::TraceContextBumper;













/*
 ********************************************************************************
 ********************** RequiredComponentMissingException ***********************
 ********************************************************************************
 */
const	wchar_t	RequiredComponentMissingException::kJava[]			=	L"Java";
const	wchar_t	RequiredComponentMissingException::kPDFViewer[]		=	L"PDFViewer";
const	wchar_t	RequiredComponentMissingException::kPDFOCXViewer[]	=	L"PDFOCXViewer";
namespace	{
	wstring	mkMsg (const wstring& component)
		{
			wstring	cName	=	component;
			if (cName == RequiredComponentMissingException::kPDFViewer)			{ cName = L"PDF Viewer";		}
			else if (cName == RequiredComponentMissingException::kPDFOCXViewer)	{ cName = L"PDF Viewer (OCX)";	}
			wstring	x	=	Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
			return x;
		}
}
RequiredComponentMissingException::RequiredComponentMissingException (const wstring& component):
	StringException (mkMsg (component)),
	fComponent (component)
{
}








/*
 ********************************************************************************
 **************** RequiredComponentVersionMismatchException *********************
 ********************************************************************************
 */
namespace	{
	wstring	mkMsg (const wstring& component, const wstring& requiredVersion)
		{
			wstring	x	=	Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
			if (not requiredVersion.empty ()) {
				x += Format (L"; version '%s' is required", requiredVersion.c_str ());
			}
			return x;
		}
}
RequiredComponentVersionMismatchException::RequiredComponentVersionMismatchException (const wstring& component, const wstring& requiredVersion):
	StringException (mkMsg (component, requiredVersion))
{
}






/*
 ********************************************************************************
 **************** FeatureNotSupportedInThisVersionException *********************
 ********************************************************************************
 */
FeatureNotSupportedInThisVersionException::FeatureNotSupportedInThisVersionException (const wstring& feature)
	: StringException (Format (L"%s is not supported in this version of HealthFrame: see the documentation (F1) on features in this version", feature.c_str ()))
	, fFeature (feature)
{
}

