/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextView.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Changes:
 *	$Log: TextView.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/08  03:41:23  lewis
 *		Use AssertNotReached - etc instead of SwitchFallThru ().
 *
 *		Revision 1.3  1992/07/03  02:19:22  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum. And use
 *		AbstractTextView:: to scope access to Justification.
 *
 *		Revision 1.2  1992/06/25  08:50:27  sterling
 *		Lots of changes including renamed CalcDefaultSize to CalcDefaultSize_, and font changes.
 *
 *		Revision 1.17  1992/05/13  12:55:40  lewis
 *		STERL: Futzed with definition of CalcDefaultSize () on mac with word-wrap - set
 *		Border/Margin in owned view rather than taking into account ourselves - oops - sorry I forgot???.
 *		Also, added SetText () overload for char*.
 *
 *		Revision 1.16  92/04/20  14:35:33  14:35:33  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		Also, use )Min (GetBorder ().GetV (), GetBorder ().GetH ()) instead of Bordered::CalcShadowThickness () since that
 *		rouitine went away - See rcslog in Bordered.hh.
 *		
 *		Revision 1.15  92/04/02  13:07:40  13:07:40  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.14  92/03/26  09:43:24  09:43:24  lewis (Lewis Pringle)
 *		Added EffectiveFontChanged method to inval layouts, etc - stuff that used to be done in View, but
 *		now we do it only for classes which need it.
 *		
 *		Revision 1.12  1992/03/10  00:04:40  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.9  1992/03/05  21:15:52  sterling
 *		support for borders
 *
 *		Revision 1.8  1992/01/31  17:01:29  sterling
 *		minoc cleanups
 *		no longer has default font, inherits from parent instead
 *
 *		Revision 1.7  1992/01/29  04:51:00  sterling
 *		fixed Mac problems with building temporary TextEdit
 *
 * 		Revision 1.4  1992/01/24  17:59:31  lewis
 * 		Added Realize/Unrealize overrides to update widget with values set before we were realized.
 *
 * 		Revision 1.3  1992/01/23  08:14:03  lewis
 * 		Motifoscontrol no longer keeps track of sensative flag while were unrealized.
 *
 * 		Revision 1.1  1992/01/22  15:18:29  sterling
 * 		Initial revision
 *
 *
 *
 */



#include	"OSRenamePre.hh"
#if 	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Label.h>
#include	<Xm/LabelG.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"OSControls.hh"

#include	"TextView.hh"







/*
 ********************************************************************************
 ******************************* AbstractTextView *******************************
 ********************************************************************************
 */
const	Boolean	AbstractTextView::kWordWrapOn = True;

AbstractTextView::AbstractTextView ()
{
}

AbstractTextView::~AbstractTextView ()
{
}

String	AbstractTextView::GetText () const
{
	Invariant ();
	return (GetText_ ());
}

void	AbstractTextView::SetText (const String& text, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (text != GetText ()) {
		SetText_ (text, updateMode);
	}
	Ensure (text == GetText ());
}

void	AbstractTextView::SetText (const char* text, Panel::UpdateMode updateMode)
{
	SetText (String (text), updateMode);
}

AbstractTextView::Justification	AbstractTextView::GetJustification () const
{
	Invariant ();
	return (GetJustification_ ());
}

void	AbstractTextView::SetJustification (Justification justification, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (justification != GetJustification ()) {
		SetJustification_ (justification, updateMode);
	}
	Ensure (justification == GetJustification ());
}

Boolean	AbstractTextView::GetWordWrap () const
{
	Invariant ();
	return (GetWordWrap_ ());
}

void	AbstractTextView::SetWordWrap (Boolean wordWrap, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (wordWrap != GetWordWrap ()) {
		SetWordWrap_ (wordWrap, updateMode);
	}
	Ensure (wordWrap == GetWordWrap ());
}
	
#if		qDebug
void	AbstractTextView::Invariant_ () const
{
	// no data fields, so nothing much to do
}
#endif




// temp hack quicky implemenation of statictext ....
#include "TextEdit.hh"







/*
 ********************************************************************************
 *********************************** TextView ***********************************
 ********************************************************************************
 */

const	Point	kBadTextBox = Point (-1, -1);

TextView::TextView (const String& text, AbstractTextView::Justification justification):
	fText (text),
	fJustification (justification),
	fWordWrap (False)
#if		qXmToolkit
	,fLabelWidget (Nil)
