/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Led_Win32.cpp,v 2.19 2004/02/10 03:06:03 lewis Exp $
 *
 * Changes:
 *	$Log: Led_Win32.cpp,v $
 *	Revision 2.19  2004/02/10 03:06:03  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.18  2004/01/26 19:59:38  lewis
 *	SPR#1604: lose fColumnNum - and use TextImager::GoalColumnRecomputerControlContext/
 *	ti.GetRowRelativeCharAtLoc(GetSelectionGoalColumn)) to provide more efficient, and better
 *	positioning of caret when adjusting selection due to scrolls
 *	
 *	Revision 2.17  2004/01/26 14:16:04  lewis
 *	SPR#1623: added new FunnyMSPageUpDownAdjustSelectionHelper
 *	
 *	Revision 2.16  2003/12/31 03:59:21  lewis
 *	SPR#1580: significant improvements to the SimpleWin32WndProcHelper class to help with this SPR.
 *	
 *	Revision 2.15  2003/07/16 22:02:46  lewis
 *	SPR#1549 - make sure the className depends on the ADDRESS of StaticWndProc
 *	
 *	Revision 2.14  2003/03/28 12:58:37  lewis
 *	SPR#1388: destruction order of IdleManager and OSIdleManagerImpls is arbitrary - since
 *	done by static file scope DTORs. Could work around this other ways (with .h file with
 *	ref count based DTOR - as old days of iostream hack) - but easier to just make
 *	IdleManager::SetIdleManagerOSImpl static method and careful not to recreate IdleManager
 *	if setting impl property to NULL
 *	
 *	Revision 2.13  2003/03/26 16:23:46  lewis
 *	Fix PeekMessage () calls so it will compile on MSVC60 and earlier (PM_QS_INPUT etc).
 *	And, SPR#1380 - Create window and register its class later - on demand - to avoid
 *	doing it too early in ActiveLedIt/RegServer context
 *	
 *	Revision 2.12  2003/03/21 14:50:46  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.11  2003/03/20 22:28:37  lewis
 *	lose an assert on messages we get in StaticMSG handler - dont think it matters
 *	
 *	Revision 2.10  2003/03/20 21:21:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2003/03/20 15:55:51  lewis
 *	SPR#1360 - added IdleManagerOSImpl_Win32 Win32 support for new Idler classes
 *	
 *	Revision 2.8  2003/02/03 20:56:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  2003/02/03 15:13:04  lewis
 *	SPR#1291 - Added qLedCheckCompilerFlagsConsistency to check for mismatches on compile flags
 *	
 *	Revision 2.6  2002/05/06 21:33:47  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.5  2001/11/27 00:29:53  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/28 18:43:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2001/04/26 16:39:29  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.2  2001/01/03 14:41:13  Lewis
 *	lose include file which is unneeded and caused problems compiling with borland C++
 *	
 *	Revision 2.1  1999/11/28 16:50:41  lewis
 *	new file - based on Led_MFC code - but now MFC independent
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"IdleManager.h"

#include	"Led_Win32.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
namespace	Led {
#endif




#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags_Led_Win32 {
		bool	LedCheckCompilerFlags_(qSupportWindowsSDKCallbacks)											=	qSupportWindowsSDKCallbacks;
		bool	LedCheckCompilerFlags_(qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug)	=	qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug;
	}
#endif




namespace Private {
	IdleMangerLinkerSupport::IdleMangerLinkerSupport ()
	{
	}
}









/*
@CLASS:			IdleManagerOSImpl_Win32
@BASES:			@'IdleManager::IdleManagerOSImpl'
@ACCESS:		public
@DESCRIPTION:	<p>Implemenation detail of the idle-task management system. This can generally be ignored by Led users.
			</p>
*/
class	IdleManagerOSImpl_Win32 : public IdleManager::IdleManagerOSImpl {
	public:
		IdleManagerOSImpl_Win32 ();
		~IdleManagerOSImpl_Win32 ();

