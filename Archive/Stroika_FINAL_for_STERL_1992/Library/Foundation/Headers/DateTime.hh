/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DateTime__
#define	__DateTime__

/*
 * $Header: /fuji/lewis/RCS/DateTime.hh,v 1.4 1992/11/19 05:02:52 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: DateTime.hh,v $
 *		Revision 1.4  1992/11/19  05:02:52  lewis
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/07/16  06:00:24  lewis
 *		Added accessors for date and time fields.
 *
 *
 */

#include	"Date.hh"
#include	"Time.hh"


class	DateTime {
	public:
		DateTime ();
		DateTime (const Date& date);
		DateTime (const Time& time);
		DateTime (const Date& date, const Time& time);

		nonvirtual	Date	GetDate () const;
		nonvirtual	Time	GetTime () const;

	private:
		Date	fDate;
		Time	fTime;
};





/*
 * Arithmatic
 */
//extern	Date	operator+ (const Date& lhs, const Date& rhs);
//extern	Date	operator- (const Date& lhs, const Date& rhs);


/*
 * Comparisons
 */
extern	Boolean	operator== (const DateTime& lhs, const DateTime& rhs);
extern	Boolean	operator!= (const DateTime& lhs, const DateTime& rhs);
extern	Boolean	operator<  (const DateTime& lhs, const DateTime& rhs);
extern	Boolean	operator<= (const DateTime& lhs, const DateTime& rhs);
extern	Boolean	operator>  (const DateTime& lhs, const DateTime& rhs);
extern	Boolean	operator>= (const DateTime& lhs, const DateTime& rhs);


extern	DateTime	Min (const DateTime& d1, const DateTime& d2);
extern	DateTime	Max (const DateTime& d1, const DateTime& d2);


/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const DateTime& dateTime);
extern	istream&	operator>> (istream& in, DateTime& dateTime);


extern	DateTime	Now ();




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Date	DateTime::GetDate () const { return (fDate); }
inline	Time	DateTime::GetTime () const { return (fTime); }


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__DateTime__*/
