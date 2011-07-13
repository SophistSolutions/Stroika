/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SetTests.cc,v 1.11 1992/11/26 02:54:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SetTests.cc,v $
 *		Revision 1.11  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.10  1992/10/19  20:57:03  lewis
 *		With debug off, use larger testsize.
 *
 *		Revision 1.9  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.8  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.6  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.3  1992/09/04  01:12:36  lewis
 *		Deleted extra implements. We get these from CollectionTests.
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"SetTests.hh"


#define		qPrintTimings		!qDebug





#if		qRealTemplatesAvailable
void	SetIteratorTests (Set<UInt32>& s)
#else
void	SetIteratorTests (Set(UInt32)& s)
#endif
{
#if		qDebug
	const	int	kTestSize	=	10;
#else
	const	int	kTestSize	=	500;
#endif
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}

		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			TestCondition (s.Contains (i));
			TestCondition (s.GetLength () == kTestSize - i + 1);
			s.Remove (i++);
			TestCondition (s.GetLength () == kTestSize - i + 1);
			TestCondition (not s.Contains (i-1));
			if (i == kTestSize+1) {
				Assert (s.IsEmpty ());
			}
		}
		TestCondition (s.IsEmpty ());
		TestCondition (s.GetLength () == 0);

		for (i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	kTestSize;
		ForEach (UInt32, it1, s) {
			TestCondition (s.Contains (i));
			s.Remove (i--);
			TestCondition (not s.Contains (i+1));
		}
		TestCondition (s.GetLength () == kTestSize/2);
		
		s.RemoveAll ();
		TestCondition (s.IsEmpty ());
		for (i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		ForEach (UInt32, it2, s) {
			s.Remove (it2.Current ());
		}
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		s.RemoveAll ();
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		ForEach (UInt32, it, s) {
		  ForEach (UInt32, it2, s) {
			ForEachT (SetMutator, UInt32, it3, s) {
					it3.RemoveCurrent ();
					s.Add (1);
					s.Remove (1);
				}
			}
		}
	}
}

#if		qRealTemplatesAvailable
void	TimedSetTests (Set<UInt32>& s)
#else
void	TimedSetTests (Set(UInt32)& s)
#endif
{
}

#if		qRealTemplatesAvailable
void	SimpleTests (Set<UInt32>& s)
#else
void	SimpleTests (Set(UInt32)& s)
#endif
{
	TimedSetTests (s);
	
#if		qRealTemplatesAvailable
	Set<UInt32>	s1 (s);
#else
	Set(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Set<UInt32>	s2 = s1;
#else
	Set(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (1);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	SetIteratorTests (s);
}

#if		qRealTemplatesAvailable
void	SimpleTests (Set<SimpleClass>& s)
#else
void	SimpleTests (Set(SimpleClass)& s)
#endif
{
#if		qRealTemplatesAvailable
	Set<SimpleClass>	s1 (s);
#else
	Set(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Set<SimpleClass>	s2 = s1;
#else
	Set(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (5);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