	public:
		override	void	StartSpendTimeCalls ();
		override	void	TerminateSpendTimeCalls ();
		override	float	GetSuggestedFrequency () const;
		override	void	SetSuggestedFrequency (float suggestedFrequency);

	protected:
		nonvirtual	void	OnTimer_Msg (UINT nEventID, TIMERPROC* proc);

	protected:
		nonvirtual	void	CheckAndCreateIdleWnd ();

	public:
		static	LRESULT	CALLBACK	StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		enum	{ eTimerEventID		=	34252 };	// Magic#
		HWND	fIdleWnd;
		float	fSuggestedFrequency;
		UINT	fTimerID;
};



namespace	{
	/*
	 *	Code to automatically install and remove our idle manager.
	 */
	struct	IdleMangerSetter {
		IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (&fIdleManagerOSImpl);
			}
		~IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (NULL);
			}
		IdleManagerOSImpl_Win32	fIdleManagerOSImpl;
	};
	struct	IdleMangerSetter	sIdleMangerSetter;
}







/*
 ********************************************************************************
 ******************** FunnyMSPageUpDownAdjustSelectionHelper ********************
 ********************************************************************************
 */
FunnyMSPageUpDownAdjustSelectionHelper::FunnyMSPageUpDownAdjustSelectionHelper ():
	fRowNum (0)
{
}

void	FunnyMSPageUpDownAdjustSelectionHelper::CaptureInfo (TextInteractor& ti)
{
	size_t	pinPoint	=	ti.GetSelectionStart ();
	long rowNum		=	ti.CalculateRowDeltaFromCharDeltaFromTopOfWindow (long (pinPoint) - long (ti.GetMarkerPositionOfStartOfWindow ()));
	fRowNum = ::abs (rowNum);
}

void	FunnyMSPageUpDownAdjustSelectionHelper::CompleteAdjustment (TextInteractor& ti)
{
	// Finish the crazy caret adjustments!
	size_t	totalRowsInWindow	=	ti.GetTotalRowsInWindow ();		// not can have changed from above due to scrolling
	Led_Assert (totalRowsInWindow >= 1);
	fRowNum = min (fRowNum, totalRowsInWindow-1);

	size_t	newRowStart	=	ti.CalculateCharDeltaFromRowDeltaFromTopOfWindow (fRowNum) + ti.GetMarkerPositionOfStartOfWindow ();
	size_t	newRowEnd	=	ti.GetEndOfRowContainingPosition (newRowStart);
	Led_Assert (newRowEnd >= newRowStart);
	#if		qMultiByteCharacters
		size_t	newRowLen	=	newRowEnd - newRowStart;
	#endif

	TextImager::GoalColumnRecomputerControlContext skipRecompute (ti, true);

	size_t	newSelStart	=	ti.GetRowRelativeCharAtLoc (TextImager::Tablet_Acquirer (&ti)->CvtFromTWIPSH (ti.GetSelectionGoalColumn ()), newRowStart);

	newSelStart = min (newSelStart, newRowEnd);			// pin to END of row

	// Avoid splitting mbyte characters
	#if		qMultiByteCharacters
		if (newSelStart > newRowStart) {
			Led_SmallStackBuffer<Led_tChar>	buf (newRowLen);
			ti.CopyOut (newRowStart, newRowLen, buf);
			const	char*	text	=	buf;
			size_t			index	=	newSelStart-newRowStart;
			if (Led_FindPrevOrEqualCharBoundary (&text[0], &text[index]) != &text[index]) {
				Led_Assert (newSelStart > newRowStart);
				newSelStart--;
			}
		}
	#endif

	// Ha! Finally!
	ti.SetSelection (newSelStart, newSelStart);
}








/*
 ********************************************************************************
 **************************** SimpleWin32WndProcHelper **************************
 ********************************************************************************
 */

