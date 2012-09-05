/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__FilteredFIlePicker_h__
#define	__FilteredFIlePicker_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/FilteredFilePicker.h,v 1.7 2003/03/13 02:36:01 lewis Exp $
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
 *	$Log: FilteredFilePicker.h,v $
 *	Revision 1.7  2003/03/13 02:36:01  lewis
 *	SPR#1337- bad prev checkin comment - REALLY added qUseNavServices support (and works at least basically on MacOSX
 *	
 *	Revision 1.6  2003/03/13 02:34:47  lewis
 *	SPR#1337 - I noted the indexes for the file formats where wrong in LedItApplication::ChooseDocument () MACONLY CODE
 *	
 *	Revision 1.5  2002/05/06 21:30:56  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.4  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.3  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2001/07/19 19:53:15  lewis
 *	SPR#0961- Carbon support
 *	
 *	Revision 1.1  2001/05/14 20:54:42  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *	Revision 2.6  1997/12/24 04:41:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.4  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.3  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/03/16  19:16:17  lewis
 *	Must keep track of fMainDialog to avoid bug with xltra subdialogs from
 *	CustomFileDialog.
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<Dialogs.h>
#include	<Navigation.h>
#include	<StandardFile.h>

#include	<LDocApplication.h>

#include	"LedSupport.h"

#include	"LedItConfig.h"




#ifndef	qUseNavServices
	#if		TARGET_CARBON
		#define	qUseNavServices	1
	#else
		#define	qUseNavServices	0
	#endif
#endif




class	FilteredSFGetDLog {
	public:
		struct	TypeSpec {
			const char*	fName;
			OSType		fOSType;
		};
		FilteredSFGetDLog (const TypeSpec* typeNameList, size_t nTypes);

		nonvirtual	bool	PickFile (FSSpec* result, bool* typeSpecified, size_t* typeIndex);


#if		qUseNavServices
	protected:
		static	pascal	void	StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData);
		virtual	void			NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams);
		static	pascal	Boolean	StaticNavObjectFilterProc (AEDesc* theItem, void* info, void* callBackUD, NavFilterModes filterMode);
		virtual	bool			NavObjectFilterProc (AEDesc* theItem, void* info, NavFilterModes filterMode);

	protected:
		NavDialogRef	fCurDialog;
#endif


	protected:
#if		!qUseNavServices && !TARGET_CARBON
		static	pascal	short	SFGetDlgHook (short item, DialogPtr dialog, void* myData);
		static	pascal	Boolean	SFFilterProc (CInfoPBPtr pb, void* myData);
#endif

		const TypeSpec*	fTypeSpecs;
		size_t			fTypeSpecCount;
		size_t			fCurPopupIdx;
		size_t			fPopupDLGItemNumber;
#if		!qUseNavServices && !TARGET_CARBON
		DialogPtr		fMainDialog;
#endif
};




class	FilteredSFPutDLog {
	public:
		struct	TypeSpec {
			const char*	fName;
			OSType		fOSType;
		};
		FilteredSFPutDLog (const TypeSpec* typeNameList, size_t nTypes);

		nonvirtual	bool	PickFile (ConstStr255Param defaultName, FSSpec* result, bool* replacing, size_t* typeIndex);

#if		qUseNavServices
	protected:
		static	pascal	void	StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData);
		virtual	void			NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams);

	protected:
		NavDialogRef	fCurDialog;
#endif


	protected:
#if		!qUseNavServices && !TARGET_CARBON
		static	pascal	short	SFPutDlgHook (short item, DialogPtr dialog, void* myData);
#endif

		const TypeSpec*	fTypeSpecs;
		size_t			fTypeSpecCount;
		size_t			fCurPopupIdx;
		size_t			fPopupDLGItemNumber;
#if		!qUseNavServices && !TARGET_CARBON
		DialogPtr		fMainDialog;
#endif
};




#endif	/*__FilteredFIlePicker_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
