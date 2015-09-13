/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItApplication_h__
#define	__LedLineItApplication_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Headers/LedLineItApplication.h,v 2.12 2003/12/31 16:31:22 lewis Exp $
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
 *	$Log: LedLineItApplication.h,v $
 *	Revision 2.12  2003/12/31 16:31:22  lewis
 *	small changes to get compiling with recent SearchParam changes for find dialog/replace dialog
 *	
 *	Revision 2.11  2003/12/13 02:00:05  lewis
 *	small changes so uses new spellcheck code (and stores engine in applicaiton object)
 *	
 *	Revision 2.10  2003/03/11 02:40:36  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 2.9  2002/05/06 21:31:26  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:26  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/09/12 00:22:17  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.6  2001/08/30 01:01:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.4  1997/12/24 04:52:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1997/07/27  16:03:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/07/14  14:44:28  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.1  1997/03/04  20:27:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:51:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	<Dialogs.h>

#include	<LDocApplication.h>

#include	"LedSupport.h"
#include	"TextInteractor.h"

#include	"LedLineItConfig.h"

#if		qIncludeBasicSpellcheckEngine
#include	"SpellCheckEngine_Basic.h"
#endif


class	LedLineItApplication : public LDocApplication {
	public:
		LedLineItApplication ();
		virtual	~LedLineItApplication ();

	public:
		static	LedLineItApplication&	Get ();
	private:
		static	LedLineItApplication*	sThe;

	private:
		typedef	LDocApplication	inherited;

	#if		qIncludeBasicSpellcheckEngine
	public:
		SpellCheckEngine_Basic_Simple	fSpellCheckEngine;
	#endif

		public:
		override	void	MakeMenuBar ();
		override	void	StartUp ();
		override	void	ProcessNextEvent ();
		override	void	HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber);

	protected:
		nonvirtual	void	HandleMacOSException (OSErr err);
		nonvirtual	void	HandlePowerPlantException (ExceptionCode err);
		nonvirtual	void	HandleBadUserInputException () throw ();
		nonvirtual	void	HandleUnknownException ();

	public:
		override	void	ShowAboutBox ();

		override	Boolean	ObeyCommand (CommandT inCommand, void *ioParam = nil);
		override	void	FindCommandStatus (CommandT inCommand,
									Boolean &outEnabled, Boolean &outUsesMark,
									UInt16& outMark, Str255 outName
								);

	public:
		nonvirtual	void	OnHelpMenuCommand ();
	private:
		short	fHelpMenuItem;

	public:
		nonvirtual	void	OnGotoLedLineItWebPageCommand ();
	private:
		short	fGotoLedLineItWebPageMenuItem;


	public:
		nonvirtual	void	OnGotoSophistsWebPageCommand ();
	private:
		short	fGotoSophistsWebPageMenuItem;


	public:
		nonvirtual	void	OnCheckForUpdatesWebPageCommand ();
	private:
		short	fCheckForUpdatesWebPageMenuItem;


	public:
		nonvirtual	const TextInteractor::SearchParameters&	GetSearchParameters () const;
		nonvirtual	void									SetSearchParameters (const TextInteractor::SearchParameters& searchParameters);
	private:
		TextInteractor::SearchParameters	fSearchParameters;

	public:
		override	void	UseIdleTime (const EventRecord& inMacEvent);
	private:
		float	fLastLowMemWarnAt;

	public:
		override void			OpenDocument (FSSpec *inMacFSSpec);
		override LModelObject*	MakeNewDocument();

	public:
		override void	ChooseDocument ();
	private:
		static	pascal	Boolean	SFGetDlgModalFilter (DialogPtr dialog, EventRecord* theEvent, short* itemHit, void* myData);
		static	pascal	short	SFGetDlgHook (short item, DialogPtr dialog, void* myData);
};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	const TextInteractor::SearchParameters&	LedLineItApplication::GetSearchParameters () const
		{
			return fSearchParameters;
		}
	inline	void	LedLineItApplication::SetSearchParameters (const TextInteractor::SearchParameters& searchParameters)
		{
			fSearchParameters = searchParameters;
		}



#endif	/*__LedLineItApplication_h__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