/*
@METHOD:		SimpleWin32WndProcHelper::Create
@DESCRIPTION:	<p></p>
*/
void	SimpleWin32WndProcHelper::Create (DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
{
	Led_SDK_String	tmpClassName;
	if (lpClassName == NULL) {
		tmpClassName	=	Format (_T("LED_SimpleWin32WndProcHelper-%d-%d"), ::GetCurrentProcessId (), reinterpret_cast<int> (StaticWndProc));
		lpClassName = tmpClassName.c_str ();
		{
			static	bool	sRegistered	=	false;
			if (not sRegistered) {
				WNDCLASSEX wcex;
				memset (&wcex, 0, sizeof (wcex));
				wcex.cbSize = sizeof(WNDCLASSEX); 
				wcex.lpfnWndProc	= (WNDPROC)StaticWndProc;
				wcex.lpszClassName	= lpClassName;
				ATOM	regResult	=	::RegisterClassEx (&wcex);
				if (regResult == 0) {
					DWORD	x = ::GetLastError ();
					if (x == ERROR_CLASS_ALREADY_EXISTS) {
						// Shouldn't happen - but if it does - SB OK since StaticWndProc addr is the same!
					}
					else {
						Led_Assert (false);	// could be a problem?
					}
				}
				sRegistered = true;
			}
		}
	}
	HWND	hWnd	=	::CreateWindowEx (dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, this);
	Led_Assert (hWnd == GetValidatedHWND ());	// already pre-set on the WM_CREATE message...
}

LRESULT	CALLBACK	SimpleWin32WndProcHelper::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE) {
		LPCREATESTRUCT	lpcs	=	(LPCREATESTRUCT) lParam;
		Led_AssertNotNil (lpcs);
		SimpleWin32WndProcHelper*	pThis	=	reinterpret_cast<SimpleWin32WndProcHelper*> (lpcs->lpCreateParams);
		Led_Assert (pThis->GetHWND () == NULL);	// cuz not set yet...
		pThis->SetHWND (hWnd);
	}

	SimpleWin32WndProcHelper*	pThis	=	reinterpret_cast<SimpleWin32WndProcHelper*> (::GetWindowLong (hWnd, GWL_USERDATA));

	if (pThis == NULL) {
		/*
			*	The only time I know of where this can happen is that these messages can be passed before the WM_CREATE.
			*	Seeing this assertion isn't necesarily a bug - but its a clue something wrong maybe going on.
			*/
		Led_Assert (message == WM_GETMINMAXINFO or message == WM_NCCREATE or message == WM_NCCALCSIZE);
		return ::DefWindowProc (hWnd, message, wParam, lParam);
	}

	Led_Assert (pThis != NULL);
	Led_Assert (pThis->GetHWND () == hWnd);
	return pThis->WndProc (message, wParam, lParam);
}







/*
 ********************************************************************************
 **************************** IdleManagerOSImpl_Win32 ***************************
 ********************************************************************************
 */
IdleManagerOSImpl_Win32::IdleManagerOSImpl_Win32 ():
	fIdleWnd (NULL),
	fSuggestedFrequency (0),
	fTimerID (0)
{
}

IdleManagerOSImpl_Win32::~IdleManagerOSImpl_Win32 ()
{
	if (fIdleWnd != NULL) {
		::DestroyWindow (fIdleWnd);
	}
}

void	IdleManagerOSImpl_Win32::StartSpendTimeCalls ()
{
	CheckAndCreateIdleWnd ();
	Led_AssertNotNil (fIdleWnd);
	// ignore if already started
	if (fTimerID == 0) {
		int	timeout	=	static_cast<int> (fSuggestedFrequency * 1000);	// cvt specified frequency to milliseconds
		Led_Verify (fTimerID = ::SetTimer (fIdleWnd, eTimerEventID, timeout, NULL));
	}
}

