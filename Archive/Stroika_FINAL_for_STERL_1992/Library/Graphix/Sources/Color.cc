/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Color.cc,v 1.4 1992/11/25 22:38:49 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Color.cc,v $
 *		Revision 1.4  1992/11/25  22:38:49  lewis
 *		Real->double - real obsolete.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/02  03:39:46  lewis
 *		Include Math.hh since no longer pulled in autoamatically/indirectly/
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.15  1992/05/18  16:48:13  lewis
 *		Cleanup compiler warnings.
 *
 *		Revision 1.10  92/04/07  01:01:13  01:01:13  lewis (Lewis Pringle)
 *		Make color compare function return long, instead of short since then we can avoid some overflow problems.
 *		
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Picker.h>
#endif /*qMacGDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Math.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"Color.hh"






/*
 ********************************************************************************
 ************************************* Color ************************************
 ********************************************************************************
 */

Color::Color ():
	fRed (kMinComponent),
	fGreen (kMinComponent),
	fBlue (kMinComponent)
{
}

Color::Color (ColorComponent red, ColorComponent green, ColorComponent blue):
	fRed (red),
	fGreen (green),
	fBlue (blue)
{
}

void	Color::GetRGB (ColorComponent& red, ColorComponent& green, ColorComponent& blue) const
{
	red = fRed;
	green = fGreen;
	blue = fBlue;
}

void	Color::SetRGB (ColorComponent red, ColorComponent green, ColorComponent blue)
{
	fRed = red;
	fGreen = green;
	fBlue = blue;
}

void	Color::GetCMY (ColorComponent& cyan, ColorComponent& magenta, ColorComponent& yellow) const
{
#if		qMacGDI
	CMYColor	cymColor;
	RGBColor	rgbColor;

	GetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
	Assert (GDIConfiguration ().ColorQDAvailable ());
	::RGB2CMY (&rgbColor, &cymColor);

	cyan = cymColor.cyan;
	magenta = cymColor.magenta;
	yellow = cymColor.yellow;
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Color::SetCMY (ColorComponent cyan, ColorComponent magenta, ColorComponent yellow)
{
#if		qMacGDI
	CMYColor	cymColor;
	RGBColor	rgbColor;

	cymColor.cyan = cyan;
	cymColor.magenta = magenta;
	cymColor.yellow = yellow;

	Assert (GDIConfiguration ().ColorQDAvailable ());
	::CMY2RGB (&cymColor, &rgbColor);
	SetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Color::GetHLS (ColorComponent& hue, ColorComponent& lightness, ColorComponent& saturation) const
{
#if		qMacGDI
	HSLColor	hslColor;
	RGBColor	rgbColor;

	GetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
	Assert (GDIConfiguration ().ColorQDAvailable ());
	::RGB2HSL (&rgbColor, &hslColor);

	hue = hslColor.hue;
	saturation = hslColor.saturation;
	lightness = hslColor.lightness;
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Color::SetHLS (ColorComponent hue, ColorComponent lightness, ColorComponent saturation)
{
#if		qMacGDI
	HSLColor	hslColor;
	RGBColor	rgbColor;

	hslColor.hue = hue;
	hslColor.saturation = saturation;
	hslColor.lightness = lightness;

	Assert (GDIConfiguration ().ColorQDAvailable ());
	::HSL2RGB (&hslColor, &rgbColor);
	SetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Color::GetHSV (ColorComponent& hue, ColorComponent& saturation, ColorComponent& value) const
{
#if		qMacGDI
	HSVColor	hsvColor;
	RGBColor	rgbColor;

	GetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
	Assert (GDIConfiguration ().ColorQDAvailable ());
	::RGB2HSV (&rgbColor, &hsvColor);

	hue = hsvColor.hue;
	saturation = hsvColor.saturation;
	value = hsvColor.value;
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Color::SetHSV (ColorComponent hue, ColorComponent saturation, ColorComponent value)
{
#if		qMacGDI
	HSVColor	hsvColor;
	RGBColor	rgbColor;

	hsvColor.hue = hue;
	hsvColor.saturation = saturation;
	hsvColor.value = value;

	Assert (GDIConfiguration ().ColorQDAvailable ());
	::HSV2RGB (&hsvColor, &rgbColor);
	SetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
#else
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}




/*
 ********************************************************************************
 ************************************* operator* ********************************
 ********************************************************************************
 */


Color	operator* (Color color, double scale)
{
	long double	maxComponent = kMaxComponent;
	long double	minComponent = kMinComponent;
	
	ColorComponent	red   = ColorComponent (Max (Min (color.GetRed ()   * scale, maxComponent), minComponent));
	ColorComponent	green = ColorComponent (Max (Min (color.GetGreen () * scale, maxComponent), minComponent));
	ColorComponent	blue  = ColorComponent (Max (Min (color.GetBlue ()  * scale, maxComponent), minComponent));

	return (Color (red, green, blue));
}

Color	operator* (double scale, Color color)
{
	long double	maxComponent = kMaxComponent;
	long double	minComponent = kMinComponent;
	
	ColorComponent	red   = ColorComponent (Max (Min (color.GetRed ()   * scale, maxComponent), minComponent));
	ColorComponent	green = ColorComponent (Max (Min (color.GetGreen () * scale, maxComponent), minComponent));
	ColorComponent	blue  = ColorComponent (Max (Min (color.GetBlue ()  * scale, maxComponent), minComponent));

	return (Color (red, green, blue));
}




/*
 ********************************************************************************
 **************************** Distace between colors  ***************************
 ********************************************************************************
 */
UInt32	EuclideanRGBDistance (const Color& lhs, const Color& rhs)
{
	Require (sizeof (UInt32) > sizeof (ColorComponent));	// otherwise we might overflow
	double d = 0;
	d += double (rhs.GetRed () - lhs.GetRed ()) * double (rhs.GetRed () - lhs.GetRed ());
	d += double (rhs.GetGreen () - lhs.GetGreen ()) * double (rhs.GetGreen () - lhs.GetGreen ());
	d += double (rhs.GetBlue () - lhs.GetBlue ()) * double (rhs.GetBlue () - lhs.GetBlue ());
	d = sqrt (d);

// maybe still test for overflow of UInt32 - cannot happen???
//	if (d > kMaxComponent) {
//		d = kMaxComponent;
//	}
	return (UInt32 (d));
}







/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */

ostream&	operator<< (ostream& out, const Color& c)
{
	ColorComponent	red;
	ColorComponent	green;
	ColorComponent	blue;
	c.GetRGB (red, green, blue);
	out << lparen << red << ' ' << green << ' ' << blue << rparen;
	return (out);
}

istream&	operator>> (istream& in, Color& c)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	ColorComponent	red;
	ColorComponent	green;
	ColorComponent	blue;
	in >> red >> green >> blue;

	in >> ch;
	if (!in or (ch != rparen)) {		// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	/*
	 * We win.
	 */
	Assert (in);
	c = Color (red, green, blue);
	return (in);

}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