#endif
{
#if		qXmToolkit
	SetMargin (2, 2, eNoUpdate);

	class	MyLabelWidget : public MotifOSControl {
		public:
			MyLabelWidget ():
				MotifOSControl ()
			{
			}

			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
				Arg	arg;
				XtSetArg (arg, XmNsensitive, True);	// otherwise text is gray
				osWidget* w = ::XmCreateLabel (parent, "a label", &arg, 1);
				return (w);
			}	
	};
	AddSubView (fLabelWidget = new MyLabelWidget ());
#endif
}

Point	TextView::CalcDefaultSize_ (const Point& defaultSize) const
{
#if		qMacToolkit
	if (GetWordWrap ()) {
		// This is just a quick hack - OK to addsubview and remove - should cause no flicker - just inval
		// layout, but our layout does nothing!
		TextEdit	te;
		Font	f = GetEffectiveFont ();
		te.SetFont (&f);
		te.SetWordWrap (True, eNoUpdate);
		te.SetJustification (GetJustification (), eNoUpdate);
		te.SetText (GetText (), eNoUpdate);
		te.SetSize (GetSize (), eNoUpdate);
		te.SetEditMode (AbstractTextEdit::eDisplayOnly);
		te.SetBorder (GetBorder (), eNoUpdate);
		te.SetMargin (GetMargin (), eNoUpdate);
		AddSubView (&te);
		te.ProcessLayout ();
		Point size = te.CalcDefaultSize (defaultSize);
		RemoveSubView (&te);
		return (size);
	}
	else {
		const Font&	f = GetEffectiveFont ();
		Point size = Point (f.GetFontHeight (), TextWidth (GetText (), f, kZeroPoint));
		return (size + GetBorder ()*2 + GetMargin ()*2);
	}
#elif	qXmToolkit
	const Font&	f = GetEffectiveFont ();
	Point size = Point (f.GetFontHeight (), TextWidth (GetText (), f, kZeroPoint));
	return (size + GetBorder ()*2 + GetMargin ()*2);
#endif	/*Toolkit*/
}

Boolean	TextView::GetLive () const
{
	return (False);
}
		
void	TextView::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	View::EffectiveFontChanged (newFont, updateMode);

	InvalidateLayout ();					// cuz our layout may change
	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}

#if		qMacToolkit
void	TextView::Draw (const Region& update)
{
	if (GetWordWrap ()) {
		// This is just a quick hack - OK to addsubview and remove - should cause no flicker - just inval
		// layout, but our layout does nothing!
		TextEdit	te;
		Font	f = GetEffectiveFont ();
		te.SetFont (&f);
		te.SetWordWrap (True, eNoUpdate);
		te.SetJustification (GetJustification (), eNoUpdate);
		te.SetText (GetText (), eNoUpdate);
		te.SetSize (GetSize (), eNoUpdate);
		te.SetBorder (GetBorder (), eNoUpdate);
		te.SetMargin (GetMargin (), eNoUpdate);
		te.SetEditMode (AbstractTextEdit::eDisplayOnly);
		AddSubView (&te);
		te.ProcessLayout ();
		te.Render (update);
		RemoveSubView (&te);
	}
	else {
		const Font&	f = GetEffectiveFont ();
		Coordinate	textV = GetBorder ().GetV () + GetMargin ().GetV () +
			(GetSize ().GetV () - (GetBorder ().GetV ()*2 + GetMargin ().GetV ()*2 + f.GetFontHeight ()))/2;
		Coordinate	deltaH = GetBorder ().GetH () + GetMargin ().GetH ();
		
		switch (GetJustification ()) {
			case eJustLeft:
			case eSystemDefined:
				DrawText (GetText (), Point (textV, deltaH));
				break;
		
			case eJustCenter:
				DrawText (GetText (), Point (textV, deltaH + (GetSize ().GetH () - deltaH*2 - TextWidth (GetText (), f, kZeroPoint)) / 2));
				break;
		
			case eJustRight:
				DrawText (GetText (), Point (textV, GetSize ().GetH () - TextWidth (GetText (), f, kZeroPoint) - deltaH));
				break;
		
			case eJustFull:
				// not really implemented right, just fake and use left justification...	
				DrawText (GetText (), Point (textV, deltaH));
				break;	
		
			default:	
				AssertNotReached ();
		}
		DrawBorder ();
	}
}
#endif	/*Toolkit*/

