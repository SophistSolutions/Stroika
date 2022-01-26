/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Time.cc,v 1.14 1992/12/10 05:51:44 lewis Exp $
 *
 *
 * TODO:
 *
 * Changes:
 *	$Log: Time.cc,v $
 *		Revision 1.14  1992/12/10  05:51:44  lewis
 *		For UNIX/POSIX we must include <sys/times.h> - still looking for my POSIX
 *		manual to be sure, but at least AUX headers require it.
 *
 *		Revision 1.13  1992/12/08  21:29:32  lewis
 *		Add case for qWinNT.
 *		Try out more Spartan includes. See which OS's fail.
 *
 *		Revision 1.12  1992/11/12  08:11:23  lewis
 *		Use double instead of Real.
 *
 *		Revision 1.11  1992/09/25  21:10:38  lewis
 *		 Get rid of Declares/Implements for containers of Time - no longer needed.
 *
 *		Revision 1.10  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.9  1992/09/15  17:14:08  lewis
 *		Get rid of AbSequence.
 *
 *		Revision 1.8  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/03  19:28:11  lewis
 *		Got rid of conditional code to use ::gettimeofday() for BSD
 *		based UNIXs. Instead, always use times(), for POSIX/UNIX.
 *
 *		Revision 1.6  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/08  01:35:27  lewis
 *		Use Sterls new Real based Time class, and change GetCurrentTime () to be
 *		0 AD based, instead of 1970AD based.
 *
 *		Revision 1.3  1992/07/03  07:06:13  lewis
 *		Get rid of warnings from gcc, and tried code that broke assembler before
 *		with gcc and seemed to work fine now - so commented back in.
 *
 *		Revision 1.2  1992/06/22  15:45:26  lewis
 *		Commented out procedure body that gcc 2.2.1 barfs on - fix later.
 *
 *		Revision 1.19  1992/04/20  14:12:44  lewis
 *		Doesn't seem to be an POSIX complaint way to get time to better than
 *		second granularity. Tried to make this file Posix compliant, but dont
 *		know how - for now, under _POSIX_SOURCE for when we compile for snake,
 *		and we'll have to take each port on a case by case basis as far as this
 *		one point is concerned.
 *
 *		Revision 1.18  92/04/07  12:55:55  12:55:55  sterling (Sterling Wight)
 *		fixed unix version of GetCurrentTime
 *		
 *		Revision 1.17  92/03/30  14:17:31  14:17:31  lewis (Lewis Pringle)
 *		Ported to Borland and added ifdefs around marco based templates Implement calls.
 *		
 *		Revision 1.15  1992/03/22  16:44:18  lewis
 *		Add iostream operators.
 *
 *		Revision 1.13  1992/01/03  07:06:08  lewis
 *		Fix float/integer overflow probelm in Time(BigReal). Came up on Hp with
 *		large input values for seconds.
 *
 *
 *
 */



#if		1


// See if this new approach to time includes works - shorter and simpler...

#include	<time.h>

#include	"OSRenamePre.hh"
	#if		qMacOS
	// See which of these mac includes really necessary...
		#include	<Events.h>
		#include	<OSUtils.h>
	//	#include	<Math.h>
		#include	<SysEqu.h>			// could use to access vars TimeLM, and Ticks - currently using functs.
	#elif	qUnixOS
		#include	<sys/times.h>		// See in POSIX manual why this is necessary???
	#endif	/*qMacOS*/
#include	"OSRenamePost.hh"


#else

				#include	"OSRenamePre.hh"
				#if		qMacOS
				#include	<Events.h>
				#include	<OSUtils.h>
				#include	<Math.h>
				#include	<SysEqu.h>			// could use to access vars TimeLM, and Ticks - currently using functs.
				#elif	qUnixOS
				#include	<time.h>
				#include	<sys/times.h>
				#elif	qDOS || qWinNT || qWinOS
				#include	<math.h>			// Needed for???
				#include	<time.h>
				#endif	/*qMacOS*/
				#include	"OSRenamePost.hh"
#endif


#include	"Debug.hh"
#include	"Exception.hh"
#include	"StreamUtils.hh"

