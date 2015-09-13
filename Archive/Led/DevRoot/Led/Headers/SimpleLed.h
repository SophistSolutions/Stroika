/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SimpleLed_h__
#define	__SimpleLed_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SimpleLed.h,v 2.10 2004/01/01 23:33:03 lewis Exp $
 */


/*
@MODULE:	SimpleLed
@DESCRIPTION:
		<p>Simple wrappers on the word processing classes and line editing classes to make it a bit easier to
	instantiate a Led editor. Using these has some costs - however - as it removes some opportunities to customize exactly
	what you build into your Led-based editor. Still - its a very helpful way to get started using Led.</p>
 */


/*
 * Changes:
 *	$Log: SimpleLed.h,v $
 *	Revision 2.10  2004/01/01 23:33:03  lewis
 *	fix SimpleLed to no longer use WordProcessorHScrollbarHelper (obsolete)
 *	
 *	Revision 2.9  2003/04/04 19:52:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  2002/09/19 16:47:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  2002/09/19 14:26:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2002/09/19 12:54:57  lewis
 *	SPR#1099- fix args to OnCreate_Msg() to be JSUT the LPCREATESTRUCT. Override OnCreate() in
 *	MFCHELPER wrapper. Use OnCreate () msg hook to check incoming default window style - and to
 *	set the default value for SetScrollBarType () accordingly.
 *	
 *	Revision 2.5  2002/09/19 01:58:46  lewis
 *	SPR#1103- qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR workaround AND added _WIN32
 *	support AND added WP HSCROLLER code and command-handler.
 *	
 *	Revision 2.4  2002/05/06 21:33:32  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.3  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.2  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.1  2001/10/16 15:36:27  lewis
 *	SPR#1062- Renamed Led.h/cpp to SimpleLed.h/cpp. Renamed SimpleLed to SimpleLedWordProcessor.
 *	Added new SimpleLedLineEditor.
 *	
 *
 *
 *	<<<< BASED ON Led.cpp module - just renamed to SimpleLed.cpp -- LGP 2001-10-16 >>>>
 *
 *
 *	Revision 2.23  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  2000/07/25 14:10:48  lewis
 *	rename Led class to SimpleLed so as to not conflict with Led namespace - and
 *	adjusted Led tutorial accordinly
 *	
 *	Revision 2.21  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.20  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.19  1999/11/13 16:32:17  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.18  1999/05/03 22:04:50  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.17  1999/02/23 16:03:32  lewis
 *	fix Led::GetLayoutMargins () override
 *	
 *	Revision 2.16  1998/10/30 14:10:50  lewis
 *	revised GetLayoutWidth/GetLayoutMargins API
 *	
 *	Revision 2.15  1998/07/24  00:55:18  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1998/03/04  20:18:24  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.11  1997/07/23  23:01:53  lewis
 *	update for Led 2.2 changes, support powerplant, desupport tcl, and include AutoDoc docs.
 *
 *	Revision 2.10  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.8  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.6  1996/02/26  18:31:44  lewis
 *	Significant change here. No more version string. And now
 *	this class always creates text store. You its various base classes
 *	to subclass from.
 *
 *	Revision 2.5  1996/02/05  04:06:37  lewis
 *	Get rid of OpenDoc stuff, and now HERE is where we do the automatic consing
 *	up of text store - not various base clases.
 *
 *	Revision 2.4  1995/12/06  01:29:56  lewis
 *	replaced use of defined (_MSC_VER) with qQuiteAnnoyingDominanceWarnings.
 *
 *	Revision 2.3  1995/11/02  21:45:00  lewis
 *	Changed args to styled Led versions - take SharedStyleDatabase* now.
 *
 *	Revision 2.2  1995/10/19  22:00:50  lewis
 *	Now default to new StandardStyledWordWrappedLed_XXX
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.4  1995/05/29  23:44:47  lewis
 *	Changed emacs-mode string to be c++ from Text
 *
 *	Revision 1.3  1995/03/03  15:48:33  lewis
 *	Change kVersion from being hardwired here to being computed from #defines
 *	in LedConfig.hh
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *	<<<<From Led_DialogText - merged in here 960215>>>
 *	Revision 2.2  1996/02/05  04:08:01  lewis
 *	Use qMFC/qTCL instead of defined (__TCL__)/defined(_MFC_VER)
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.2  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.1  1995/05/21  17:01:41  lewis
 *	Initial revision
 *	<<<<End Led_DialogText - merged in here 960215>>>
 *
 */



#if		!defined (__PowerPlant__) && !defined (_MFC_VER) && !defined (_WIN32)
	#error	"This file provides a simple to use wrapper on one of the above class libraries"
	#error	"But you don't seem to be using any of them. Be sure to set the proper defines"
	#error	"if you ARE in fact using one of the supported class libraries."
#endif

#include	"ChunkedArrayTextStore.h"
#include	"Command.h"
#include	"SimpleTextInteractor.h"
#include	"WordProcessor.h"

#if		defined (__PowerPlant__)
	#include	"Led_PP.h"
#elif	defined (_MFC_VER)
	#include	"Led_MFC.h"
	#include	"Led_MFC_WordProcessor.h"
#elif	defined (_WIN32)
	#include	"Led_Win32.h"
#endif

#include	"WordProcessor.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
/*
@CLASS:			SimpleLedWordProcessor
@BASES:			@'Led_MFC', @'Led_PP', @'WordProcessor'
@DESCRIPTION:	<p>Led is a concrete editor class. You can directly instantiate and use this one.
			Or mix and match various component classes to make your own editor with the features
			you need. This is really only intended for use as a 'quick start' to using Led. You will probably
			want to directly use the base classes of Led, or various other components directly, once you get
			the hang of using Led.</p>
				<P>NB: if you are using PowerPlant, be sure to somehow arrange for "PP_Prefix.h" to be included before
			this file is (since thats where __PowerPlant__ is defined). Typically this will be done via the precompiled
			header prefix file.</p>
 */
