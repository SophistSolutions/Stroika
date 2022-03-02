/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Date.cc,v 1.4 1992/12/03 07:09:48 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Date.cc,v $
 *		Revision 1.4  1992/12/03  07:09:48  lewis
 *		Work around qGCC_InserterToSubclassOfOStreamBug.
 *
 *		Revision 1.3  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/06/20  19:27:35  lewis
 *		Be more careful about nested class names etc - cuz broke gcc - hope fix here doesnt break
 *		cfront 2.1 based compilers - add ifdefs if it does...
 *
 *		Revision 1.6  1992/03/11  23:06:52  lewis
 *		Got rid of old support for qGPlus.
 *
 *
 */



#include	<time.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Date.hh"








/*
 ********************************************************************************
 ************************************** Date ************************************
 ********************************************************************************
 */

Date::Date ():
	fDateRep (0)
{
	time_t	clk = time (0);
	const tm* now = localtime (&clk);
	fDateRep = jday (now->tm_mon+1, now->tm_mday, now->tm_year+1900);
}

Date::Date (Int32 dayCount):
	fDateRep (dayCount)
{
}

Date::Date (DayOfMonth dayOfMonth, MonthOfYear monthOfYear, Year year):
	fDateRep (0)
{
	fDateRep = jday (monthOfYear, dayOfMonth, year);
}

/*
 * Convert Gregorian calendar date to the corresponding Julian day number
 * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
 * This function not valid before that.
 *
 * (This code originally from NIHCL)
 */
UInt32	Date::jday (MonthOfYear month, DayOfMonth day, Year year)
{
// since above invalid earlier, assert in range
//Assert (y > 1753);
// hope works earlier???

	UInt32	c;
	UInt32	ya;
	if (month > 2) {
		month -= 3;
	}
	else {
		month += 9;
		year--;
	}
	c = year / 100;
	ya = year - 100*c;
	return (((146097*c)>>2) + ((1461*ya)>>2) + (153*month + 2)/5 + day + 1721119);
}

/*
 * Convert a Julian day number to its corresponding Gregorian calendar
 * date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
 * This function not valid before that.
 *
 * (This code originally from NIHCL)
 */
void Date::mdy (MonthOfYear& month, DayOfMonth& day, Year& year) const
{
	UInt32	m;
	UInt32	d;
	UInt32	y;

	UInt32 j = fDateRep - 1721119;
	y = (((j<<2) - 1) / 146097);
	j = (j<<2) - 1 - 146097*y;
	d = (j>>2);
	j = ((d<<2) + 3) / 1461;
	d = ((d<<2) + 3 - 1461*j);
	d = (d + 4)>>2;
	m = (5*d - 3)/153;
	d = 5*d - 3 - 153*m;
	d = (d + 5)/5;
	y = (100*y + j);
	if (m < 10) {
		m += 3;
	}
	else {
		m -= 9;
		y++;
	}
	month = m;
	day = d;
	year = y;
}

// not really right!!!
Date::Year	Date::YearsBetween (const Date& d1, const Date& d2)
{
	return (UInt32 ((d1 - d2).GetJulianRep () / 365.25));
}


Date::	operator String ()
{
	StringStream buf;
#if 	qGCC_InserterToSubclassOfOStreamBug
	((ostream&)buf) << *this;
#else
	buf << *this;
#endif
	return (String (buf));
}

/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */

Date	operator+ (const Date& d1, const Date& d2)
{
	return (Date (d1.GetJulianRep () + d2.GetJulianRep ()));
}



/*
 ********************************************************************************
 ************************************* operator- ********************************
 ********************************************************************************
 */

Date	operator- (const Date& d1, const Date& d2)
{
	return (Date (d1.GetJulianRep () - d2.GetJulianRep ()));
}



/*
 ********************************************************************************
 ************************************ operator== ********************************
 ********************************************************************************
 */

Boolean	operator== (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () == rhs.GetJulianRep ());
}



/*
 ********************************************************************************
 ************************************ operator!= ********************************
 ********************************************************************************
 */

Boolean	operator!= (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () != rhs.GetJulianRep ());
}




/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */

Boolean	operator< (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () < rhs.GetJulianRep ());
}



/*
 ********************************************************************************
 ************************************ operator<= ********************************
 ********************************************************************************
 */

Boolean	operator<= (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () <= rhs.GetJulianRep ());
}




/*
 ********************************************************************************
 ************************************* operator> ********************************
 ********************************************************************************
 */

Boolean	operator> (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () > rhs.GetJulianRep ());
}



/*
 ********************************************************************************
 ************************************ operator>= ********************************
 ********************************************************************************
 */

Boolean	operator>= (const Date& lhs, const Date& rhs)
{
	return Boolean (lhs.GetJulianRep () >= rhs.GetJulianRep ());
}



/*
 ********************************************************************************
 ***************************************** Min **********************************
 ********************************************************************************
 */
Date	Min (const Date& d1, const Date& d2)
{
	return ((d1 < d2)? d1: d2);
}



/*
 ********************************************************************************
 ***************************************** Max **********************************
 ********************************************************************************
 */
Date	Max (const Date& d1, const Date& d2)
{
	return ((d1 > d2)? d1: d2);
}





/*
 ********************************************************************************
 ****************************** iostream support ********************************
 ********************************************************************************
 */

ostream&	operator<< (ostream& out, const Date& date)
{
	Date::Year			y;
	Date::MonthOfYear	m;
	Date::DayOfMonth	d;

	date.mdy (m, d, y);	// from NIHCL
	if ((y >= 1900) and (y <= 1999)) {
		y -= 1900;	// make more standard format... (should generalize this code so it works in 10 years!!!)
	}
	out << (int)m << '/' << (int)d << '/' << (int)y;
	return (out);
}

istream&	operator>> (istream& in, Date& date)
{
// assumes format is given by above - later generalize ---
// mm/dd/[yy]yy
	UInt32	m = 0;
	UInt32	d = 0;
	UInt32	y = 0;
	char	sep1;
	char	sep2;
	in >> m >> sep1 >> d >> sep2 >> y;
	if ((m == 0) or (m > 12)) {
		in.clear (ios::badbit | in.rdstate ());		return (in);
	}
	if ((d == 0) or (d > 31)) {
		in.clear (ios::badbit | in.rdstate ());		return (in);
	}
	if ((sep1 != '/') or (sep2 != '/')) {
		in.clear (ios::badbit | in.rdstate ());		return (in);
	}
	if (y < 100) {
		y += 1900;
	}
	if (in) {
		date = Date (Date::jday ((Date::MonthOfYear)m, (Date::DayOfMonth)d, (Date::Year)y));
	}
	return (in);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