#include	"Time.hh"









/*
 ********************************************************************************
 ********************************** operator+ ***********************************
 ********************************************************************************
 */
Time	operator+ (const Time& lhs, const Time& rhs)
{
	Time t = lhs;
	t += rhs;
	return (t);
}




/*
 ********************************************************************************
 ********************************** operator- ***********************************
 ********************************************************************************
 */
Time	operator- (const Time& lhs, const Time& rhs)
{
	Time t = lhs;
	t -= rhs;
	return (t);
}





/*
 ********************************************************************************
 ********************************** operator* ***********************************
 ********************************************************************************
 */
Time	operator* (const Time& lhs, double rhs)
{
	Time t = lhs;
	t *= rhs;
	return (t);
}

Time	operator* (double lhs, const Time& rhs)
{
	Time t = rhs;
	t *= lhs;
	return (t);
}





/*
 ********************************************************************************
 ********************************** operator/ ***********************************
 ********************************************************************************
 */
Time	operator/ (const Time& lhs, double rhs)
{
	Time t = lhs;
	t /= rhs;
	return (t);
}





/*
 ********************************************************************************
 ****************************** Stream Operators ********************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const Time& t)
{
	out << t.GetSeconds () << ' ';
	return (out);
}

istream&	operator>> (istream& in, Time& t)
{
	double	r;
	in >> r;
	if (in) {
		t = r;
	}
	return (in);
}






/*
 ********************************************************************************
 ********************************** GetCurrentTime ******************************
 ********************************************************************************
 */

Time	GetCurrentTime ()
{
	// A rough guestimate...
	const	double	kSecondsFromGodsBirthToUnixsBirth	=	double (1970) * (365.25*60.0*60.0);

#if		qMacOS
	static	Int32	kTimeOffset	=	0;		// SECONDS between Jan 1, 1970 & start of Tickcounts...

	if (kTimeOffset	== 0) {
		unsigned	long	curSEX;			// seconds since Jan 1, 1904
		::GetDateTime (&curSEX);
		Assert (curSEX == *(long*)TimeLM);	// so why not avoid trap??? (actualy could be off by 1?)

		DateTimeRec	unixDATE;
		unsigned	long	unixSEX;		// SEX between Jan 1, 1904 and Jan 1 1970
		unixDATE.year = 1970;
		unixDATE.month = 1;
		unixDATE.day = 1;
		unixDATE.hour = 0;
		unixDATE.minute = 0;
		unixDATE.second = 0;
		unixDATE.dayOfWeek = 0;		//???? according to page IM II-379 seems to be ignored.
		::Date2Secs (&unixDATE, &unixSEX);

		/*
		 * Could be off by as much as 1/60 tick, + time to do above code, but is fixed
		 * invalid amount, and deltas always will remain correct, so no big deal.
		 */
		kTimeOffset = (curSEX - unixSEX) - ::TickCount ()/60 + kSecondsFromGodsBirthToUnixsBirth;
	}

	register	Int32	time	=	kTimeOffset + ::TickCount ();
	return (double (kTimeOffset + ::TickCount ()) / 60.0);
#elif	qUnixOS
	// perhaps this should be if POSIX - ???
	/*
	 * POSIX gives us no way to get the current time precisely, but it gives us a
	 * way to get the current time +- 1 second (time) and a way to get the time
	 * since the computer started(or some such initial condition) and now. We can
	 * use these together to give us the current time accurately, by calibrating
	 * the times () values by the result of time (). Thus, other than the first time
	 * when we are calibrating, we just call times () and add our calibration.
	 */
	struct	tms	tmsBuf;
	static	double	kTimeDelta		=	time (Nil) - double (times (&tmsBuf))/CLOCKS_PER_SEC + kSecondsFromGodsBirthToUnixsBirth;
	return (double (times (&tmsBuf))/CLOCKS_PER_SEC + kTimeDelta);
#elif	qDOS || qWinNT || qWinOS
	time_t now = time (Nil);
	return (now +  kSecondsFromGodsBirthToUnixsBirth);
#endif	/*OS*/
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

