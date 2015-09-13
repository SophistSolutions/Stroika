/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItServerItem_h__
#define	__LedItServerItem_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItServerItem.h,v 2.5 1997/12/24 04:46:26 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItServerItem.h,v $
 *	Revision 2.5  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.3  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.2  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/06/01  02:34:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

class LedItServerItem : public COleServerItem {
		DECLARE_DYNAMIC(LedItServerItem)
	
	// Constructors
	public:
		LedItServerItem(LedItDocument* pContainerDoc);
	
	// Attributes
		LedItDocument* GetDocument() const
			{ return (LedItDocument*)COleServerItem::GetDocument(); }
	
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(LedItServerItem)
		public:
		virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
		virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
		//}}AFX_VIRTUAL
	
	// Implementation
	public:
		~LedItServerItem();
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif
	
	protected:
		virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

#endif	/*__LedItServerItem_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

