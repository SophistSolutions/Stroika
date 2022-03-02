/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__NumberText__
#define	__NumberText__

/*
 * $Header: /fuji/lewis/RCS/NumberText.hh,v 1.3 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NumberText.hh,v $
 *		Revision 1.3  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.14  1992/06/09  15:50:11  sterling
 *		made GetValue be const
 *
 *		Revision 1.13  1992/05/19  14:44:20  lewis
 *		Get rid of enum NumberBase - just make a typedef.
 *
 *		Revision 1.12  92/03/10  00:35:39  00:35:39  lewis (Lewis Pringle)
 *		Also, change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *		
 *		Revision 1.11  1992/03/05  21:46:06  sterling
 *		changed validateion code
 *
 *		Revision 1.10  1992/01/29  05:30:07  sterling
 *		eliminated typedef inherited to compiler bugs
 *
 *		Revision 1.8  1992/01/24  23:42:33  lewis
 *		Removed unessary overides, and fValue field. GetValue no longer does validation testing. No longer
 *		provide number conversion routines - use Format.hh
 *
 *
 *
 *
 */

#include "Math.hh"

#include "TextEdit.hh"


class	NumberText : public TextEdit {
	public:
		typedef	unsigned	NumberBase;
		
		NumberText (TextController* controller = Nil, Real start = kBadNumber, Real minValue = kBadNumber, 
					Real maxValue = kBadNumber, Int8 precision = 0, NumberBase base = 10);

		nonvirtual	Real	GetValue () const;
		virtual		void	SetValue (Real value, UpdateMode updateMode = eDelayedUpdate);
		
		nonvirtual	Real	GetMinValue () const;
		virtual		void	SetMinValue (Real value);
		
		nonvirtual	Real	GetMaxValue () const;
		virtual		void	SetMaxValue (Real value);
		
		nonvirtual	Int8	GetPrecision () const;
		virtual		void	SetPrecision (Int8 precision);
		
		nonvirtual	NumberBase	GetBase () const;
		virtual		void		SetBase (NumberBase base);

		virtual	Real	StringToNumber (const String& s) const;
		virtual	String	NumberToString (Real number) const;

	protected:
		override	void	Validate ();
		override	Boolean	ValidateKey (const KeyStroke& keyStroke);
		
		virtual	Boolean	NumberValid (Real number) const;

	private:
//		typedef	TextEdit	inherited;

	private:
		Real		fMinValue;
		Real		fMaxValue;
		Int8		fPrecision;
		NumberBase	fBase;
};



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif /* _NumberText__ */

