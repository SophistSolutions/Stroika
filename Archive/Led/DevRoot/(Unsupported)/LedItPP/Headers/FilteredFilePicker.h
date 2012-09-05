/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__FilteredFIlePicker_h__
#define	__FilteredFIlePicker_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/FilteredFilePicker.h,v 2.6 1997/12/24 04:41:30 lewis Exp $
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

#include	<StandardFile.h>
#include	<Dialogs.h>

#include	<LDocApplication.h>

#include	"LedSupport.h"

#include	"LedItConfig.h"



class	FilteredSFGetDLog {
	public:
		struct	TypeSpec {
			const char*	fName;
			OSType		fOSType;
		};
		FilteredSFGetDLog (const TypeSpec* typeNameList, size_t nTypes);

		nonvirtual	bool	PickFile (StandardFileReply* result, bool* typeSpecified, size_t* typeIndex);

	protected:
		static	pascal	short	SFGetDlgHook (short item, DialogPtr dialog, void* myData);
		static	pascal	Boolean	SFFilterProc (CInfoPBPtr pb, void* myData);

		const TypeSpec*	fTypeSpecs;
		size_t			fTypeSpecCount;
		size_t			fCurPopupIdx;
		size_t			fPopupDLGItemNumber;
		DialogPtr		fMainDialog;
};


class	FilteredSFPutDLog {
	public:
		struct	TypeSpec {
			const char*	fName;
			OSType		fOSType;
		};
		FilteredSFPutDLog (const TypeSpec* typeNameList, size_t nTypes);

		nonvirtual	bool	PickFile (ConstStr255Param defaultName, StandardFileReply* result, size_t* typeIndex);

	protected:
		static	pascal	short	SFPutDlgHook (short item, DialogPtr dialog, void* myData);

		const TypeSpec*	fTypeSpecs;
		size_t			fTypeSpecCount;
		size_t			fCurPopupIdx;
		size_t			fPopupDLGItemNumber;
		DialogPtr		fMainDialog;
};




#endif	/*__FilteredFIlePicker_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
