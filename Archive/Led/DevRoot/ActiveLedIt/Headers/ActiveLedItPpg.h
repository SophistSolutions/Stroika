/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedItPpg_h__
#define	__ActiveLedItPpg_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/ActiveLedItPpg.h,v 2.5 2002/05/06 21:34:19 lewis Exp $
 *
 * Description:
 *		Declaration of the ActiveLedItPropPage property page class.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedItPpg.h,v $
 *	Revision 2.5  2002/05/06 21:34:19  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.4  2001/11/27 00:32:28  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
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

#include	"Resource.h"



class ActiveLedItPropPage : public COlePropertyPage {
		DECLARE_DYNCREATE(ActiveLedItPropPage)
		DECLARE_OLECREATE_EX(ActiveLedItPropPage)

	// Constructor
	public:
		ActiveLedItPropPage();

		//{{AFX_DATA(ActiveLedItPropPage)
		enum { IDD = IDD_PROPPAGE_ACTIVELEDIT };
			// NOTE - ClassWizard will add data members here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_DATA

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	protected:
		//{{AFX_MSG(ActiveLedItPropPage)
			// NOTE - ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__ActiveLedItPpg_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