#if		qXmToolkit
void	TextView::Layout ()
{
	AssertNotNil (fLabelWidget);
	Assert (fLabelWidget->GetOrigin () == kZeroPoint);
	fLabelWidget->SetSize (GetSize ());
	View::Layout ();
}
#endif	/*qXmToolkit*/

String	TextView::GetText_ () const
{
	return (fText);
}
		
void	TextView::SetText_ (const String& text, Panel::UpdateMode updateMode)
{
	fText = text;
#if		qXmToolkit
	AssertNotNil (fLabelWidget);
	fLabelWidget->SetStringResourceValue (XmNlabelString, text);
#else
	Refresh (updateMode);
#endif
}

AbstractTextView::Justification	TextView::GetJustification_ () const
{
	return (fJustification);
}

void	TextView::SetJustification_ (AbstractTextView::Justification justification, Panel::UpdateMode updateMode)
{
	fJustification = justification;
#if		qXmToolkit
	if (fLabelWidget->GetOSRepresentation () != Nil) {
		// OK to ignore for now if nil
		switch (fJustification) {
			case eJustLeft:			fLabelWidget->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;
			case eJustCenter:		fLabelWidget->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_CENTER); break;
			case eJustRight:		fLabelWidget->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_END); break;
			case eJustFull:			fLabelWidget->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;
			case eSystemDefined:	fLabelWidget->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;

			default:	RequireNotReached ();
		}
	}
#endif
	Refresh (updateMode);
}

Boolean	TextView::GetWordWrap_ () const
{
	return (fWordWrap);
}

void	TextView::SetWordWrap_ (Boolean wordWrap, Panel::UpdateMode updateMode)
{
	fWordWrap = wordWrap;
	Refresh (updateMode);
}

#if		qXmToolkit
void	TextView::Realize (osWidget* parent)
{
	// Dont call inherited version since we call our childs realize directly...
	RequireNotNil (parent);
	AssertNotNil (fLabelWidget);
	fLabelWidget->Realize (parent);
	SetJustification_ (GetJustification_ (), eNoUpdate);	// called protected version so that real work is always done (no comparison with existing value)
	fLabelWidget->SetStringResourceValue (XmNlabelString, GetText ());
	fLabelWidget->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorder ().GetV (), GetBorder ().GetH ()));
	fLabelWidget->SetDimensionResourceValue (XmNmarginHeight, GetMargin ().GetV ());
    fLabelWidget->SetDimensionResourceValue (XmNmarginWidth, GetMargin ().GetH ());
}

void	TextView::UnRealize ()
{
	AssertNotNil (fLabelWidget);
	fLabelWidget->UnRealize ();
}
#endif	/*qXmToolkit*/

void	TextView::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	View::SetBorder_ (border, eNoUpdate);
#if		qXmToolkit
	if (fLabelWidget != Nil) {
		fLabelWidget->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorder ().GetV (), GetBorder ().GetH ()));
	}
#endif
	Refresh (updateMode);
}

void	TextView::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	View::SetMargin_ (margin, eNoUpdate);
#if		qXmToolkit
	if (fLabelWidget != Nil) {
		fLabelWidget->SetDimensionResourceValue (XmNmarginHeight, GetMargin ().GetV ());
   		fLabelWidget->SetDimensionResourceValue (XmNmarginWidth, GetMargin ().GetH ());
	}
#endif	/*qXmToolkit*/
	Refresh (updateMode);
}




/*
 ********************************************************************************
 ****************************** IOStream Utilities ******************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const AbstractTextView::Justification& a)
{
	switch (a) {
		case TextView::eJustLeft:		out << 'L';	break;
		case TextView::eJustRight:		out << 'R';	break;
		case TextView::eJustCenter:		out << 'C';	break;
		case TextView::eJustFull:		out << 'F';	break;
		case TextView::eSystemDefined:	out << 'S';	break;
		default:	RequireNotReached ();
	}
	return (out);
}

istream&	operator>> (istream& in, AbstractTextView::Justification& a)
{
	char c;
	in >> c;
	switch (c) {
		case 'L':	a = TextView::eJustLeft;		break;
		case 'R':	a = TextView::eJustRight;		break;
		case 'C':	a = TextView::eJustCenter;		break;
		case 'F':	a = TextView::eJustFull;		break;
		case 'S':	a = TextView::eSystemDefined;	break;
		default:	AssertNotReached ();// SHOULD REALLY SET IOS BAD--NOT ASSERT
	}
	return (in);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

