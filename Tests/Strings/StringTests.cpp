/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

#include	"Stroika/Foundation/StroikaPreComp.h"


#include	<iostream>


#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Time/Realtime.h"
#include	"Stroika/Foundation/Characters/String.h"



extern  void    TestStrings ();

#define	TestCondition(e)\
	if (!(e)) cout << "File '" << __FILE__ << "'; Line "  << __LINE__ << " ## TEST FAILED: " #e << endl;

#define		qPrintTimings		!qDebug

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;


namespace	{
	void	_ASSERT_HANDLER_(const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}


bool	operator== (Character lhs, wchar_t rhs)
{
    return (lhs.GetCharacterCode () == rhs);
}

bool	operator== (wchar_t lhs, Character rhs)
{
    return (lhs == rhs.GetCharacterCode ());
}


bool	operator== (Character lhs, char rhs)
{
    return (lhs.GetAsciiCode () == rhs);
}

bool	operator== (char lhs, Character rhs)
{
    return (lhs == rhs.GetAsciiCode ());
}

static	void	Test1 ();
static	void	Test2_Helper (String& s1, String& s2);
static	void	Test2 ();
static	void	Test3 ();
static	void	Test4 ();
static	void	Test5 ();
static	String	Test6 (const String& a, int depth);
static	void	Test7 ();
static  void    TestReadOnlyStrings ();

#if qDebug
	const	int	kLoopEnd = 1000;
#else
	const	int	kLoopEnd = 2000;
#endif

wstring	ToStdStr (const String& s)
{
    return wstring ((wchar_t*)s.Peek (), s.GetLength ());
}

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

void    TestStrings ()
{
#if		qDebug
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif

	cout << "Testing strings..." << endl;

    TestReadOnlyStrings ();

	Test1 ();
	Test2 ();
	Test3 ();
	Test4 ();
	Test5 ();

	const	int	kRecurseDepth = 6;
	String	testString = L"some dump test";

#if		qPrintTimings
	cout << '\t' << "Recursive build test with depth " << kRecurseDepth << " for " << testString << endl;
	Time t = GetCurrentTime ();
#endif

	String s = Test6 (testString, kRecurseDepth);	// returns length 114688 for depth 6
	TestCondition (s.GetLength () ==  (ipow (4,kRecurseDepth) * 2 * testString.GetLength ()));

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished Recursive build test. Time elapsed = " << t << " length = " << s.GetLength () << endl;
#endif

	Test7 ();

	cout << "String testing Succeeds!" << endl;
}

static	void	Test1 ()
{
	/*
	 * Some simple tests to start off with.
	 */
	{
		TestCondition (String (String (L"fred") + String (L"joe")).GetLength () == 7);

		TestCondition (String (L"fred") + String (L"joe") == String (L"fredjoe"));
		{
			String	s1	=	String_CharArray (L"test strings");
			String	s2	=	String_CharArray (L"test strings");
			Test2_Helper (s1, s2);
		}

		{
			String_BufferedCharArray s1 (L"test strings");
			String_BufferedCharArray s2 (L"test strings");

			TestCondition (Character ('a') == 'a');

			Test2_Helper (s1, s2);
		}

		{
			String	s1	=	String_ReadOnlyChar (L"test strings");
			String	s2	=	String_ReadOnlyChar (L"test strings");
			Test2_Helper (s1, s2);
		}
	}
}


static	void	StressTest1 (String big)
{
	for (int j = 1; j <= kLoopEnd/50; j++) {
		String_ReadOnlyChar	a (L"a");
		for (int i = 0; i <= kLoopEnd; i++) {
			big += a;
			TestCondition ((big.GetLength () -1) == i);
			TestCondition (big[i] == 'a');
		}
		big.SetLength (0);
	}

	String	s1	=	L"test strings";
	for (int i = 1; i <= kLoopEnd; i++) {
		big += s1;
		TestCondition (big.GetLength () == s1.GetLength () * i);
	}
}

static	void	StressTest2 (String big)
{
	String	s1	=	L"test strings";
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
	String s (L"");
	StressTest1 (s);
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished Stress testing strings += ... time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	{
	String s (L"");
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
	String_BufferedCharArray s (L"");
	StressTest1 (s);
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	{
	String_BufferedCharArray s (L"");
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
	TestCondition (s1[2] == 's');
	TestCondition ('s' == s1[2]);
	TestCondition (s1.GetLength () == 12);

	String s3;
	s3 += s1;
	s3 += s2;

	s1 += L"\n";
	TestCondition (s1.GetLength () == 13);
}

static	void	Test3 ()
{
	{
		const	wchar_t	frobaz[]	=	L"abc";

		String	t1;
		String	t2	=	t1;
		String	t3	=	L"a";
		String	t4	=	L"a";

		TestCondition (t1 == L"");
		TestCondition (t1 == String ());
		TestCondition (t1 == String (L""));
		TestCondition (t1 == t2);
		TestCondition (t3 == L"a");
		TestCondition (t3 == String(L"a"));
		TestCondition (t4 == L"a");
		t1 = t1;
		TestCondition (t1 == L"");

		t1 += 'F';
		t1 += 'r';
		t1 += 'e';
		t1 += 'd';
		t1 += L" Flintstone";
		TestCondition (t1 == L"Fred Flintstone");
		TestCondition (L"Fred Flintstone" == t1);
		TestCondition (String (L"Fred Flintstone") == t1);
		TestCondition (t1 == String (L"Fred Flintstone"));
		TestCondition (t2 != L"Fred Flintstone");
		TestCondition (L"Fred Flintstone" != t2);
		TestCondition (String (L"Fred Flintstone") != t2);
		TestCondition (t2 != String (L"Fred Flintstone"));

		TestCondition (t1.GetLength () == 15);
		t1.SetLength (20);
		TestCondition (t1.GetLength () == 20);
		t1.SetLength (4);
		TestCondition (t1.GetLength () == 4);
		TestCondition (t1 == L"Fred");

		TestCondition (t1[0] == 'F');
		TestCondition (t1[1] == 'r');
		TestCondition (t1[2] == 'e');
		TestCondition (t1[3] == 'd');

		TestCondition (t1[0] == 'F');
		TestCondition (t1[1] == 'r');
		TestCondition (t1[2] == 'e');
		TestCondition (t1[3] == 'd');

		String	a[10];
		TestCondition (a[2] == L"");
		a [3] = L"Fred";
		TestCondition (a[3] == L"Fred");
		TestCondition (a[2] != L"Fred");
	}
}

static	void	Test4 ()
{
	{
		const	wchar_t	frobaz[]	=	L"abc";

		String	t1;
		String	t3	=	L"a";
		String	t5	=	String (frobaz);
		String*	t6	=	new String (L"xyz");
		delete (t6);

		t5 = t1;
		t1 = t5;
		t1 = t1;
		TestCondition (t1 == L"");
		TestCondition (t5 == L"");

		t1 += 'F';
		t1 += 'r';
		t1 += 'e';
		t1 += 'd';
		t1 += L" Flintstone";
		TestCondition (t1 == L"Fred Flintstone");
		t5 = t1.SubString (5, 10);
		TestCondition (t5 == L"Flintstone")

		t1.SetLength (20);
		t1.SetLength (4);

		t5 = t1;
		t5.SetCharAt ('f', 0);
		t5.SetCharAt ('R', 1);
		t5.SetCharAt ('E', 2);
		t5.SetCharAt ('D', 3);

		TestCondition (t5[0] == 'f');
		TestCondition (t5[1] == 'R');
		TestCondition (t5[2] == 'E');
		TestCondition (t5[3] == 'D');
		TestCondition (t5.IndexOf ('f') == 0);
		TestCondition (t5.IndexOf (L"f") == 0);
		TestCondition (t5.IndexOf (L"fR") == 0);
		TestCondition (t5.IndexOf (L"fRE") == 0);
		TestCondition (t5.IndexOf (L"fRED") == 0);
		TestCondition (t5.IndexOf (L"fRD") == kBadStringIndex);
		TestCondition (t5.IndexOf ('R') == 1);
		TestCondition (t5.IndexOf ('E') == 2);
		TestCondition (t5.IndexOf ('D') == 3);
		TestCondition (t5.IndexOf (L"D") == 3);

		TestCondition (t5.RIndexOf ('f') == 0);
		TestCondition (t5.RIndexOf ('R') == 1);
		TestCondition (t5.RIndexOf ('E') == 2);
		TestCondition (t5.RIndexOf ('D') == 3);
		TestCondition (t5.RIndexOf (L"D") == 3);
		TestCondition (t5.RIndexOf (L"ED") == 2);
		TestCondition (t5.RIndexOf (L"RED") == 1);
		TestCondition (t5.RIndexOf (L"fRED") == 0);
		TestCondition (t5.RIndexOf (L"fr") == kBadStringIndex);
		TestCondition (t5.RIndexOf (L"f") == 0);

		t5.SetCharAt ('D', 0);
		t5.SetCharAt ('D', 1);
		t5.SetCharAt ('D', 2);
		t5.SetCharAt ('D', 3);
		TestCondition (t5.IndexOf ('D') == 0);
		TestCondition (t5.IndexOf (L"D") == 0);
		TestCondition (t5.RIndexOf ('D') == 3);
		TestCondition (t5.RIndexOf (L"D") == 3);

		TestCondition (t5.IndexOf ('f') == kBadStringIndex);
		TestCondition (t5.IndexOf (L"f") == kBadStringIndex);
		TestCondition (t5.RIndexOf ('f') == kBadStringIndex);
		TestCondition (t5.RIndexOf (L"f") == kBadStringIndex);

		TestCondition ((t5.Peek ())[0] == 'D');
		TestCondition ((t5.Peek ())[1] == 'D');
		TestCondition ((t5.Peek ())[2] == 'D');
		TestCondition ((t5.Peek ())[3] == 'D');
	}
}

static	void	Test5 ()
{
	{
		String arr [100];
		arr [3] = L"fred";
		TestCondition (arr[3] == L"fred");
		String*	l	=	new String [100];
		l[3] = L"FRED";
		TestCondition (l[3] == L"FRED");
		TestCondition (l[99] == L"");
        delete[] (l);
		size_t nSlots = 100;
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
	TestCondition (String (L"1") <= String (L"1"));
	TestCondition (String (L"1") <= String (L"10"));
	TestCondition (not (String (L"1") > String (L"10")));
	TestCondition (not (String (L"1") >= String (L"10")));
	TestCondition (String (L"1") < String (L"10"));

	TestCondition (String (L"20") > String (L"11"));
	TestCondition (String (L"20") >= String (L"11"));
	TestCondition (not (String (L"20") < String (L"11")));
	TestCondition (not (String (L"20") <= String (L"11")));
	TestCondition (String (L"11") < String (L"20"));
	TestCondition (String (L"11") <= String (L"20"));
	TestCondition (not (String (L"11") > String (L"20")));
	TestCondition (not (String (L"11") >= String (L"20")));

	TestCondition (String (L"aac") > String (L"aab"));
	TestCondition (String (L"aac") >= String (L"aab"));
	TestCondition (not (String (L"aac") < String (L"aab")));
	TestCondition (not (String (L"aac") <= String (L"aab")));

	TestCondition (String (L"apple") < String (L"apples"));
	TestCondition (String (L"apple") <= String (L"apples"));
	TestCondition (not (String (L"apple") > String (L"apples")));
	TestCondition (not (String (L"apple") >= String (L"apples")));
}

void    TestReadOnlyStrings ()
{
    String_ReadOnlyChar s (L"fred");
    TestCondition (s[0] == 'f');
    s.SetLength (3);
    TestCondition (s[0] == 'f');
    TestCondition (s.GetLength () == 3);
    s += L"x";
    TestCondition (s.GetLength () == 4);
    TestCondition (s[3] == 'x');
}
