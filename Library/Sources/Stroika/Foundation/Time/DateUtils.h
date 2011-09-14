/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateUtils_h_
#define	_Stroika_Foundation_Time_DateUtils_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"


#include	"Date.h"
#include	"DateTime.h"
#include	"Duration.h"
#include	"TimeOfDay.h"


/*
 * SHORT TERM - TODAY - TODO:

	+	GET RID OF THIS FILE ASAP - ONCE HEALTHFRAME HAS BEEN UPDATED

	+	add if POSIX to list of ifdefs we support and where easy support them and where not - oh well

	+	Replace (reserach firrst) use of name XML here with iso8601.
		+ maybe not quite. See http://www.w3.org/TR/xmlschema-2/#isoformats
			XML really is its own - nearly identical to iso8601, but see deviations...
			Maybe have iso8601, and XML support (and for our primitive purposes maybe the com eto the same thing?)

	+	Add regression test suite (TEST 7)

	+	Probably use times() for tickcount:
			#include <sys/times.h>
			clock_t times(struct tms *buf);

	+	Only test suite code to lift from old stroika is:



 */
#if 0


static	void	SimpleTimeTests ();
static	void	TestTime ();

int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Time..." << endl;

	TestTime ();
	
	cout << "Done Testing Time" << endl;
	return (0);
}

void	SimpleTimeTests ()
{
#if 0
Real	x = 0.001;
x *= 1000000.0;
TestCondition (x == 1000.0);
TestCondition ((0.001 * 1000000.0) == 1000.0);
TestCondition ((0.01 * 1000000.0) == 10000.0);
TestCondition ((0.1 * 1000000.0) == 100000.0);
TestCondition ((0.0001 * 1000000.0) == 100.0);
TestCondition ((0.00001 * 1000000.0) == 10.0);
TestCondition ((0.000001 * 1000000.0) == 1.0);

TestCondition ((0.002 * 1000000.0) == 2000.0);
TestCondition ((0.003 * 1000000.0) == 3000.0);
TestCondition ((0.004 * 1000000.0) == 4000.0);
TestCondition ((0.005 * 1000000.0) == 5000.0);
TestCondition ((0.006 * 1000000.0) == 6000.0);
TestCondition ((0.007 * 1000000.0) == 7000.0);
TestCondition ((0.008 * 1000000.0) == 8000.0);
TestCondition ((0.009 * 1000000.0) == 9000.0);
TestCondition ((0.0010 * 1000000.0) == 1000.0);
TestCondition ((0.0011 * 1000000.0) == 1100.0);

TestCondition ((0.000001 * 100000.0) == 	.1);
TestCondition ((0.000001 * 10000.0) ==  	.01);
TestCondition ((0.000001 * 1000.0) == 		.001);
TestCondition ((0.000001 * 100.0) == 		.0001);
TestCondition ((0.000001 * 10.0) == 		.00001);
TestCondition ((0.000001 * 1.0) == 			.000001); 
TestCondition ((0.000001 * .1) == 			.0000001);

TestCondition ((0.0015 * 1000000.0) == 1500.0);
TestCondition ((0.000999 * 1000000.0) == 999.0);
#endif

	TestCondition (Time () == 0);
#if 0
//	TestCondition ((kOneMicroSecond * 1000) == kOneMilliSecond);
Time f = Time::kOneMicroSecond * 1000;
//TestCondition (f == kOneMilliSecond);
//cerr << "f = " << f.fSeconds << endl;
//cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	TestCondition ((kOneMilliSecond * 1000) == kOneSecond);
f = Time::kOneMilliSecond * 1000;
//TestCondition (f == kOneSecond);
	TestCondition ((Time::kOneMicroSecond * 1000000) == Time::kOneSecond);
f = Time::kOneMicroSecond * 1000000;
TestCondition (f == Time::kOneSecond);
	
//	cerr << "kOneMicroSecond = " << kOneMicroSecond.fSeconds << endl;
//	cerr << "kOneMicroSecond * 1000 = " << (kOneMicroSecond * 1000).fSeconds << endl;
//	cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	cerr << "kOneMilliSecond * 1000 = " << (kOneMilliSecond * 1000).fSeconds << endl;

	Time t = Time::kOneSecond - Time::kOneMicroSecond;
	TestCondition (t < Time::kOneSecond);
	TestCondition (t <= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t <= Time::kOneSecond);
	TestCondition (t == Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t > Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
t = t - Time::kOneMicroSecond;
	TestCondition (t == Time::kOneSecond);
	t = t / 5;
	TestCondition (floor (t.GetSeconds ()) == 0);
//	TestCondition (t.fSeconds == 200000);
#endif
}

void	TestTime ()
{
	SimpleTimeTests ();
	
	Time t = GetCurrentTime ();
//	cerr << "current time = " << t.GetSeconds () << ", " << t.fSeconds << endl;
	Time t1 = GetCurrentTime ();
	Assert (t <= t1);
}
#endif



#endif
