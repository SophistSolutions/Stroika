/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__Math__
#define		__Math__

/*
 * $Header: /fuji/lewis/RCS/Math.hh,v 1.10 1992/11/21 09:02:19 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Math.hh,v $
 *		Revision 1.10  1992/11/21  09:02:19  lewis
 *		With kdjs help, I finally got RoundUpTo() and RoundDownTo() correct!
 *
 *		Revision 1.9  1992/11/12  08:06:39  lewis
 *		Comment out Real - based stuff - kBadNumber, and IsBadNumber().
 *		Stuff not portably - talk with sterl about it, but I don't think
 *		we should support this.
 *
 *		Revision 1.7  1992/09/21  05:46:49  sterling
 *		Fixed RoundUpTo/RoundDownTo () yet again (really just simplified).
 *
 *		Revision 1.6  1992/09/15  03:54:46  lewis
 *		Tried to further make RoundDownTo() closer to correct. Agh!
 *
 *		Revision 1.5  1992/09/11  13:00:08  lewis
 *		Yet again re-implement RoundUpTo/RoundDownTo (). No more confidence
 *		this time, but I did do a testsuite, and it seems to pass....
 *
 *		Revision 1.3  1992/07/21  19:09:10  sterling
 *		Fixed RoundUpTo () - needed to add back original value.
 *
 *		Revision 1.2  1992/07/07  22:46:11  lewis
 *		Added RoundUpTo () and overloaded PinValue () to take unsigned (as
 *		well as int) Added more precise defintion of kPi.
 *
 *		Revision 1.11  92/05/21  17:09:52  17:09:52  sterling (Sterling Wight)
 *		Lewis/Sterl marathon rewrite of UInt64 - for now comment out Int64.
 *		
 *		Revision 1.10  92/04/16  09:44:53  09:44:53  lewis (Lewis Pringle)
 *		Moved Random stuff to Ranndom.hh
 *		
 *		Revision 1.9  92/04/15  13:54:02  13:54:02  lewis (Lewis Pringle)
 *		Add random number facility (closely based on ANSI rand()).
 *		
 *		Revision 1.7  1992/03/11  22:58:22  lewis
 *		Use new conditional compile directive qLongLongAvailable instead
 *		of qGPlus.
 *
 *
 */

#include	<math.h>

#include	"Config-Foundation.hh"
#include	"Exception.hh"



//extern	const	Real	kBadNumber;
const	double			kPi				=	3.14159265358979;	// Best I can do from memory
const	double			kEulersNumber	=	2.71;				// Best I can do from memory


//extern	Boolean	IsBadNumber (const double& r);


extern	Exception	gMathException;		// need to differentiate kinds of exceptions
										// and to catch SANE exceptions, and generate Stroika ones...


extern	Exception	gMathRangeException;	// is a math exception








// 64bit integers
#if		qLongLongAvailable
typedef	unsigned long long	UInt64;
typedef	long long			Int64;
#else	/*qLongLongAvailable*/
class UInt64 {
	public:
		UInt64 (UInt32 it);		
		UInt64 (const UInt64& from);
		nonvirtual	const UInt64&	operator= (const UInt64& from);


		nonvirtual const	UInt64&	operator<<= (unsigned by);
		nonvirtual const	UInt64&	operator>>= (unsigned by);
		nonvirtual const	UInt64&	operator+= 	(const UInt64& right);
		nonvirtual const	UInt64&	operator-= 	(const UInt64& right);
		nonvirtual const	UInt64&	operator*= 	(const UInt64& right);
		nonvirtual const	UInt64&	operator/= 	(const UInt64& right);
		nonvirtual const	UInt64&	operator%=	(const UInt64& right);

		nonvirtual	UInt32	Low32 () const;

	private:
		UInt16 fDigit[4];

	friend	Boolean		operator==	(const UInt64& left, const UInt64& right);
	friend	Boolean		operator<	(const UInt64& left, const UInt64& right);		
	class ostream;
	friend	ostream&	operator<< (ostream& out, UInt64 s);
};

extern	Boolean	operator==	(const UInt64& left, const UInt64& right);
extern	Boolean	operator!= (const UInt64 left, const UInt64 right);
extern 	Boolean	operator<	(const UInt64& left, const UInt64& right);
extern	Boolean	operator<= (const UInt64 left, const UInt64 right);
extern	Boolean	operator>  (const UInt64 left, const UInt64 right);
extern	Boolean	operator>= (const UInt64 left, const UInt64 right);

