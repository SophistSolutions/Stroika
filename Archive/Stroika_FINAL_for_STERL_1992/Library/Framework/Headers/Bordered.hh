/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Bordered__
#define	__Bordered__	1
/*
 * $Header: /fuji/lewis/RCS/Bordered.hh,v 1.3 1992/07/02 03:45:11 lewis Exp $
 *
 * Description:
 *
 * Mix-in for views that want to support a border, that is, a shadowing frame. Has two
 * values, Border, representing the width of the line itself, and Margin, representing
 * the gap between the border and the rest of the view. In addition, it has a three dimensional
 * depth, called its Plane which can be either in or out, or etched in or out. Etched in means in
 * only slightly, and etched out means out only slightly.
 *
 * There is no corresponding concepts on the Macintosh, although TextEdit in dialogs usually 
 * do something like this (albeit without any help from the toolbox). Also pushbuttons by
 * default have a border of 1. By default, the plane values are ignored on the Macintosh.
 *
 * Motif has these concepts, but with slightly different names. It uses XmNShadowThickness to 
 * correspond to border, and XmNMarginHeight and XmNMarginWidth for margin. There are
 * differences between these concepts, however. I allow the programmer to specify a separate
 * shadow height and width, whereas Motif allows only a single uniform thickness.
 *
 * In Motif there is usually (always?) an additional set of margins, specified by XmNmarginLeft,
 * XmNmarginTop, XmNmarginLeft, and XmNmarginRight. I do not allow access to these parameters 
 * because (1) Kobara discourages such practices, (2) Motif feels free to reset these parameters
 * at will for its own purposes (worse, the implementations of these resets is at least sometimes
 * buggy)
 *
 * Finally, Motif has another resource called, discouragingly enough, XmNborderWidth. It is defined
 * in Core, and defaults to 1. However, every widget I could find resets this default value to zero,
 * and I can find no definition for this resource anywhere in the Motif documentation. Further,
 * the documentation frequently treats "shadow" and "border" as synonyms. Therefore I currently
 * ignore this resource altogether, assuming that it is obsolete.
 *
 * TODO:
 *
 * Changes:
 *	$Log: Bordered.hh,v $
 *		Revision 1.3  1992/07/02  03:45:11  lewis
 *		Properly scoped UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  04:45:50  sterling
 *		Sterl made lots of changes (LGP checking in) to this - got rid  of BorderView class,
 *		and I think instead mixes this into View itself. Changed names for enumerators for kinds
 *		of planes (recessed, etchedIn/Out, etc).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.4  1992/06/09  15:04:14  sterling
 *		many tweaks, including C functions to do motif style borders
 *
 *		Revision 1.3  92/04/30  13:06:16  13:06:16  sterling (Sterling Wight)
 *		added BorderedView classes
 *		
 *		Revision 1.2  92/04/20  14:16:34  14:16:34  lewis (Lewis Pringle)
 *		Get rid of CalcShadoowThickness for Bordered - it was a virtual method, conditionally compiled into this
 *		class. This had the unfortunate side effect of making VERY difficult portable motif GUI controls - since they
 *		naturaly wanted to use this function - at least as much as any native versions did. Since this feature was
 *		clearly GUI dependent, I had two choices - a GUI splitting of classes, or blow it away - and since it was VERY
 *		rarely used, and very simple, I just inlined it.
 *		If you find you counted on it, its implenetnation was just Max (GetMarginWidth(),GetMarginHeight()) - I generally
 *		replaced it with Min rather than Max, since that seemed more likely correct, thgough in no case I could
 *		find did the distinction matter.
 *		
 *		Revision 1.1  92/03/05  20:09:46  20:09:46  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */

#include	"Debug.hh"

#include	"Color.hh"

#include	"Config-Framework.hh"
#include	"Panel.hh"




class	Bordered {
	public:
		enum	Plane { eIn, eEtchedIn, eEtchedOut, eOut, };
		
	protected:
		Bordered ();				// abstract class

	public:
		virtual ~Bordered ();
		
		nonvirtual	Point	GetBorder () const;
		nonvirtual	void	SetBorder (const Point& border, Panel::UpdateMode updateMode = Panel::eDelayedUpdate); 
		nonvirtual	void	SetBorder (Coordinate height, Coordinate width, UpdateMode updateMode = Panel::eDelayedUpdate); 

		nonvirtual	Point	GetMargin () const;
		nonvirtual	void	SetMargin (const Point& margin, UpdateMode updadeMode = Panel::eDelayedUpdate); 
		nonvirtual	void	SetMargin (Coordinate height, Coordinate width, UpdateMode updateMode = Panel::eDelayedUpdate); 

		nonvirtual	Bordered::Plane	GetPlane () const;
		nonvirtual	void			SetPlane (Bordered::Plane plane, Panel::UpdateMode updadeMode = Panel::eDelayedUpdate);
		
	protected:
		virtual	void	SetBorder_ (const Point& border, Panel::UpdateMode updateMode);
		virtual	void	SetMargin_ (const Point& margin, Panel::UpdateMode updateMode);
		virtual	void	SetPlane_ (Bordered::Plane plane, Panel::UpdateMode updateMode);

	private:
		Point	fBorder;
		Point	fMargin;
		Plane	fPlane;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Point			Bordered::GetBorder () const	{	return (fBorder);	}
inline	Point			Bordered::GetMargin () const	{	return (fMargin);	}
inline	Bordered::Plane	Bordered::GetPlane () const		{	return (fPlane);	}




/*
 * For gnuemacs:
 * Local Variables: ***
 * mode:C++ ***
 * tab-width:4 ***
 * End: ***
 */

#endif	/*__Bordered__*/
