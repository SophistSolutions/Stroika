/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Sources/ActiveSpelledItMain.cpp,v 1.3 2003/06/10 16:30:32 lewis Exp $
 *
 * Changes:
 *	$Log: ActiveSpelledItMain.cpp,v $
 *	Revision 1.3  2003/06/10 16:30:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2003/06/10 14:30:04  lewis
 *	SPR#1526: more work - improved test suite - added basic DEMO-MODE support,
 *	added version# OLE AUTOMATION API, exception safety in OLEAUT code, and maybe
 *	a bit more...
 *	
 *	Revision 1.1  2003/06/10 02:06:18  lewis
 *	first cut at ActiveSpelledIt control
 *	
 *
 *
 *
 */
#include	"ActiveSpelledItConfig.h"

#include	"Led_ATL.h"

#include	"resource.h"

#include	"ActiveSpelledIt_IDL.h"




class	ActiveSpelledItModule : public CAtlDllModuleT <ActiveSpelledItModule> {
	public:
		DECLARE_LIBID (LIBID_ActiveSpelledItLib)
		DECLARE_REGISTRY_APPID_RESOURCEID (IDR_ACTIVESPELLEDIT, "{A2EC28DE-F05D-400D-9A1D-957684A6EB0E}")
};

ActiveSpelledItModule	_AtlModule;





#if		qDemoMode
/*
 ********************************************************************************
 ******************************* DemoModeAlerter ********************************
 ********************************************************************************
 */
int				DemoModeAlerter::sNextWarningAfter		=	0;
const	float	DemoModeAlerter::kTimeBetweenWarnings	=	0.5*60;	// at most every 1 minute(s) - but really less often - cuz only on draws...

void	DemoModeAlerter::ShowAlert (HWND parentWnd)
{
	if (parentWnd == NULL) {
		parentWnd = ::GetActiveWindow ();
	}
	// Only check if we are visible. Sometimes this gets called - somewhat inexplicably - during the close of the browser
	// window. Avoid somewhat ugly/confusing display of alert.
	if (::IsWindow (parentWnd) and ::IsWindowVisible (parentWnd)) {
		sNextWarningAfter = INT_MAX;	// disable warnings til after Dialog finishes display to prevent multiple dialogs from appearing
		try {
			int result	=	::MessageBox (parentWnd,
											_T("The ActiveSpelledIt! demo has expired. Go to the ActiveSpelledIt website to find a newer version, or to purchase a copy."),
											_T ("ActiveSpelledIt! Demo Expired"),
											MB_OKCANCEL | MB_TASKMODAL
									);
			if (result == IDOK) {
				Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveSpelledIt/DemoExpired.asp", kAppName, string (kURLDemoFlag) + kDemoExpiredExtraArgs));
			}
			int		now						=	time (NULL);
			sNextWarningAfter = static_cast<int> (now + kTimeBetweenWarnings);	// Set here so delay based on when dialog dismissed instead of when dialog comes up.
		}
		catch (...) {
			// ignore any errors with this code...
			int		now						=	time (NULL);
			sNextWarningAfter = static_cast<int> (now + kTimeBetweenWarnings);	// even on failure - reset timer for next call
		}
	}
}

void	DemoModeAlerter::ShowAlertIfItsBeenAWhile (HWND parentWnd)
{
	int		now						=	time (NULL);
	if (now > sNextWarningAfter) {
		ShowAlert (parentWnd);
	}
	else {
		Led_BeepNotify ();
	}
}
#endif





// DLL Entry Point
extern "C" BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	Led_Arg_Unused (hInstance);
    return _AtlModule.DllMain (dwReason, lpReserved); 
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI	DllCanUnloadNow ()
{
    return _AtlModule.DllCanUnloadNow ();
}

// Returns a class factory to create an object of the requested type
STDAPI	DllGetClassObject (REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject (rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI	DllRegisterServer ()
{
    // registers object, typelib and all interfaces in typelib
    return _AtlModule.DllRegisterServer ();
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI	DllUnregisterServer ()
{
	return _AtlModule.DllUnregisterServer ();
}
