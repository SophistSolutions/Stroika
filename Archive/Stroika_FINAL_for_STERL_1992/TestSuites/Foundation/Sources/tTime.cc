/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tTime.cc,v 1.7 1992/12/03 06:42:33 lewis Exp $
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: tTime.cc,v $
 *		Revision 1.7  1992/12/03  06:42:33  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug
 *
 *		Revision 1.6  1992/09/11  13:19:41  lewis
 *		Use endl rather than newline.
 *
 *		Revision 1.4  1992/07/14  20:07:31  lewis
 *		Scope Time:: constants.
 *
 *		Revision 1.3  1992/07/10  22:38:56  lewis
 *		scoped time constants.
 *
 *		Revision 1.2  1992/07/08  03:58:34  lewis
 *		Commented out tests for eqaulity of real computations - not quite sure why
 *		they fail, but in general not a reliable practice. Also, renamed TimeX to
 *		Time, and moved it to Time.hh/cc (really sterl did all this - I'm checking in).
 *
 *
 */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include	"OSInit.hh"
#pragma	push
	#pragma	force_active	on
	#if		qGCC_SimpleClassStaticInitializerBug
		static	OSInit	sOSInit;
	#else
		static	OSInit	sOSInit	=	OSInit ();
	#endif
#pragma	pop

#define		qUseAllGlobals		0
#define		qUseStringGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"Debug.hh"
#include	"Time.hh"

#include	"TestSuite.hh"










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



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
