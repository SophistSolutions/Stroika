/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GroupView__
#define	__GroupView__

/*
 * $Header: /fuji/lewis/RCS/GroupView.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: GroupView.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  05:39:06  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_. And GroupView no longer inherits from Bordered, it
 *
 *		gets it indirectly from View.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/03/26  09:25:17  lewis
 *		Initial revision
 *
 *
 */

#include	"View.hh"




class	GroupView : public View {
	public:
		GroupView ();
		GroupView (const Rect& extent);

		nonvirtual	void	AddSubView (View* subView, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	AddSubView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);		
		nonvirtual	void	RemoveSubView (View* subView, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	ReorderSubView (View* v, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	ReorderSubView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	CollectionSize	GetSubViewCount () const;
		nonvirtual	CollectionSize	GetSubViewIndex (View* v);
		nonvirtual	View*			GetSubViewByIndex (CollectionSize index);
		nonvirtual	SequenceIterator(ViewPtr)*	MakeSubViewIterator (SequenceDirection d = eSequenceForward) const ;
	
		override	void	Draw (const Region& update);
};


class	LabeledGroup : public GroupView {
	public:
		LabeledGroup (const String& label);
		
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
	
		override	void	Draw (const Region& update);

	protected:
		virtual	void	SetLabel_ (const String& label, UpdateMode updateMode);
	
	private:
		String	fLabel;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



#endif	/*__GroupView__*/
