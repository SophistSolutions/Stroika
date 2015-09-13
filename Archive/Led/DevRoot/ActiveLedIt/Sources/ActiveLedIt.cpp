/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Sources/ActiveLedIt.cpp,v 2.23 2004/02/09 16:25:10 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedIt.cpp,v $
 *	Revision 2.23  2004/02/09 16:25:10  lewis
 *	SPR#1599: Added qKeepListOfALInstancesForSPR_1599BWA  for MSIE quit without closing last control bug. For SPR#1641: added assert to check MODULE::m_nObjectCount = 0. Had  to be much more careful about using OLE_GetXXX functions that return IDispatch*. Instead - do non-OLE_ versions that return CComPtr<IDispatch> - so we don't get redundant refcounts.
 *	
 *	Revision 2.22  2004/02/03 00:05:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.21  2004/01/23 23:11:21  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based context
 *	menus. Tons of new OLE APIs. Can create context menus from scratch, or built from building blocks
 *	of existing submenus (like font menu).
 *	
 *	Revision 2.20  2003/12/17 01:23:56  lewis
 *	SPR#1599: tiny bit of bug dection code added to ActiveLedItApp::ExitInstance (). Must come up
 *	with a REAL fix for this
 *	
 *	Revision 2.19  2003/06/03 14:42:13  lewis
 *	SPR#1521: added 'SelListStyle' support. Somewhat related - also changed build of ActiveLedIt
 *	to include the _h.h file generated from the IDL for access to enums rather than redefining
 *	them in the ActiveLedItControl sources.
 *	
 *	Revision 2.18  2003/03/26 21:58:13  lewis
 *	SPR#1379 - DllUnregisterServer fixed to undo category changes before rest of unregisters
 *	(mirroring in reverse order what we do on a REGISTER) - cuz otherwise they fail in the
 *	unregister (presumably cuz some key they depend on is already gone)
 *	
 *	Revision 2.17  2002/09/04 01:59:33  lewis
 *	Only #include <atlimpl.cpp> if  _MSC_VER < _MSVC70_VER_, as its not part of ATL anymore
 *	
 *	Revision 2.16  2002/05/06 21:34:28  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.15  2001/11/27 00:32:36  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.14  2001/09/18 14:03:35  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 2.13  2001/08/30 01:11:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2001/08/20 22:23:01  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text).
 *	Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 2.10  2001/04/11 22:55:23  lewis
 *	SPR#0861- DEMO MODE annoyance dialog. Keep track in registry of how long demo been going on.
 *	Bring up annoyance dialog after a while - when demo expired. And add note in about box saying
 *	how long left in demo
 *	
 *	Revision 2.9  2000/10/16 22:50:21  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.8  1999/12/09 03:16:52  lewis
 *	spr#0645 - -D_UNICODE support
 *	
 *	Revision 2.7  1999/06/23 22:07:11  lewis
 *	cosmetic cleanup
 *	
 *	Revision 2.6  1999/06/16 14:47:36  lewis
 *	spr#0589- implement component categories stuff(http://msdn.microsoft.com/workshop/components/com/
 *	IObjectSafetyExtensions.asp) - so we don't get annoying warnings from MSIE
 *	
 *	Revision 2.5  1998/03/04 20:44:54  lewis
 *	Major cleanups and bugfixes, but mostly fixing/addition of OLEAUT support.
 *	
 *	Revision 2.4  1997/12/24  04:44:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/12/24  04:31:13  lewis
 *	Initialize Picts for unsupprotedmacformatpict, unknowntypestylemarker, and urlpict.
 *	Include <altimpl.cpp> (so I can use ATL).
 *
 *	Revision 2.2  1997/07/27  16:00:23  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/23  16:58:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:25:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 */

#include	<afxctl.h>
#include	<comcat.h>
#include	<objsafe.h>

#include	"StyledTextEmbeddedObjects.h"

#include	"ActiveLedItControl.h"
#include	"Resource.h"
#include	"UserConfigCommands.h"

#include	"ActiveLedIt.h"

#include	"ActiveLedIt_i.c"


#ifdef _DEBUG
	#define new DEBUG_NEW
#endif




inline	const	void*	LoadDLLResource (long resID, LPCTSTR resType)
	{
		HRSRC	hrsrc	=	::FindResource (::AfxGetResourceHandle (), MAKEINTRESOURCE (resID), resType);
		Led_AssertNotNil (hrsrc);
		HGLOBAL	hglobal	=	::LoadResource (::AfxGetResourceHandle (), hrsrc);
		const void*	lockedData	=	::LockResource (hglobal);
		Led_EnsureNotNil (lockedData);
		return (lockedData);
	}




//	Support for registering in the ComponentCategoryManager - that we are a SAFE control, to avoid nasty warnings
//	when running embedded in IE
static	HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
    {
		ICatRegister* pcr = NULL ;
		HRESULT hr = S_OK ;

		hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
		if (FAILED(hr))
			return hr;

		// Make sure the HKCR\Component Categories\{..catid...}
		// key is registered.
		CATEGORYINFO catinfo;
		catinfo.catid = catid;
		catinfo.lcid = 0x0409 ; // english

		// Make sure the provided description is not too long.
		// Only copy the first 127 characters if it is.
		int len = wcslen(catDescription);
		if (len>127)
			len = 127;
		wcsncpy(catinfo.szDescription, catDescription, len);
		// Make sure the description is null terminated.
		catinfo.szDescription[len] = '\0';

		hr = pcr->RegisterCategories(1, &catinfo);
		pcr->Release();

		return hr;
    }
