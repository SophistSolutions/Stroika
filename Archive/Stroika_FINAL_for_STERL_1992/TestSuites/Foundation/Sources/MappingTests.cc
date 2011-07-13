/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MappingTests.cc,v 1.14 1992/12/04 19:31:51 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MappingTests.cc,v $
 *		Revision 1.14  1992/12/04  19:31:51  lewis
 *		Get rid of >> - space out for templates to avoid compiler
 *		confusion.
 *
 *		Revision 1.13  1992/12/04  19:30:39  lewis
 *		Fix some missing <> for qRealTemplatesAvialable, and move static
 *		functions to bottom of file.
 *
 *		Revision 1.12  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.11  1992/10/10  04:39:54  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.9  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.5  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cout - use cout.
 *
 *		Revision 1.2  1992/09/04  01:11:48  lewis
 *		Deleted extra implements. We get these from CollectionTests.
 *
 *
 */


#include	"Debug.hh"
#include	"Format.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"MappingTests.hh"


#define		qPrintTimings		!qDebug





#if		qRealTemplatesAvailable
static	void	PopulateMap (Mapping<SimpleClass, UInt32>& m, size_t amountToAdd);
static	void	MappingIteratorTests (Mapping<SimpleClass, UInt32>& m, size_t testSize);
#else
static	void	PopulateMap (Mapping(SimpleClass, UInt32)& m, size_t amountToAdd);
static	void	MappingIteratorTests (Mapping(SimpleClass, UInt32)& m, size_t testSize);
#endif




#if		qRealTemplatesAvailable
void	SimpleMappingTests (Mapping<SimpleClass, UInt32>& m)
#else
void	SimpleMappingTests (Mapping(SimpleClass, UInt32)& m)
#endif
{
#if		qRealTemplatesAvailable
	Mapping<SimpleClass, UInt32>	s1 (m);
#else
	Mapping(SimpleClass, UInt32)	s1 (m);
#endif
	TestCondition (s1 == m);
	TestCondition (s1 == m);
#if		qRealTemplatesAvailable
	Mapping<SimpleClass, UInt32>	s2 = s1;
#else
	Mapping(SimpleClass, UInt32)	s2 = s1;
#endif
	TestCondition (s2 == m);
	TestCondition (s2 == s1);
	s2.Enter (SimpleClass (5), 1);
	TestCondition (s1 == m);
	TestCondition (s2 != s1);

	
	#if qDebug
		const	int	kTestSize	=	20;
	#else
		const	int	kTestSize	=	200;
	#endif
	

	const int loopLength = kTestSize/20;
	UInt32 i;

	#if		qPrintTimings
		Time t = GetCurrentTime ();
		cout << tab << "timing Mapping<SimpleClass,UInt32>..." << endl;
	#endif

	PopulateMap (m, kTestSize);
	TestCondition (m.GetLength () == kTestSize);

	SimpleClass	offTheEnd = kTestSize+1;
	for (i = 1; i <= kTestSize; i++) {
		SimpleClass s = i;
		UInt32 temp = 0;
		for (int j = 1; j <= loopLength; j++) {
			TestCondition (m.Lookup (s, &temp));
			TestCondition (not m.Lookup (0, &temp));
			TestCondition (not m.Lookup (offTheEnd, &temp));
		}
	}

	TestCondition (m.GetLength () == kTestSize);
	for (i = 1; i <= kTestSize; i++) {
		m.Remove (i);
		TestCondition (m.GetLength () == (kTestSize-i));
	}
	TestCondition (m.IsEmpty ());

	PopulateMap (m, kTestSize);
	TestCondition (m.GetLength () == kTestSize);

	for (i = 1; i <= kTestSize; i++) {
		SimpleClass s = i;
		UInt32 temp = 0;
		m.RemoveAt (s);
		TestCondition (not m.Lookup (s, &temp));
	}
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished timing Mapping<SimpleClass,UInt32>; time elapsed = " << t << endl;
#endif
}







#if		qRealTemplatesAvailable
static	void	PopulateMap (Mapping<SimpleClass, UInt32>& m, size_t amountToAdd)
#else
static	void	PopulateMap (Mapping(SimpleClass, UInt32)& m, size_t amountToAdd)
#endif
{
	size_t x = amountToAdd/10;
	for (size_t j = 10; j > 5; j--) {
		for (size_t i = 1; i <= x; i++) {
			SimpleClass s = i+(j-1)*x;
			m.Enter (s, i+(j-1)*x);
		}
	}
	for (j = 1; j <= 5; j++) {
		for (size_t i = x; i > 0; i--) {
			SimpleClass s = i+(j-1)*x;
			m.Enter (s, i+(j-1)*x);
		}
	}
}

#if		qRealTemplatesAvailable
static	void	MappingIteratorTests (Mapping<SimpleClass, UInt32>& m, size_t testSize)
#else
static	void	MappingIteratorTests (Mapping(SimpleClass, UInt32)& m, size_t testSize)
#endif
{
	m.RemoveAll ();

	{
		PopulateMap (m, testSize);
		TestCondition (m.GetLength () == testSize);
#if		qRealTemplatesAvailable
		for (Iterator<MapElement<SimpleClass,UInt32> > it (m); it.More (); ) {
			for (Iterator<MapElement<SimpleClass,UInt32> > it1 (m); it.More (); ) {
				ForEach (SimpleClass, it2, m) {
					m.RemoveAt (it2.Current ());
				}
			}
		}
#else
		ForEach (MapElement(SimpleClass,UInt32), it, m) {
			ForEach (MapElement(SimpleClass,UInt32), it1, m) {
				ForEach (SimpleClass, it2, m) {
					m.RemoveAt (it2.Current ());
				}
			}
		}
#endif
		TestCondition (m.GetLength () == 0);
	}

	{
		PopulateMap (m, testSize);	
		TestCondition (m.GetLength () == testSize);
		size_t index = 1;
		ForEach (SimpleClass, it, m) {
			if ((index % 3) == 0) {
				m.RemoveAt (it.Current ());
			}
			else if ((index % 5) == 0) {
				SimpleClass s = index;
				m.Enter (s, index);
			}
			index++;
		}
		TestCondition (m.GetLength () == 0);
	}

	{
		PopulateMap (m, testSize);	
		TestCondition (m.GetLength () == testSize);
		size_t index = 0;
		ForEach (SimpleClass, it, m) {
			m.RemoveAll ();
			index++;
		}
		TestCondition (index == 1);
		TestCondition (m.GetLength () == 0);
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

