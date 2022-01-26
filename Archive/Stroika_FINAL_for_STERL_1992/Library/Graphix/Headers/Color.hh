/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Color__
#define	__Color__

/*
 * $Header: /fuji/lewis/RCS/Color.hh,v 1.3 1992/11/25 22:38:49 lewis Exp $
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
 *	$Log: Color.hh,v $
 *		Revision 1.3  1992/11/25  22:38:49  lewis
 *		Real->double - real obsolete.
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.9  1992/04/30  13:33:42  sterling
 *		operator* and cyan, magenta, yellow
 *
 *		Revision 1.8  92/04/07  01:00:32  01:00:32  lewis (Lewis Pringle)
 *		Make color compare function return long, instead of short since then we can avoid some overflow probelms.
 *
 *
 *
 *
 */

#include	"Config-Graphix.hh"


typedef	UInt16	ColorComponent;
const	ColorComponent	kMinComponent	=	kMinUInt16;
const	ColorComponent	kMaxComponent	=	kMaxUInt16;

class	Color {
	public:
		Color ();		// all components kMinComponent - needed for allocating arrays...
		Color (ColorComponent red, ColorComponent green, ColorComponent blue);

		/*
		 * Get at values as red/green/blue.
		 */
		nonvirtual	void	GetRGB (ColorComponent& red, ColorComponent& green, ColorComponent& blue) const;
		nonvirtual	void	SetRGB (ColorComponent red, ColorComponent green, ColorComponent blue);

		nonvirtual	ColorComponent	GetRed () const;
		nonvirtual	void			SetRed (ColorComponent red);
		nonvirtual	ColorComponent	GetGreen () const;
		nonvirtual	void			SetGreen (ColorComponent green);
		nonvirtual	ColorComponent	GetBlue () const;
		nonvirtual	void			SetBlue (ColorComponent blue);

		/*
		 * Other color space representations.
		 */
		nonvirtual	void	GetCMY (ColorComponent& cyan, ColorComponent& magenta, ColorComponent& yellow) const;
		nonvirtual	void	SetCMY (ColorComponent cyan, ColorComponent magenta, ColorComponent yellow);
		nonvirtual	void	GetHLS (ColorComponent& hue, ColorComponent& lightness, ColorComponent& saturation) const;
		nonvirtual	void	SetHLS (ColorComponent hue, ColorComponent lightness, ColorComponent saturation);
		nonvirtual	void	GetHSV (ColorComponent& hue, ColorComponent& saturation, ColorComponent& value) const;
		nonvirtual	void	SetHSV (ColorComponent hue, ColorComponent saturation, ColorComponent value);

	private:
		ColorComponent	fRed;
		ColorComponent	fGreen;
		ColorComponent	fBlue;
};

Color	operator* (Color color, double scale);
Color	operator* (double scale, Color color);


extern	const	Color	kBlackColor;
extern	const	Color	kWhiteColor;
extern	const	Color	kRedColor;
extern	const	Color	kGreenColor;
extern	const	Color	kBlueColor;
extern	const	Color	kCyanColor;
extern	const	Color	kMagentaColor;
extern	const	Color	kYellowColor;
extern	const	Color	kLightGrayColor;
extern	const	Color	kGrayColor;
extern	const	Color	kDarkGrayColor;



// Euclidean distance between colors
extern	UInt32	EuclideanRGBDistance (const Color& lhs, const Color& rhs);



/*
 * Stream inserters and extractors.
 */
extern	class ostream&	operator<< (class ostream& out, const Color& c);
extern	class istream&	operator>> (class istream& in, Color& c);




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	ColorComponent	Color::GetRed () const					{ return (fRed); }
inline	void			Color::SetRed (ColorComponent red)		{ fRed = red; }
inline	ColorComponent	Color::GetGreen () const				{ return (fGreen); }
inline	void			Color::SetGreen (ColorComponent green)	{ fGreen = green; }
inline	ColorComponent	Color::GetBlue () const					{ return (fBlue); }
inline	void			Color::SetBlue (ColorComponent blue)	{ fBlue = blue; }


inline	Boolean	operator== (const Color& lhs, const Color& rhs)
{
	return Boolean ((lhs.GetRed () == rhs.GetRed ()) and (lhs.GetGreen () == rhs.GetGreen ()) and (lhs.GetBlue () == rhs.GetBlue ()));
}

inline	Boolean	operator!= (const Color& lhs, const Color& rhs)
{
	return Boolean ((lhs.GetRed () != rhs.GetRed ()) or (lhs.GetGreen () != rhs.GetGreen ()) or (lhs.GetBlue () != rhs.GetBlue ()));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Color__*/
