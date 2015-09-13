/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItServerItem_h__
#define	__LedItServerItem_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItServerItem.h,v 1.4 2002/05/06 21:30:59 lewis Exp $
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
 *	Revision 1.4  2002/05/06 21:30:59  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:44  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
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