class	SimpleLedWordProcessor
#if		defined (__PowerPlant__)
	: public Led_PPView_X<WordProcessor, Led_PPView_Traits_Default>
#elif	defined (_MFC_VER)
	: public Led_MFC_X<WordProcessor>
#elif	defined (_WIN32)
	: public Led_Win32_SimpleWndProc_Helper <Led_Win32_Helper <WordProcessor> >
#endif
	{
	private:
		#if		defined (__PowerPlant__)
			typedef	Led_PPView_X<WordProcessor, Led_PPView_Traits_Default>				inherited;
		#elif	defined (_MFC_VER)
			typedef	Led_MFC_X<WordProcessor>											inherited;
		#elif	defined (_WIN32)
			typedef	Led_Win32_SimpleWndProc_Helper <Led_Win32_Helper <WordProcessor> >	inherited;
		#endif
	public:
		SimpleLedWordProcessor ();
		~SimpleLedWordProcessor ();


	#if		qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
	public:
		#if		defined (_MFC_VER)
			override	void	OnInitialUpdate();
			override	void	PostNcDestroy ();
		#elif	defined (_WIN32)
			override	LRESULT	OnCreate_Msg (LPCREATESTRUCT createStruct);
			override	void	OnNCDestroy_Msg ();
		#endif
	#endif

	private:
		enum { kMaxUndoLevels	=	5 };
		ChunkedArrayTextStore			fTextStore;
		MultiLevelUndoCommandHandler	fCommandHandler;

	#if		defined (_MFC_VER)
	protected:
		DECLARE_MESSAGE_MAP()	   
		DECLARE_DYNCREATE(SimpleLedWordProcessor)
	#endif
};

#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif










#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
/*
@CLASS:			SimpleLedLineEditor
@BASES:			@'Led_MFC', @'Led_PP', @'SimpleTextInteractor'
@DESCRIPTION:	<p>Led is a concrete editor class. You can directly instantiate and use this one.
			Or mix and match various component classes to make your own editor with the features
			you need. This is really only intended for use as a 'quick start' to using Led. You will probably
			want to directly use the base classes of Led, or various other components directly, once you get
			the hang of using Led.</p>
				<P>NB: if you are using PowerPlant, be sure to somehow arrange for "PP_Prefix.h" to be included before
			this file is (since thats where __PowerPlant__ is defined). Typically this will be done via the precompiled
			header prefix file.</p>
 */
class	SimpleLedLineEditor
#if		defined (__PowerPlant__)
	: public Led_PPView_X<SimpleTextInteractor, Led_PPView_Traits_Default>
#elif	defined (_MFC_VER)
	: public Led_MFC_X<SimpleTextInteractor>
#elif	defined (_WIN32)
	: public Led_Win32_SimpleWndProc_Helper <Led_Win32_Helper <SimpleTextInteractor> >
#endif
	{
	private:
		#if		defined (__PowerPlant__)
			typedef	Led_PPView_X<SimpleTextInteractor, Led_PPView_Traits_Default>					inherited;
		#elif	defined (_MFC_VER)
			typedef	Led_MFC_X<SimpleTextInteractor>													inherited;
		#elif	defined (_WIN32)
			typedef	Led_Win32_SimpleWndProc_Helper <Led_Win32_Helper <SimpleTextInteractor> >		inherited;
		#endif
	public:
		SimpleLedLineEditor ();
		~SimpleLedLineEditor ();

	#if		qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
	public:
		#if		defined (_MFC_VER)
			override	void	OnInitialUpdate();
			override	void	PostNcDestroy ();
		#elif	defined (_WIN32)
			override	LRESULT	OnCreate_Msg (LPCREATESTRUCT createStruct);
			override	void	OnNCDestroy_Msg ();
		#endif
	#endif

	private:
		enum { kMaxUndoLevels	=	5 };
		ChunkedArrayTextStore			fTextStore;
		MultiLevelUndoCommandHandler	fCommandHandler;

	#if		defined (_MFC_VER)
	protected:
		DECLARE_MESSAGE_MAP()	   
		DECLARE_DYNCREATE(SimpleLedLineEditor)
	#endif
};

#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif









#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
/*
@CLASS:			LedDialogText
@BASES:			@'SimpleLedWordProcessor'
@DESCRIPTION:	<p>Very similar to class @'Led'. But with a few class-library-specific twists to make it easier
			to embed the Led view into a dialog. Not really needed. But if you are having trouble getting your Led-based
			view working in a dialog, you may want to try, or at least look at this code.</p>
				<p>PROBABLY SHOULD RE-IMPLEMENT THIS to be more like a textedit box. Make WordWrap optional. For MFC - use
			CWND instead of CView. Actually TEST it and make clear where/why its useful (UNICODE for one thing).
			</p>
 */
class	LedDialogText : public SimpleLedWordProcessor {
	private:
		typedef	SimpleLedWordProcessor	inherited;
	public:
		LedDialogText ();

	#if		qWindows && defined (_MFC_VER)
	public:
		override	void	PostNcDestroy ();
		override	int		OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	#endif

	#if		qWindows && defined (_MFC_VER)
		protected:
			DECLARE_MESSAGE_MAP()	   
			DECLARE_DYNCREATE(LedDialogText)
	#endif
};
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif












/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif


#endif	/*__SimpleLed_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
