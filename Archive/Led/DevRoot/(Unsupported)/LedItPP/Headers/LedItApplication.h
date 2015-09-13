/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItApplication_h__
#define	__LedItApplication_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/LedItApplication.h,v 2.13 1997/12/24 04:41:30 lewis Exp $
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
 *	$Log: LedItApplication.h,v $
 *	Revision 2.13  1997/12/24 04:41:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.11  1997/07/14  01:23:43  lewis
 *	Renamed TextStore_ to TextStore etc..
 *
 *	Revision 2.10  1997/03/04  20:35:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.8  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/09/30  15:05:43  lewis
 *	Added fSearchParameters - search params now kept same accross windows.
 *	override        void    MakeMenuBar (); to work with new CW10 PowerPlant.
 *
 *	Revision 2.6  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/07/03  01:36:38  lewis
 *	Add LedItApplication::Get () method, and OnHelpMenuCommand()
 *
 *	Revision 2.4  1996/06/01  02:54:00  lewis
 *	Lose fLastShwonAT, but add UseIdleTime/fLastLowMemWarnAt;
 *
 *	Revision 2.3  1996/05/23  20:42:58  lewis
 *	Added fLastShownAt - for reduced no-mem warnigns.
 *
 *	Revision 2.2  1996/05/05  14:53:03  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1996/04/18  16:02:42  lewis
 *	override ProcessNextEvent ()
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<Dialogs.h>

#include	<LDocApplication.h>

#include	"LedSupport.h"
#include	"TextStore.h"

#include	"LedItConfig.h"



class	LedItApplication : public LDocApplication {
	public:
		LedItApplication ();
		virtual	~LedItApplication ();

	public:
		static	LedItApplication&	Get ();
	private:
		static	LedItApplication*	sThe;

	protected:
		typedef	LDocApplication	inherited;

	public:
		override	void	MakeMenuBar ();
		override	void	StartUp ();
		override	void	ProcessNextEvent ();
		override	void	HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber);

	protected:
		nonvirtual	void	HandleMacOSException (OSErr err);
		nonvirtual	void	HandlePowerPlantException (ExceptionCode err);
		nonvirtual	void	HandleUnknownException ();

	public:
		override	void	ShowAboutBox ();

		override	Boolean	ObeyCommand (CommandT inCommand, void *ioParam = nil);
		override	void	FindCommandStatus (CommandT inCommand,
									Boolean &outEnabled, Boolean &outUsesMark,
									Char16 &outMark, Str255 outName
								);

	public:
		nonvirtual	void	OnHelpMenuCommand ();
	private:
		short	fHelpMenuItem;

	public:
		nonvirtual	void	OnGotoLedItWebPageCommand ();
	private:
		short	fGotoLedItWebPageMenuItem;

	public:
		nonvirtual	void	OnGotoSophistsWebPageCommand ();
	private:
		short	fGotoSophistsWebPageMenuItem;


	public:
		nonvirtual	const TextStore::SearchParameters&	GetSearchParameters () const;
		nonvirtual	void								SetSearchParameters (const TextStore::SearchParameters& searchParameters);
	private:
		TextStore::SearchParameters	fSearchParameters;

	public:
		override	void	UseIdleTime (const EventRecord& inMacEvent);
	private:
		float	fLastLowMemWarnAt;

	public:
		override void			OpenDocument (FSSpec *inMacFSSpec);
		override void			OpenDocument (FSSpec *inMacFSSpec, Led_FileFormat format);
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
	inline	const TextStore::SearchParameters&	LedItApplication::GetSearchParameters () const
		{
			return fSearchParameters;
		}
	inline	void	LedItApplication::SetSearchParameters (const TextStore::SearchParameters& searchParameters)
		{
			fSearchParameters = searchParameters;
		}



#endif	/*__LedItApplication_h__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