extern	UInt64	operator<< (const UInt64 left, unsigned by);
extern	UInt64	operator>> (const UInt64 left, unsigned by);
extern	UInt64	operator+ (const UInt64 left, const UInt64 right);
extern	UInt64	operator- (const UInt64 left, const UInt64 right);
extern	UInt64	operator* (const UInt64 left, const UInt64 right);
extern	UInt64	operator/ (const UInt64 left, const UInt64 right);
extern	UInt64	operator% (const UInt64 left, const UInt64 right);



/*
 * Stream inserters and extractors.
 */
class	istream;
class	ostream;
extern	ostream&	operator<< (ostream& out, UInt64 s);
extern	istream&	operator>> (istream& in, UInt64& s);




#if 0
// til we get it working..

class Int64 {
	public:
		Int64 (Int32 it)
			{
				fDigit[3] = fDigit[2] = (it < 0) ? 0xffff : 0;
				fDigit[1] = UInt16(it >> 16);
				fDigit[0] = UInt16(it);
			}
		Int64 (Int16 it)
			{
				fDigit[3] = fDigit[2] = fDigit[1] = (it < 0) ? 0xffff : 0;
				fDigit[0] = it;
			}
#if		qSignedTypeImplemented
		Int64 (signed char it)
			{
				fDigit[3] = fDigit[2] = fDigit[1] = (it < 0 ) ? 0xffff : 0;
				fDigit[0] = it;
			}
#endif	/*qSignedTypeImplemented*/
		Boolean IsMinus() const			{ return ((fDigit[3] & 0x8000) ? True : False); }
		Boolean	IsZero () const;


		nonvirtual Int64    operator-	() const;

		nonvirtual Int64	operator+= 	(const Int64& right);
		nonvirtual Int64	operator-= 	(const Int64& right);
		nonvirtual Int64	operator*= 	(const Int64& right);
		nonvirtual Int64	operator/= 	(const Int64& right);
		nonvirtual Int64	operator%=	(const Int64& rihgt);

		nonvirtual Boolean	operator==	(const Int64& right) const;
		nonvirtual Boolean	operator<	(const Int64& right) const;

	private:
		UInt16 fDigit[4];
};

extern	Int64	operator+ (const Int64 left, const Int64 right);
extern	Int64	operator- (const Int64 left, const Int64 right);
extern	Int64	operator* (const Int64 left, const Int64 right);
extern	Int64	operator/ (const Int64 left, const Int64 right);
extern	Int64	operator% (const Int64 left, const Int64 right);

extern	Boolean	operator!= (const Int64 left, const Int64 right);
extern	Boolean	operator<= (const Int64 left, const Int64 right);
extern	Boolean	operator>  (const Int64 left, const Int64 right);
extern	Boolean	operator>= (const Int64 left, const Int64 right);

/*
   Stream inserters and extractors.
 */
class ostream&	operator<< (class ostream& out, const Int64 s);
class istream&	operator>> (class istream& in, Int64 s);
#endif

#endif	/*qLongLongAvailable*/










/* TWO arg guys like min/max are insane to do this way!!! How? */
/* REALLY NEED PARAMTYPES!!! */

inline	char		Abs (char a)			{ return ((a>0)? a: -a); }
inline	short		Abs (short a)			{ return ((a>0)? a: -a); }
inline	int			Abs (int a)				{ return ((a>0)? a: -a); }
inline	long		Abs (long a)			{ return ((a>0)? a: -a); }
inline	float		Abs (float a)			{ return ((a>0)? a: -a); }
inline	double		Abs (double a)			{ return ((a>0)? a: -a); }
inline	long double	Abs (long double a)		{ return ((a>0)? a: -a); }