void	IdleManagerOSImpl_Win32::TerminateSpendTimeCalls ()
{
	if (fTimerID != 0) {
		Led_AssertNotNil (fIdleWnd);
		Led_Verify (::KillTimer (fIdleWnd, eTimerEventID));
		fTimerID = 0;
	}
}

float	IdleManagerOSImpl_Win32::GetSuggestedFrequency () const
{
	return fSuggestedFrequency;
}

void	IdleManagerOSImpl_Win32::SetSuggestedFrequency (float suggestedFrequency)
{
	if (fSuggestedFrequency != suggestedFrequency) {
		fSuggestedFrequency = suggestedFrequency;
		if (fTimerID != 0) {
			TerminateSpendTimeCalls ();
			StartSpendTimeCalls ();
		}
	}
}

LRESULT	CALLBACK	IdleManagerOSImpl_Win32::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE) {
		LPCREATESTRUCT	lpcs	=	reinterpret_cast<LPCREATESTRUCT> (lParam);
		Led_AssertNotNil (lpcs);
		IdleManagerOSImpl_Win32*	pThis	=	reinterpret_cast<IdleManagerOSImpl_Win32*> (lpcs->lpCreateParams);
 		::SetWindowLong (hWnd, GWL_USERDATA, reinterpret_cast<LONG> (pThis));
	}

	IdleManagerOSImpl_Win32*	pThis	=	reinterpret_cast<IdleManagerOSImpl_Win32*> (::GetWindowLong (hWnd, GWL_USERDATA));

	if (pThis == NULL) {
		return ::DefWindowProc (hWnd, message, wParam, lParam);
	}

	Led_Assert (pThis != NULL);
	switch (message) {
		case WM_TIMER:			pThis->OnTimer_Msg (wParam, reinterpret_cast<TIMERPROC*> (lParam)); return 0;
	}
	return ::DefWindowProc (hWnd, message, wParam, lParam);
}

void	IdleManagerOSImpl_Win32::OnTimer_Msg (UINT /*nEventID*/, TIMERPROC* /*proc*/)
{
	/*
	 *	Check if any input or paint messages pending, and if so - ignore the timer message as
	 *	this isn't really IDLE time.
	 */
	#if		defined (PM_QS_INPUT) || defined (PM_QS_PAINT)
		MSG	msg;
		if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_INPUT | PM_QS_PAINT) == 0) {
			CallSpendTime ();
		}
	#else
		//PM_QS_INPUT and PM_QS_PAINT not defined in MSVC60
		CallSpendTime ();
	#endif
}

void	IdleManagerOSImpl_Win32::CheckAndCreateIdleWnd ()
{
	if (fIdleWnd == NULL) {
		// Because of SPR#1549 - make sure the className depends on the ADDRESS of StaticWndProc
		Led_SDK_String	className	=	Format (Led_SDK_TCHAROF ("Led::IdleManagerOSImpl_Win32 (0x%x)"), reinterpret_cast<int> (StaticWndProc));
		static	Led_SDK_String	sRegisteredClassName;
		if (sRegisteredClassName != className) {
			WNDCLASSEX wcex;
			memset (&wcex, 0, sizeof (wcex));
			wcex.cbSize = sizeof(WNDCLASSEX); 
			wcex.lpfnWndProc	= (WNDPROC)StaticWndProc;
			wcex.lpszClassName	= className.c_str ();
			ATOM	regResult	=	::RegisterClassEx (&wcex);
			if (regResult == 0) {
				DWORD	x = ::GetLastError ();
				if (x == ERROR_CLASS_ALREADY_EXISTS) {
					// ignore - probably multiple template instantiations, or something like that. SB OK. They are all identical.
				}
				else {
					Led_Assert (false);	// could be a problem?
				}
			}
			sRegisteredClassName = className;
		}
		fIdleWnd = ::CreateWindowEx (0, className.c_str (), _T(""), 0, 0, 0, 1, 1, NULL, NULL, NULL, this);
	}
}


#if		qLedUsesNamespaces
}
#endif

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


