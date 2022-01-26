/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Axis__
#define	__Axis__

/*
 * $Header: /fuji/lewis/RCS/Axis.hh,v 1.4 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *		Graph utility classes.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Axis.hh,v $
 *		Revision 1.4  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  02:22:56  lewis
 *		Put in conditional definiiotn of String containers.
 *
 *		Revision 1.2  1992/06/25  09:15:26  sterling
 *		Renamed Orientation to AxisOrientation to avoid name conflict (only cuz broken CFront 21 compiler
 *		doesnt scope names properly).
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/15  13:14:42  sterling
 *		Initial revision
 *
 *		
 *
 */

#include	"TextView.hh"
#include	"View.hh"


class	GraphTitle : public TextView {
	public:
		GraphTitle (const String& title);

		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title, UpdateMode updateMode = eDelayedUpdate);
};


class Axis : public View {
	public:
		enum AxisOrientation { eVertical, eHorizontal};
		Axis (const String& label, AxisOrientation orientation, 
			  const Real& minValue, const Real& maxValue,
			  Int16 ticks = 10, Int16 subTicks = 1);

		override		void	Draw (const Region& update);

		nonvirtual	String	GetLabel () const;
		virtual		void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	Real	GetMin () const;
		virtual		void	SetMin (const Real& newMin, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	Real	GetMax () const;
		virtual		void	SetMax (const Real& newMax, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	UInt8	GetTicks () const;
		virtual		void	SetTicks (UInt8 ticks, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	UInt8	GetSubTicks () const;
		virtual		void	SetSubTicks (UInt8 subTicks, UpdateMode updateMode = eDelayedUpdate);
		
		virtual	Real		CoordinateToValue (Coordinate coordinate);
		virtual	Coordinate	ValueToCoordinate (const Real& value);

		nonvirtual	Rect	GetTickRect () const;
		nonvirtual	Rect	GetTickLabelRect () const;
		nonvirtual	Rect	GetLabelRect () const;

		nonvirtual	Axis::AxisOrientation	GetOrientation () const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

		virtual		String	ValueToString (const Real& value) const;

	private:
		AxisOrientation	fOrientation;
		Real		fMin;
		Real		fMax;
		UInt8		fTicks;
		UInt8		fSubTicks;

		Rect		fTickRect;
		Rect		fTickLabelRect;
		Rect		fLabelRect;

		String		fLabel;

		static	const	Int16	kTickLength;
		static	const	Int16	kSubTickLength;

		virtual		void	DrawTicks ();
		virtual		void	DrawTickLabels ();
		virtual		void	DrawLabel ();
		virtual		void	CalcRects (const Point& size);

		nonvirtual	Coordinate	GetCoordinateOfMin ();
		nonvirtual	Coordinate	GetCoordinateOfMax ();
};


#if		!qRealTemplatesAvailable
	// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Array.hh"
	#include	"Collection.hh"
	#include	"DoubleLinkList.hh"
	#include	"Sequence.hh"
	#include	"Sequence_Array.hh"
	#include	"Sequence.hh"
	#include	"Set_Array.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/

	#ifndef		_ContainersOfRealDeclared_
		#define		_ContainersOfRealDeclared_
		Declare (Iterator, Real);
		Declare (Collection, Real);
		Declare (AbSequence, Real);
		Declare (Array, Real);
		Declare (Sequence_Array, Real);
		Declare (Sequence, Real);
	#endif		/*_ContainersOfRealDeclared_*/
#endif		/*!qRealTemplatesAvailable*/


class	LabelledAxis : public Axis {
	public:
		LabelledAxis (const String& label, AxisOrientation orientation, 
			  const Real& minValue, const Real& maxValue,
			  Int16 ticks = 10, Int16 subTicks = 1);

		override	void	SetTicks (UInt8 ticks, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	void	SetTickLabel (UInt8 tick, const String& label, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	String	GetTickLabel (UInt8 tick);

	private:
		override	void	DrawTickLabels ();

		Sequence(String)	fLabels;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __Axis__ */
