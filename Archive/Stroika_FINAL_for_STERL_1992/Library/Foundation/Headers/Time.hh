/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Time__
#define	__Time__

/*
 * $Header: /fuji/lewis/RCS/Time.hh,v 1.8 1992/11/12 08:14:04 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Time.hh,v $
 *		Revision 1.8  1992/11/12  08:14:04  lewis
 *		Use double instead of Real.
 *
 *		Revision 1.7  1992/10/09  18:35:47  lewis
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/09/25  21:10:38  lewis
 *		Get rid of Declares/Implements for containers of Time - no longer
 *		needed.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  16:52:49  lewis
 *		Got rid of AbSequence.
 *
 *		Revision 1.3  1992/09/01  15:20:18  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/08  00:28:49  lewis
 *		Sterls new Real - based Time implementation. Also, scoped global
 *		constants for kNSeconds, etc...
 *
 *		Revision 1.9  1992/03/30  13:43:15  lewis
 *		Put in ifdefs for macro based template hacks.
 *
 *		Revision 1.8  92/03/22  16:37:43  16:37:43  lewis (Lewis Pringle)
 *		Add inserter/extractor for Time.
 *		
 *		Revision 1.6  1992/01/03  07:04:29  lewis
 *		Get rid of default param of zero as second arg to time ctor as it
 *		tended to cause ambiguity. Not really sure why - perhaps compiler
 *		bug with 2.1 - but not worth fighting right now.
 *
 *
 */

#include	"Config-Foundation.hh"



class	Time {
	public:
		Time ();
		Time (const Time& from);
		Time (double seconds);

		nonvirtual	const Time& operator= (const Time& rhs);

		nonvirtual	double	GetSeconds () const;

		nonvirtual	Time&	operator+= (const Time& t);
		nonvirtual	Time&	operator-= (const Time& t);
		nonvirtual	Time&	operator*= (double r);
		nonvirtual	Time&	operator/= (double r);

	private:
		double	fSeconds;

	//friends:
		friend	Time	operator+ (const Time& lhs, const Time& rhs);
		friend	Time	operator- (const Time& lhs, const Time& rhs);
		friend	Time	operator* (const Time& lhs, double rhs);
		friend	Time	operator* (double lhs, const Time& rhs);
		friend	Time	operator/ (const Time& lhs, double rhs);

		friend	Boolean	operator== (const Time& lhs, const Time& rhs);
		friend	Boolean	operator!= (const Time& lhs, const Time& rhs);
		friend	Boolean	operator<  (const Time& lhs, const Time& rhs);
		friend	Boolean	operator<= (const Time& lhs, const Time& rhs);
		friend	Boolean	operator>  (const Time& lhs, const Time& rhs);
		friend	Boolean	operator>= (const Time& lhs, const Time& rhs);
};




/*
 * Arithmetic
 */
Time	operator+ (const Time& lhs, const Time& rhs);
Time	operator- (const Time& lhs, const Time& rhs);
Time	operator* (const Time& lhs, double rhs);
Time	operator* (double lhs, const Time& rhs);
Time	operator/ (const Time& lhs, double rhs);


/*
 * Comparisons
 */
Boolean	operator== (const Time& lhs, const Time& rhs);
Boolean	operator!= (const Time& lhs, const Time& rhs);
Boolean	operator<  (const Time& lhs, const Time& rhs);
Boolean	operator<= (const Time& lhs, const Time& rhs);
Boolean	operator>  (const Time& lhs, const Time& rhs);
Boolean	operator>= (const Time& lhs, const Time& rhs);


/*
 * Min/Max
 */
Time	Min (const Time& t1, const Time& t2);
Time	Max (const Time& t1, const Time& t2);



/*
 * Stream inserters and extractors.
 */
class	istream;
class	ostream;
ostream&	operator<< (ostream& out, const Time& t);
istream&	operator>> (istream& in, Time& t);


/*
 * GetCurrentTime () is for making relative time measurements, not for keeping and absolute
 * notion of time (for that use the class DateTime).
 *
 * Time since Midnight, January 1, 0000 (if its good enuf for God, its good enuf for Stroika).
 *
 *
 */
Time	GetCurrentTime ();



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
	inline	Time::Time () :
		fSeconds (0)
	{
	}
	
	inline	Time::Time (const Time& from) :
		fSeconds (from.fSeconds)
	{
	}
	
	inline	Time::Time (double seconds) :
		fSeconds (seconds)
	{
	}
	
	inline const Time& Time::operator= (const Time& rhs)
	{
		fSeconds = rhs.fSeconds;
		return (*this);
	}
	
	inline	double	Time::GetSeconds () const
	{
		return (fSeconds);
	}
	
	inline	Time&	Time::operator+= (const Time& t)
	{
		fSeconds += t.fSeconds;
		return (*this);
	}
	
	inline	Time&	Time::operator-= (const Time& t)
	{
		fSeconds -= t.fSeconds;
		return (*this);
	}
	
	inline	Time&	Time::operator*= (double r)
	{
		fSeconds *= r;
		return (*this);
	}
	
	inline	Time&	Time::operator/= (double r)
	{
		fSeconds /= r;
		return (*this);
	}
	
	inline	Boolean	operator== (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds == rhs.fSeconds));
	}
	
	inline	Boolean	operator!= (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds != rhs.fSeconds));
	}
	
	inline	Boolean	operator<  (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds < rhs.fSeconds));
	}
	
	inline	Boolean	operator<= (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds <= rhs.fSeconds));
	}
	
	inline	Boolean	operator>  (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds > rhs.fSeconds));
	}
	
	inline	Boolean	operator>= (const Time& lhs, const Time& rhs)
	{
		return (Boolean (lhs.fSeconds >= rhs.fSeconds));
	}
	
	inline	Time	Min (const Time& t1, const Time& t2)
	{
		return ((t1 <= t2) ? t1 : t2);
	}
	
	inline	Time	Max (const Time& t1, const Time& t2)
	{
		return ((t1 >= t2) ? t1 : t2);
	}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Time__*/
