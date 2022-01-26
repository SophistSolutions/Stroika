/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tString.cc,v 1.12 1992/12/03 06:43:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tString.cc,v $
 *		Revision 1.12  1992/12/03  06:43:42  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *		Get rid of #define qUseStringGlobals.
 *
 *		Revision 1.11  1992/10/16  22:54:58  lewis
 *		Added more tests for overloaded op==, etc for const char*.
 *
 *		Revision 1.9  1992/09/20  01:28:39  lewis
 *		Added Test7() to be sure relational operators work properly.
 *		Apparantly they got broken at some point recently.
 *
 *		Revision 1.8  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.5  1992/09/04  01:14:48  lewis
 *		Use delete[] instead of delete for deleting arrays in two places. Worked
 *		on mac, but just lucky - crashed under gcc/aux.
 *
 *		Revision 1.3  1992/07/21  06:48:31  lewis
 *		Add some conversions of "fred" to String("fred") in the context of op+.
 *		CFront handles overloading the way I intended here - not sure who
 *		is right, but for the purpose of a testsuite - just trying to make
 *		sure MY code works right, the more precise call now used
 *		is fine.
 *
 *		Revision 1.2  1992/07/03  04:24:05  lewis
 *		Rename Strings.hh -> String.hh.
 *		Rename DoublyLinkedList->DoubleLinkList.
 *		Rename ArrayOfPointers -> ArrayPtr.
 *		Rename DoubleLLOfPointers DoubleLinkListPtr.
 *
 *		Revision 1.4  1992/06/09  17:57:45  lewis
 *		Change Test1 () call to itself 8 times rather than 10,
 *		cuz took too long (even on snake!!!).
 *
 *		Revision 1.2  92/05/21  17:34:52  17:34:52  lewis (Lewis Pringle)
 *		React to change in definition of String::SubString().
 *		And test IndexOf/RIndexOf versions that take a String - not just char.
 *		
 *		Revision 1.9  1992/03/11  23:11:46  lewis
 *		Use cout/cerr instead of gDebugStream since thats not available when
 *		debug turned off anyhow, and using two different streams allows us to
 *		differentiate info from actual errors.
 *
 *		Revision 1.7  1992/01/22  04:12:32  lewis
 *		Cleanup, always call OSInit, and try again at bug at end of file -
 *		gave up - fix later.
 *
 *		Revision 1.6  1992/01/09  07:23:55  lewis
 *		Debugged several nasty bugs, HP compiler bug caused Align() macro not to
 *		work, I had a bug in Memory ReAlloc () where it would read past the end
 *		of the buff (reading like this OK on mac), and added new procedure Test1()
 *		which uncovered another new bug I've not debugged yet, though it looks
 *		pretty easy, like it wont come up much, and is just in the String.cc
 *		code itself. Not a big problem -- I think...
 *
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
#include	"Stroika-Foundation-Globals.hh"

#include	"StreamUtils.hh"
#include	"String.hh"
#include	"Time.hh"

#include	"TestSuite.hh"



#define		qPrintTimings		!qDebug


static	void	Test1 ();
static	void	Test2_Helper (String& s1, String& s2);
static	void	Test2 ();
static	void	Test3 ();
static	void	Test4 ();
static	void	Test5 ();
static	String	Test6 (const String& a, int depth);
static	void	Test7 ();

#if qDebug
	const	int	kLoopEnd = 100;
#else
	const	int	kLoopEnd = 2000;
#endif


static unsigned ipow (unsigned n, unsigned toPow)
{
	// quick hack since pow didnt seem to do what I want - just blindly
	// multiply and dont worry about overflow...
	unsigned result = 1;
	while (toPow-- != 0) {
		result *= n;
	}
	return (result);
}
int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing strings..." << endl;

	Test1 ();
	Test2 ();
	Test3 ();
	Test4 ();
	Test5 ();

	const	int	kRecurseDepth = 6;
	String	testString = "some dump test";

#if		qPrintTimings
	cout << tab << "Recursive build test with depth " << kRecurseDepth << " for " << testString << endl;
	Time t = GetCurrentTime ();
#endif

	String s = Test6 (testString, kRecurseDepth);	// returns length 114688 for depth 6
	TestCondition (s.GetLength () ==  (ipow (4,kRecurseDepth) * 2 * testString.GetLength ()));

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished Recursive build test. Time elapsed = " << t << " length = " << s.GetLength () << endl;
#endif

	Test7 ();

	cout << "Testing Succeeds!" << endl;
	return (0);
}

static	void	Test1 ()
{
	/*
	 * Some simple tests to start off with.
	 */
	{
		TestCondition (String (String ("fred") + String ("joe")).GetLength () == 7);
		TestCondition (String ("fred") + String ("joe") == String ("fredjoe"));
		{
			String_BufferedCharArray s1 ("test strings");
			String_BufferedCharArray s2 ("test strings");
			TestCondition (Character ('a') == 'a');
			Test2_Helper (s1, s2);
		}
		{
			String	s1	=	String_CharArray ("test strings");
			String	s2	=	String_CharArray ("test strings");
			Test2_Helper (s1, s2);
		}
		{
			String	s1	=	String_ReadOnlyChar ("test strings");
			String	s2	=	String_ReadOnlyChar ("test strings");
			Test2_Helper (s1, s2);
		}
	}
}

