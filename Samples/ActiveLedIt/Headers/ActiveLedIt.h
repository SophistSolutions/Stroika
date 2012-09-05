/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedIt_h__
#define	__ActiveLedIt_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/ActiveLedIt.h,v 2.7 2004/02/03 00:05:13 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedIt.h,v $
 *	Revision 2.7  2004/02/03 00:05:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2002/05/06 21:34:18  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.5  2001/11/27 00:32:28  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/30 00:35:55  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2000/10/16 22:50:21  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.2  1997/12/24 04:43:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  1997/07/27  15:59:52  lewis
 *	<===== Led 2.2 Released =====>
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

#include	"ActiveLedItConfig.h"


class	ActiveLedItApp : public COleControlModule {
	private:
		typedef	COleControlModule	inherited;
	public:
		override	BOOL	InitInstance ();
		override	int		ExitInstance ();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__ActiveLedIt_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

