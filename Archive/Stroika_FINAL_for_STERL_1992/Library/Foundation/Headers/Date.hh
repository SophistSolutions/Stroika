/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Date__
#define	__Date__

/*
 * $Header: /fuji/lewis/RCS/Date.hh,v 1.7 1992/11/19 05:02:43 lewis Exp $
 *
 * Description:
 *
 * The Date class is based on SmallTalk-80, The Language & Its Implementation,
 * page 108 (apx).
 *
 *		"...Date represents a specific data since the start of the Julian
 *	calendar. Class Date knows about some obvious information:
 *		->	there are seven days in a week, each day having a symbolic name and
 * 			an index 1..7
 *		->	there are twelve months in a year, each having a symbolic name and
 *			an index 1..12.
 *		->	months have 28..31 days and
 *		->	a particular year might be a leap year."
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Date.hh,v $
 *		Revision 1.7  1992/11/19  05:02:43  lewis
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/10/09  18:08:31  lewis
 *		Cleanups - got rid of extern declarations.
 *
 *		Revision 1.4  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.3  1992/07/01  00:00:53  lewis
 *		Use proper bug define qGCC_EnumeratorsNotConstantWhileDefiningEnumeration instead
 *		of hack.
 *
 *		Revision 1.2  1992/06/20  19:24:02  lewis
 *		Some gcc hacks to get things to compiler.
 *
 *
 *
 *
 */

#include	"String.hh"


class	Date {
	public:
		enum	DayOfWeek {
			eMonday = 1,
			eTuesday = 2,
			eWednesday = 3,
			eThursday = 4,
			eFriday = 5,
			eSaturday = 6,
			eSunday = 7,
#if		qGCC_EnumeratorsNotConstantWhileDefiningEnumeration
			eFirstDayOfWeek = 1,
			eLastDayOfWeek = 7,
#else	/*qGCC_EnumeratorsNotConstantWhileDefiningEnumeration*/
			eFirstDayOfWeek = eMonday,
			eLastDayOfWeek = eSunday,
#endif	/*qGCC_EnumeratorsNotConstantWhileDefiningEnumeration*/
		};
		enum	MonthOfYear {
			eJanuary = 1,
			eFebruary = 2,
			eMarch = 3,
			eApril = 4,
			eMay = 5,
			eJune = 6,
			eJuly = 7,
			eAugust = 8,
			eSeptember = 9,
			eOctober = 10,
			eNovember = 11,
			eDecember = 12,
#if		qGCC_EnumeratorsNotConstantWhileDefiningEnumeration
			eFirstMonthOfYear = 1,
			eLastMonthOfYear = 12,
#else	/*qGCC_EnumeratorsNotConstantWhileDefiningEnumeration*/
			eFirstMonthOfYear = eJanuary,
			eLastMonthOfYear = eDecember,
#endif	/*qGCC_EnumeratorsNotConstantWhileDefiningEnumeration*/
		};
		enum	DayOfMonth {
			eFirstDayOfMonth = 1,
			eLastDayOfMonth = 31,
		};
		enum	DayOfYear {
			eFirstDayOfYear = 1,
			eLastDayOfYear = 366,
		};
		enum	Year {
			eFirstYear = kMinInt16,
			eLastfYear = kMaxInt16,
		};

		static	DayOfWeek		NameToDayOfWeek (const String& dayName);
		static	String			DayOfWeekToName (DayOfWeek dayIndex);
		static	MonthOfYear		IndexOfMonth (const String& monthName);
		static	String			NameOfMonth (MonthOfYear monthIndex);
		static	DayOfMonth		DaysInMonth (MonthOfYear monthIndex, Year year);
		static	DayOfYear		DaysInYear (Year year);
		static	Boolean			LeapYear (Year year);

		Date ();					// Todays date
		Date (Int32 dayCount);		/* January 1, 1901 +/- dayCount days */
		Date (DayOfMonth dayOfMonth, MonthOfYear monthOfYear, Year year);
		Date (DayOfYear dayOfYear, Year year);

		nonvirtual	void	AddDays (Int32 dayCount);	// days from when??? Julian beginning is when?
		nonvirtual	void	SubtractDays (Int32 dayCount);
		nonvirtual	void	operator+= (const Date& delta);
		nonvirtual	void	operator-= (const Date& delta);

		nonvirtual	operator String ();		// maybe need different interace?

		nonvirtual	UInt32	GetJulianRep () const;

	public:
		static 		UInt32	jday (MonthOfYear month, DayOfMonth day, Year year);			// from NIHCL
		nonvirtual	void	mdy (MonthOfYear& month, DayOfMonth& day, Year& year) const;	// from NIHCL
	
		static	Year	YearsBetween (const Date& d1, const Date& d2);

	private:
		UInt32	fDateRep;		// Julian Date
};





/*
 * Arithmatic
 */
Date	operator+ (const Date& lhs, const Date& rhs);
Date	operator- (const Date& lhs, const Date& rhs);


/*
 * Comparisons
 */
Boolean	operator== (const Date& lhs, const Date& rhs);
Boolean	operator!= (const Date& lhs, const Date& rhs);
Boolean	operator<  (const Date& lhs, const Date& rhs);
Boolean	operator<= (const Date& lhs, const Date& rhs);
Boolean	operator>  (const Date& lhs, const Date& rhs);
Boolean	operator>= (const Date& lhs, const Date& rhs);


Date	Min (const Date& d1, const Date& d2);
Date	Max (const Date& d1, const Date& d2);


/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
ostream&	operator<< (ostream& out, const Date& date);
istream&	operator>> (istream& in, Date& date);



// DO operators to add/sub, lessThan, etc ...
//	extern	void	Now (Date& d, Time& t);		PROBABLY NOT NEEDED

// maybe do date/time stuct, and have TODAY() be funct returning that.  Then,
// do comparisons with that guy???





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


inline	UInt32	Date::GetJulianRep () const { return (fDateRep); }



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Date__*/