static	void	StressTest1 (String big)
{
	for (int j = 1; j <= kLoopEnd/50; j++) {
		String_ReadOnlyChar	a ("a");
		for (int i = 1; i <= kLoopEnd; i++) {
			big += a;
			TestCondition (big.GetLength () == i);
			TestCondition (big[i] == 'a');
		}
		big.SetLength (0);
	}

	String	s1	=	"test strings";
	for (int i = 1; i <= kLoopEnd; i++) {
		big += s1;
		TestCondition (big.GetLength () == s1.GetLength () * i);
	}
}

static	void	StressTest2 (String big)
{
	String	s1	=	"test strings";
	for (int i = 1; i <= kLoopEnd; i++) {
		big = big + s1;
		TestCondition (big.GetLength () == s1.GetLength () * i);
	}
}

static	void	StressTestStrings ()
{
#if		qPrintTimings
	cout << tab << "Stress testing strings..." << endl;
	Time t = GetCurrentTime ();
#endif

	{
	String s ("");
	StressTest1 (s);
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished Stress testing strings += ... time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	{
	String s ("");
	StressTest2 (s);
	}
	
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished Stress testing strings + ... time elapsed = " << t << endl;
#endif
}

static	void	StressTestBufferedStrings ()
{
#if		qPrintTimings
	cout << tab << "Stress testing buffered strings..." << endl;
	Time t = GetCurrentTime ();
#endif

	{
	String_BufferedCharArray s ("");
	StressTest1 (s);
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	{
	String_BufferedCharArray s ("");
	StressTest2 (s);
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished stress testing buffered strings + ... at " << t << endl;
#endif
}

static	void	Test2 ()
{
	StressTestStrings ();
	StressTestBufferedStrings ();
}

static	void	Test2_Helper (String& s1, String& s2)
{
	TestCondition (s1.GetLength () == 12);
	TestCondition (String (s1).GetLength () == 12);
	TestCondition (s1 == s2);
	TestCondition (! (s1 != s2));
	TestCondition (s1 + s1 == s2 + s2);
	TestCondition ((s1 + s1).GetLength () == s1.GetLength () *2);
	TestCondition (s1[3] == 's');
	TestCondition ('s' == s1[3]);
	TestCondition (s1.GetLength () == 12);
	String s3;
	s3 += s1;
	s3 += s2;
	s1 += "\n";
	TestCondition (s1.GetLength () == 13);
}

static	void	Test3 ()
{
	{
#if		qSunCFrontCannotDeclareConstAutoArrayOfChar_Bug
		#define	frobaz	"abc"
#else
		const	char	frobaz[]	=	"abc";
#endif
		String	t1;
		String	t2	=	t1;
		String	t3	=	"a";
		String	t4	=	"a";

		TestCondition (t1 == "");
		TestCondition (t1 == String ());
		TestCondition (t1 == String (""));
		TestCondition (t1 == t2);
		TestCondition (t3 == "a");
		TestCondition (t3 == String("a"));
		TestCondition (t4 == "a");
		t1 = t1;
		TestCondition (t1 == "");

		t1 += 'F';
		t1 += 'r';
		t1 += 'e';
		t1 += 'd';
		t1 += " Flintstone";
		TestCondition (t1 == "Fred Flintstone");
		TestCondition ("Fred Flintstone" == t1);
		TestCondition (String ("Fred Flintstone") == t1);
		TestCondition (t1 == String ("Fred Flintstone"));
		TestCondition (t2 != "Fred Flintstone");
		TestCondition ("Fred Flintstone" != t2);
		TestCondition (String ("Fred Flintstone") != t2);
		TestCondition (t2 != String ("Fred Flintstone"));

		TestCondition (t1.GetLength () == 15);
		t1.SetLength (20);
		TestCondition (t1.GetLength () == 20);
		t1.SetLength (4);
		TestCondition (t1.GetLength () == 4);
		TestCondition (t1 == "Fred");

		TestCondition (t1[1] == 'F');
		TestCondition (t1[2] == 'r');
		TestCondition (t1[3] == 'e');
		TestCondition (t1[4] == 'd');

		TestCondition (t1[1] == 'F');
		TestCondition (t1[2] == 'r');
		TestCondition (t1[3] == 'e');
		TestCondition (t1[4] == 'd');

		String	a[10];
		TestCondition (a[2] == "");
		a [3] = "Fred";
		TestCondition (a[3] == "Fred");
		TestCondition (a[2] != "Fred");
	}
}

static	void	Test4 ()
{
	{
#if		qSunCFrontCannotDeclareConstAutoArrayOfChar_Bug
		#define	frobaz	"abc"
#else
		const	char	frobaz[]	=	"abc";
#endif
		String	t1;
		String	t3	=	"a";
		String	t5	=	String (frobaz, sizeof (frobaz)-1);
		String*	t6	=	new String ("xyz");
		delete (t6);

		t5 = t1;
		t1 = t5;
		t1 = t1;
		TestCondition (t1 == "");
		TestCondition (t5 == "");

		t1 += 'F';
		t1 += 'r';
		t1 += 'e';
		t1 += 'd';
		t1 += " Flintstone";
		TestCondition (t1 == "Fred Flintstone");
		t5 = t1.SubString (6, 10);
		TestCondition (t5 == "Flintstone")		

		t1.SetLength (20);
		t1.SetLength (4);

		t5 = t1;
		t5.SetCharAt ('f', 1);
		t5.SetCharAt ('R', 2);
		t5.SetCharAt ('E', 3);
		t5.SetCharAt ('D', 4);

// was causing apples compiler to barf - fix sterl...
// LGP 7/22/92
#if	0
		TestCondition (t5[1] == 'f');
		TestCondition (t5[2] == 'R');
		TestCondition (t5[3] == 'E');
		TestCondition (t5[4] == 'D');
		TestCondition (t5.IndexOf ('f') == 1);
		TestCondition (t5.IndexOf ("f") == 1);
		TestCondition (t5.IndexOf ("fR") == 1);
		TestCondition (t5.IndexOf ("fRE") == 1);
		TestCondition (t5.IndexOf ("fRED") == 1);
		TestCondition (t5.IndexOf ("fRD") == kBadSequenceIndex);
		TestCondition (t5.IndexOf ('R') == 2);
		TestCondition (t5.IndexOf ('E') == 3);
		TestCondition (t5.IndexOf ('D') == 4);
		TestCondition (t5.IndexOf ("D") == 4);

		TestCondition (t5.RIndexOf ('f') == 1);
		TestCondition (t5.RIndexOf ('R') == 2);
		TestCondition (t5.RIndexOf ('E') == 3);
		TestCondition (t5.RIndexOf ('D') == 4);

		t5.SetCharAt ('D', 1);
		t5.SetCharAt ('D', 2);
		t5.SetCharAt ('D', 3);
		t5.SetCharAt ('D', 4);
		TestCondition (t5.IndexOf ('D') == 1);
		TestCondition (t5.IndexOf ("D") == 1);
		TestCondition (t5.RIndexOf ('D') == 4);
		TestCondition (t5.RIndexOf ("D") == 4);
		TestCondition (t5.IndexOf ('f') == kBadSequenceIndex);
		TestCondition (t5.IndexOf ("f") == kBadSequenceIndex);
		TestCondition (t5.RIndexOf ('f') == kBadSequenceIndex);
		TestCondition (t5.RIndexOf ("f") == kBadSequenceIndex);

		TestCondition (((char*)t5.Peek ())[0] == 'D');
		TestCondition (((char*)t5.Peek ())[1] == 'D');
		TestCondition (((char*)t5.Peek ())[2] == 'D');
		TestCondition (((char*)t5.Peek ())[3] == 'D');
#endif
	}
}

static	void	Test5 ()
{
	{
		String arr [100];
		arr [3] = "fred";
		TestCondition (arr[3] == "fred");
		String*	l	=	new String [100];
		l[3] = "FRED";
		TestCondition (l[3] == "FRED");
		TestCondition (l[99] == "");
        delete[] (l);
		UInt32 nSlots = 100;
		l	=	new String [size_t (nSlots)];
        delete[] (l);
	}
}

static	String	Test6 (const String& a, int depth)
{
	String	b = a;
	b += a;
	if (depth > 0) {
		b = Test6 (b, depth-1) + Test6 (b, depth-1);
	}
	return (b);
}

static	void	Test7 ()
{
	TestCondition (String ("1") <= String ("1"));
	TestCondition (String ("1") <= String ("10"));
	TestCondition (not (String ("1") > String ("10")));
	TestCondition (not (String ("1") >= String ("10")));
	TestCondition (String ("1") < String ("10"));

	TestCondition (String ("20") > String ("11"));
	TestCondition (String ("20") >= String ("11"));
	TestCondition (not (String ("20") < String ("11")));
	TestCondition (not (String ("20") <= String ("11")));
	TestCondition (String ("11") < String ("20"));
	TestCondition (String ("11") <= String ("20"));
	TestCondition (not (String ("11") > String ("20")));
	TestCondition (not (String ("11") >= String ("20")));

	TestCondition (String ("aac") > String ("aab"));
	TestCondition (String ("aac") >= String ("aab"));
	TestCondition (not (String ("aac") < String ("aab")));
	TestCondition (not (String ("aac") <= String ("aab")));

	TestCondition (String ("apple") < String ("apples"));
	TestCondition (String ("apple") <= String ("apples"));
	TestCondition (not (String ("apple") > String ("apples")));
	TestCondition (not (String ("apple") >= String ("apples")));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


