/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItServerItem_h__
#define	__LedLineItServerItem_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItServerItem.h,v 2.6 2002/05/06 21:31:15 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItServerItem.h,v $
 *	Revision 2.6  2002/05/06 21:31:15  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.5  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/23  16:36:42  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	<afxole.h>

#include	"LedSupport.h"

#include	"LedLineItConfig.h"



class	LedLineItDocument;

class	LedLineItServerItem : public COleServerItem {
	public:
		LedLineItServerItem (LedLineItDocument* pContainerDoc);

	public:
		nonvirtual	LedLineItDocument*	GetDocument () const;
	
	public:
		override	BOOL OnDraw (CDC* pDC, CSize& rSize);
		override	BOOL OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize);

	protected:
		virtual void Serialize (CArchive& ar);   // overridden for document i/o

	#ifdef _DEBUG
	public:
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	private:
		DECLARE_DYNAMIC(LedLineItServerItem)
};

#endif	/*__LedLineItServerItem_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

