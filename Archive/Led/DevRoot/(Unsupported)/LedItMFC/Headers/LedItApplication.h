/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItApplication_h__
#define	__LedItApplication_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItApplication.h,v 2.16 2000/03/17 22:40:13 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItApplication.h,v $
 *	Revision 2.16  2000/03/17 22:40:13  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 2.15  1999/11/15 21:35:26  lewis
 *	react to some small Led changes so that Led itself no longer deends on MFC (though this demo app clearly DOES)
 *	
 *	Revision 2.14  1999/02/08 22:31:34  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 2.13  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  1997/12/24  04:06:51  lewis
 *	lose search params stuff from here - now in new options module.
 *
 *	Revision 2.11  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.10  1997/07/23  23:14:33  lewis
 *	OnIdle() override to call Led CallEnterIlde
 *
 *	Revision 2.9  1997/07/14  14:49:00  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.8  1997/03/23  01:01:15  lewis
 *	Handle bad-alloc excpetions and give particular alert for it.
 *
 *	Revision 2.7  1997/03/04  20:14:24  lewis
 *	Add OnToggleSmartCutNPasteOptionCommand/OnGotoLedItWebPageCommand/
 *	OnGotoSophistsWebPageCommand ().
 *
 *	Revision 2.6  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.5  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/09/30  14:53:54  lewis
 *	Keep search params associated with the app - not the view.
 *
 *	Revision 2.3  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/07/03  01:50:48  lewis
 *	Add DTOR, and WinHelp () override.
 *	Add LedItApplication::Get() method.
 *
 *	Revision 2.1  1996/06/01  02:37:46  lewis
 *	cleanup excpetion handling code.
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<afxole.h>

#include	"LedSupport.h"
#include	"TextStore.h"

#include	"LedItConfig.h"



class	LedItApplication : public CWinApp {
	private:
		typedef	CWinApp	inherited;

	public:
		LedItApplication ();
		~LedItApplication ();

	public:
		static	LedItApplication&	Get ();
	private:
		static	LedItApplication*	sThe;

	public:
		override	BOOL	InitInstance ();


	public:
		override	void	WinHelp (DWORD dwData, UINT nCmd = HELP_CONTEXT);

	// handle exceptions....
	public:
		override	BOOL	PumpMessage ();

		nonvirtual	void	HandleMFCException (CException* e) throw ();
		nonvirtual	void	HandleHRESULTException (HRESULT hr) throw ();
		nonvirtual	void	HandleBadAllocException () throw ();
		nonvirtual	void	HandleUnknownException () throw ();

	public:
		override	BOOL	OnIdle (LONG lCount);

	private:
		nonvirtual	void	AddDocTemplateForString (const char* tmplStr, bool connectToServer);

		COleTemplateServer fOleTemplateServer;

	protected:
		afx_msg	void	OnAppAbout ();
		afx_msg	void	OnToggleSmartCutNPasteOptionCommand ();
		afx_msg	void	OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleWrapToWindowOptionCommand ();
		afx_msg	void	OnToggleWrapToWindowOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleShowHiddenTextOptionCommand ();
		afx_msg	void	OnToggleShowHiddenTextOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnGotoLedItWebPageCommand ();
		afx_msg	void	OnGotoSophistsWebPageCommand ();

	private:
		nonvirtual	void	UpdateViewsForPrefsChange ();

	private:
		DECLARE_MESSAGE_MAP()
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__LedItApplication_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

