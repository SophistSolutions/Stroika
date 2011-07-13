/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Adornment__
#define	__Adornment__

/*
 * $Header: /fuji/lewis/RCS/Adornment.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *		(1)		Remove dependency on peeking at internals of View, if possible, and also,
 *				remove need to call view draw methods - so we can make that stuff protected in view!!!
 *
 * Notes:
 *
 * Changes:
 *	$Log: Adornment.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/04  14:02:47  lewis
 *		Take advantage of new shape support- pass by value, instead of reference.
 *
 *		Revision 1.2  1992/07/02  03:44:23  lewis
 *		Properly scope UpdateMode type.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/05/23  00:59:09  lewis
 *		Get rid of EmptyAdornmt since unused and phasing out adornments anyhow.
 *
 *		Revision 1.7  1992/02/21  21:04:52  lewis
 *		Use macro INHERRIT_FROM_SINGLE_OBJECT instead of class SingleObject so
 *		that crap doesnt appear in non-mac inheritance graphs.
 *
 *		Revision 1.6  1992/02/21  18:01:00  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 *
 *
 */

#include	"Shape.hh"
#include	"View.hh"



class	Adornment INHERRIT_FROM_SINGLE_OBJECT {
	public:
		Adornment ();
		virtual	~Adornment ();
		
		virtual	void	Draw ()	= Nil;
		
		nonvirtual	Region	GetAdornRegion () const;
		
		nonvirtual	View&	GetOwner () const;

	protected:
		virtual		void	Invalidate (Panel::UpdateMode updateMode = View::eNoUpdate);
		nonvirtual	void	Refresh (Panel::UpdateMode updateMode);
		nonvirtual	void	Refresh (const Region& region, Panel::UpdateMode updateMode);
		virtual		void	SetAdornRegion (const Region& region, Panel::UpdateMode updateMode);
		virtual		Region	CalcAdornRegion (const Region& bounds) const = Nil;

	private:
		Region	fAdornRegion;
		Boolean	fRegionValid;
		View*	fOwner;
		
		friend	class	View;					// should be View::SetAdornment (Adornment* adornment, Panel::UpdateMode updateMode)
												// but for 2.0 CFront compiler bug
		friend	class	CompositeAdornment;
};



/*
 * A CompositeAdornment allows users to maintain a linked list of adornments. CompositeAdornments always have a valid
 * adornment, and may or may not have a "next" adornment (depending on the length of an adornment list);
 */
class	CompositeAdornment : public Adornment {
	public:
		CompositeAdornment (Adornment* adornment, CompositeAdornment* next = Nil);
		~CompositeAdornment ();
		
		override	void	Draw ();
		
		/*
		 *	returns a (potentially) new CompositeAdornment. Usage is to assign this return value
		 *	to the variable referencing the adornment as the old "this" pointer is now bad
		 *	i.e. fMyAdornment = fMyAdornment->RemoveAdornment (anAdornment)
		 *	this routine destroys the value of adornment
		 */
		nonvirtual	CompositeAdornment*	RemoveAdornment (Adornment* adornment);

	protected:
		override	Region	CalcAdornRegion (const Region& bounds) const;
		override	void	Invalidate (Panel::UpdateMode updateMode = View::eNoUpdate);
	
	private:
		Adornment*			fAdornment;
		CompositeAdornment*	fNextAdornment;
};




class	ShapeAdornment : public Adornment {
	public:
		static	const	Point	kDefaultInset;
		static	const	Pen		kDefaultPen;
		
		ShapeAdornment (const Shape& shape, const Pen& pen, const Point& inset = kDefaultInset);
		ShapeAdornment (const Shape& shape);
		~ShapeAdornment ();

		override	void	Draw ();
		override	Region	CalcAdornRegion (const Region& bounds) const;

		nonvirtual	const Shape&	GetShape () const;
		nonvirtual	void			SetShape (const Shape& newShape, Panel::UpdateMode updateMode = View::eDelayedUpdate);		// NB: object refernce saved, not copied!

		nonvirtual	const Pen&		GetPen () const;
		nonvirtual	void			SetPen (const Pen& pen, Panel::UpdateMode updateMode = View::eDelayedUpdate);

		nonvirtual	Point		GetInset () const;
		nonvirtual	void		SetInset (const Point& inset, Panel::UpdateMode updateMode = View::eDelayedUpdate);

	private:
		Shape	fShape;
		Pen		fPen;
		Point	fInset;
};



class	TitledAdornment : public ShapeAdornment {
	public:
		TitledAdornment (const String& title, const Shape& shape, Boolean outlineShape = True);
		TitledAdornment (const String& title, const Shape& shape, const Pen& pen, const Point& inset = kDefaultInset);
		
		override	void	Draw ();
		override	Region	CalcAdornRegion (const Region& bounds) const;

		nonvirtual	String	GetTitle () const;
		virtual		void	SetTitle (const String& title, Panel::UpdateMode updateMode = View::eDelayedUpdate);
		
		nonvirtual	Font	GetAdornFont () const;
		virtual		void	SetAdornFont (const Font& f, Panel::UpdateMode updateMode = View::eDelayedUpdate);

	protected:
		nonvirtual	Rect	GetTitleRect () const;

	private:
		String	fTitle;
		Font	fAdornFont;
		Boolean	fOutlineShape;
};



class	LabelAdornment : public ShapeAdornment {
	public:
		static	const	Coordinate	kDefaultGap;

		LabelAdornment (const String& title, const Shape& shape, Boolean outlineShape = True);
		LabelAdornment (const String& label, const Shape& shape, const Pen& pen, const Point& inset = kDefaultInset);
		
		override	void	Draw ();
		override	Region	CalcAdornRegion (const Region& bounds) const;

		nonvirtual	String	GetLabel () const;
		virtual		void	SetLabel (const String& label, Panel::UpdateMode updateMode = View::eDelayedUpdate);
		
		nonvirtual	Font	GetAdornFont () const;
		virtual		void	SetAdornFont (const Font& f, Panel::UpdateMode updateMode = View::eDelayedUpdate);
		
		nonvirtual	Coordinate	GetGap () const;
		virtual		void		SetGap (Coordinate gap, Panel::UpdateMode updateMode = View::eDelayedUpdate);

	protected:
		nonvirtual	Rect	GetLabelRect () const;

	private:
		String		fLabel;
		Font		fAdornFont;
		Coordinate	fGap;
		Boolean		fOutlineShape;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	View&			Adornment::GetOwner () const		{	EnsureNotNil (fOwner); return (*fOwner); }
inline	const Pen&		ShapeAdornment::GetPen () const		{	return (fPen);		}
inline	const Shape&	ShapeAdornment::GetShape () const	{	return (fShape);	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Adornment__*/