static	HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
    {
		// Register your component categories information.
		ICatRegister* pcr = NULL ;
		HRESULT hr = S_OK ;
		hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
		if (SUCCEEDED(hr)) {
			// Register this category as being "implemented" by the class.
			CATID rgcatid[1] ;
			rgcatid[0] = catid;
			hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
		}

		if (pcr != NULL)
			pcr->Release();

		return hr;
	}
static	HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
    {
		ICatRegister* pcr = NULL ;
		HRESULT hr = S_OK ;

		hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
		if (SUCCEEDED(hr))
		{
			// Unregister this category as being "implemented" by the class.
			CATID rgcatid[1] ;
			rgcatid[0] = catid;
			hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
		}
		if (pcr != NULL)
			pcr->Release();
		return hr;
     }






/*
 ********************************************************************************
 *********************************** ATL Support ********************************
 ********************************************************************************
 */
#include	<atlbase.h>
#if		_MSC_VER < _MSVC70_VER_
#include	<atlimpl.cpp>
#endif




namespace	{
	class	ActiveLedItModule : public CAtlMfcModule /*CAtlDllModuleT <ActiveLedItModule>*/ {
		public:
			DECLARE_LIBID (LIBID_ACTIVELEDITLib)
	};
	ActiveLedItModule	_AtlModule;
}










/*
 ********************************************************************************
 ******************************** ActiveLedItApp ********************************
 ********************************************************************************
 */

ActiveLedItApp	theApp;

const GUID CDECL BASED_CODE _tlid =		{ 0x9a013a72, 0xad34, 0x11d0, { 0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73 } };
const WORD _wVerMajor = qLed_Version_Major;
const WORD _wVerMinor = qLed_Version_Minor;



BOOL	ActiveLedItApp::InitInstance ()
{
	SetRegistryKey (_T ("Sophist Solutions, Inc."));

	BOOL bInit = inherited::InitInstance ();

	if (bInit) {
		// Tell Led about the picture resources it needs to render some special embedding markers
#if		!qURLStyleMarkerNewDisplayMode
		StandardURLStyleMarker::sURLPict = (const Led_DIB*)::LoadDLLResource (kURLPictID, RT_BITMAP);
#endif
		StandardUnknownTypeStyleMarker::sUnknownPict = (const Led_DIB*)::LoadDLLResource (kUnknownEmbeddingPictID, RT_BITMAP);
		StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadDLLResource (kUnsupportedPICTFormatPictID, RT_BITMAP);

		// Tell Led about the kinds of embeddings we will allow
		EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();
	}
	return bInit;
}

int	ActiveLedItApp::ExitInstance ()
{
	// Detect SPR#1599 a little earlier. Basic trouble with this bug is that the
	// ActiveX control objects don't get destroyed - and even though the CanUnloadDLL function
	// returns false - UnloadDLL is called anyhow - and this is called indirectly.
	// Must find some bugfix or better bug workaround eventually. -- LGP 2003-12-16
	#if		qKeepListOfALInstancesForSPR_1599BWA
	while (not ActiveLedItControl::GetAll ().empty ()) {
		ActiveLedItControl*	p	=	*ActiveLedItControl::GetAll ().begin ();
		IUnknown*			pi	=	p->GetControllingUnknown ();
		while (pi->Release () > 0)
			;
	}
	#endif
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	ASSERT(pState != NULL);
	Led_Assert (pState->m_nCtrlRef == 0);
	Led_Assert (AfxGetModuleState ()->m_nObjectCount == 0)

	return inherited::ExitInstance ();
}






/*
 ********************************************************************************
 ******************************** DllRegisterServer *****************************
 ********************************************************************************
 */
STDAPI	DllRegisterServer ()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib (AfxGetInstanceHandle(), _tlid))
		return ResultFromScode (SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll (TRUE))
		return ResultFromScode (SELFREG_E_CLASS);


    // Mark the control as safe for initializing.
    HRESULT	hr = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");
    if (FAILED(hr))
        return hr;

    hr = RegisterCLSIDInCategory(CLSID_ActiveLedIt, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    // Mark the control as safe for scripting.
    hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");
    if (FAILED(hr))
        return hr;

    hr = RegisterCLSIDInCategory(CLSID_ActiveLedIt, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

	return NOERROR;
}






/*
 ********************************************************************************
 ******************************** DllUnregisterServer ***************************
 ********************************************************************************
 */
//	Removes entries from the system registry
STDAPI	DllUnregisterServer ()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

   // Remove entries from the registry.
    HRESULT	hr=UnRegisterCLSIDInCategory(CLSID_ActiveLedIt, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    hr=UnRegisterCLSIDInCategory(CLSID_ActiveLedIt, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

	if (!AfxOleUnregisterTypeLib (_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll (FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
