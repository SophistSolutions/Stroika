/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextView__
#define	__TextView__

/*
 * $Header: /fuji/lewis/RCS/TextView.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *		Display of text in a single font and color, noneditable. Supports word wrap and text justification. 
 *
 *
 * TODO:
 *		TextView should be broken into mac/motif versions.
 *
 *
 * Changes:
 *	$Log: TextView.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/02  04:44:04  lewis
 *		Use Panel:: scope for UpdateMode args.
 *
 *		Revision 1.2  1992/06/25  07:15:42  sterling
 *		(LGP checking for sterl - he changed CalcDefaultSize to _, not sure what else).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/05/13  12:49:47  lewis
 *		STERL: Overload SetText() to take const char* arg.
 *
 *		Revision 1.9  92/03/26  09:38:44  09:38:44  lewis (Lewis Pringle)
 *		Added newFont and updateMode args to EffectiveFontChanged.
 *		
 *		Revision 1.7  1992/03/10  00:00:02  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.6  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.5  1992/02/21  20:06:00  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.4  1992/01/24  17:55:58  lewis
 *		OverRide Realize/Unrealize for motif textview.
 *
 *		Revision 1.2  1992/01/22  16:17:33  sterling
 *		TextEdit
 *
 */

#include	"View.hh"




#if		qXmToolkit
class	MotifOSControl;
#endif

class	AbstractTextView {
	public:
		enum	Justification { eJustLeft, eJustCenter, eJustRight, eJustFull, eSystemDefined };
		static	const	Boolean	kWordWrapOn;

	protected:
		AbstractTextView ();
	
	public:
		virtual	~AbstractTextView ();

		nonvirtual	String	GetText () const;
		nonvirtual	void	SetText (const String& text, Panel::UpdateMode updateMode = Panel::eDelayedUpdate);
		nonvirtual	void	SetText (const char* text, Panel::UpdateMode updateMode = Panel::eDelayedUpdate);
		
		nonvirtual	Justification	GetJustification () const;
		nonvirtual	void			SetJustification (Justification justification, Panel::UpdateMode updateMode = Panel::eDelayedUpdate);

		nonvirtual	Boolean	GetWordWrap () const;
		nonvirtual	void	SetWordWrap (Boolean wordWrap, Panel::UpdateMode updateMode = Panel::eDelayedUpdate);
	
		nonvirtual	void	Invariant () const;

	protected:
		virtual	String	GetText_ () const = Nil;
		virtual	void	SetText_ (const String& text, Panel::UpdateMode updateMode) = Nil;

		virtual	Justification	GetJustification_ () const = Nil;
		virtual	void			SetJustification_ (Justification justification, Panel::UpdateMode updateMode) = Nil;
		
		virtual	Boolean	GetWordWrap_ () const = Nil;
		virtual	void	SetWordWrap_ (Boolean wordWrap, Panel::UpdateMode updateMode) = Nil;

#if 	qDebug		
		virtual		void	Invariant_ () const;
#endif
};





class	TextView : public AbstractTextView, public View {
	public:
		TextView (const String& text = kEmptyString, Justification justification = eJustLeft);

		override	Boolean	GetLive () const;
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode);

#if		qMacToolkit
		override	void	Draw (const Region& update);
#endif

#if		qXmToolkit
		override	void	Layout ();
#endif

		override	String	GetText_ () const;
		override	void	SetText_ (const String& text, Panel::UpdateMode update);

		override	Justification	GetJustification_ () const;
		override	void			SetJustification_ (Justification justification, Panel::UpdateMode update);
		
		override	Boolean	GetWordWrap_ () const;
		override	void	SetWordWrap_ (Boolean wordWrap, Panel::UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, Panel::UpdateMode update); 
		override	void	SetMargin_ (const Point& margin, Panel::UpdateMode update); 

#if		qXmToolkit
		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();
#endif

	private:
		String			fText;
		Justification	fJustification;
		Boolean			fWordWrap;
#if		qXmToolkit
		MotifOSControl*	fLabelWidget;
#endif
};

extern	ostream&	operator<< (ostream& out, const TextView::Justification& just);
extern	istream&	operator>> (istream& in, TextView::Justification& just);




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	void	AbstractTextView::Invariant () const
	{
#if qDebug
		Invariant_ ();
#endif
	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__TextView__*/