inline	Boolean	Odd (char x)			{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (unsigned char x)	{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (short x)			{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (unsigned short x)	{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (int x)				{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (unsigned int x)	{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (long x)			{ return (Boolean (!!(x & 1))); }
inline	Boolean	Odd (unsigned long x)	{ return (Boolean (!!(x & 1))); }

inline	Boolean	Even (char x)			{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (unsigned char x)	{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (short x)			{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (unsigned short x)	{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (int x)			{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (unsigned int x)	{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (long x)			{ return (Boolean (!(x & 1))); }
inline	Boolean	Even (unsigned long x)	{ return (Boolean (!(x & 1))); }

inline	char			Min (char lhs, char rhs)						{ return ((lhs<rhs)? lhs: rhs); }
inline	unsigned char	Min (unsigned char lhs, unsigned char rhs)		{ return ((lhs<rhs)? lhs: rhs); }
inline	short			Min (short lhs, short rhs)						{ return ((lhs<rhs)? lhs: rhs); }
inline	unsigned short	Min (unsigned short lhs, unsigned short rhs)	{ return ((lhs<rhs)? lhs: rhs); }
inline	int				Min (int lhs, int rhs)							{ return ((lhs<rhs)? lhs: rhs); }
inline	unsigned int	Min (unsigned int lhs, unsigned int rhs)		{ return ((lhs<rhs)? lhs: rhs); }
inline	long			Min (long lhs, long rhs)						{ return ((lhs<rhs)? lhs: rhs); }
inline	unsigned long	Min (unsigned long lhs, unsigned long rhs)		{ return ((lhs<rhs)? lhs: rhs); }
inline	long double		Min (long double lhs, long double rhs)			{ return ((lhs<rhs)? lhs: rhs); }

inline	char			Max (char lhs, char rhs)						{ return ((lhs>rhs)? lhs: rhs); }
inline	unsigned char	Max (unsigned char lhs, unsigned char rhs)		{ return ((lhs>rhs)? lhs: rhs); }
inline	short			Max (short lhs, short rhs)						{ return ((lhs>rhs)? lhs: rhs); }
inline	unsigned short	Max (unsigned short lhs, unsigned short rhs)	{ return ((lhs>rhs)? lhs: rhs); }
inline	int				Max (int lhs, int rhs)							{ return ((lhs>rhs)? lhs: rhs); }
inline	unsigned int	Max (unsigned int lhs, unsigned int rhs)		{ return ((lhs>rhs)? lhs: rhs); }
inline	long			Max (long lhs, long rhs)						{ return ((lhs>rhs)? lhs: rhs); }
inline	unsigned long	Max (unsigned long lhs, unsigned long rhs)		{ return ((lhs>rhs)? lhs: rhs); }
inline	long double		Max (long double lhs, long double rhs)			{ return ((lhs>rhs)? lhs: rhs); }


inline	int			PinValue (int min, int max, int value)					{ return (Min (Max (value, min), max)); }
inline	unsigned	PinValue (unsigned min, unsigned max, unsigned value)	{ return (Min (Max (value, min), max)); }


/*
 * RoundUpTo() - round towards posative infinity.
 * RoundDownTo() - round towards negative infinity.
 */
inline	int			RoundUpTo (unsigned x, unsigned toNearest)		{ return (((x+toNearest-1u)/toNearest)*toNearest); }
inline	int			RoundDownTo (unsigned x, unsigned toNearest)	{ return ((x/toNearest)*toNearest); }

inline	int			RoundUpTo (int x, unsigned toNearest)
{
	if (x < 0) {
		return (- RoundDownTo (unsigned (-x), toNearest));
	}
	else {
		return (RoundUpTo (unsigned (x), toNearest));
	}
}
inline	int			RoundDownTo (int x, unsigned toNearest)			
{ 
	if (x < 0) {
		return (- RoundUpTo (unsigned (-x), toNearest));
	}
	else {
		return (RoundDownTo (unsigned (x), toNearest));
	}
}



// UInt64 inlines
inline	UInt64::UInt64 (UInt32 it)
{
	fDigit[3] = fDigit[2] = 0;
	fDigit[1] = UInt16(it >> 16);
	fDigit[0] = UInt16(it);
}
		
inline	UInt64::UInt64 (const UInt64& from)
{
	fDigit[0] = from.fDigit[0];
	fDigit[1] = from.fDigit[1];
	fDigit[2] = from.fDigit[2];
	fDigit[3] = from.fDigit[3];
}

inline	const UInt64&	UInt64::operator= (const UInt64& from)
{
	fDigit[0] = from.fDigit[0];
	fDigit[1] = from.fDigit[1];
	fDigit[2] = from.fDigit[2];
	fDigit[3] = from.fDigit[3];
	return (*this);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif		/*__Math__*/
