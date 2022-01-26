/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Math.cc,v 1.7 1992/11/12 08:14:30 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Math.cc,v $
 *		Revision 1.7  1992/11/12  08:14:30  lewis
 *		Get rid of Real based stuff - kBadNumber is IsBadNumber () - if 0'd out.
 *
 *		Revision 1.6  1992/09/11  13:13:29  lewis
 *		Got rid of unneeded includes, and use endl, instead of old newline.
 *
 *		Revision 1.5  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/08  01:20:11  lewis
 *		Change SwitchFallThru () to AssertNotReached  ().
 *
 *		Revision 1.2  1992/07/01  03:48:39  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.14  92/05/21  17:10:33  17:10:33  sterling (Sterling Wight)
 *		Lewis/Sterl marathon rewrite of UInt64, and comment out for now of Int64.
 *		Most of the work was on division, and its still pretty bad, and slow, but
 *		I believe it may work. More to be done on testsuite and I may have some
 *		confidence.
 *		
 *		Revision 1.13  92/04/16  09:45:09  09:45:09  lewis (Lewis Pringle)
 *		Moved random stuff to Random.cc
 *		
 *		Revision 1.12  92/04/15  13:54:29  13:54:29  lewis (Lewis Pringle)
 *		Add random number facility (closely based on ANSI rand()).
 *		
 *		Revision 1.10  1992/03/11  23:08:41  lewis
 *		Use qLongLongAvailable instead of qGPlus
 *
 *
 */



#include	<stdlib.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<SANE.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"

#include	"Math.hh"


Exception	gMathException;
Exception	gMathRangeException		=		gMathException;





#if		0
#if		qMacOS
const	Real	kBadNumber	=	nan (0);		// not quite sure what # is right here?
#else
/*
 * There is no really good answer to this, unless you have a nan. But, for systems that dont,
 * kMaxSmallReal is a pretty good answer. It is unlikely to conflict with any real choice of numbers,
 * and will not accedently have its precision lost (thats why we dont use kMaxReal) when temporarily converted
 * to a float in some computation.
 */
const	Real	kBadNumber	=	kMaxSmallReal;
#endif
#endif

#if		0
Boolean	IsBadNumber (const double& r)
{
#if		qMacOS
	numclass 	nc	=	classextended (r);
	switch (nc) {
		case	SNAN:			return (True);
		case	QNAN:			return (True);
		case	INFINITE:		return (True);
		case	ZERONUM:		return (False);
		case	NORMALNUM:		return (False);
		case	DENORMALNUM:	return (False);
		default:				AssertNotReached ();return (False);
	}
#else   /* !qMacOS */
	return Boolean (r == kBadNumber);
#endif	/*qMacOS*/
}
#endif







/*
 ********************************************************************************
 ************************************* UInt64 ***********************************
 ********************************************************************************
 */

#if		!qLongLongAvailable

const	UInt64&	UInt64::operator<<= (unsigned by)
{
	for (int i = 3 ; i >= 0 ; i--) {
		int	whichDigit = i - by / (8*sizeof (fDigit));
		if (whichDigit < 0) {
			fDigit [i] = 0;
		}
		else {
			fDigit [i] = (fDigit [whichDigit] << (by % (8*sizeof (fDigit))));
		}
	}
	return (*this);
}

const	UInt64&	UInt64::operator>>= (unsigned by)
{
	for (int i = 0 ; i <= 3 ; i++) {
		int	whichDigit = i + by / (8*sizeof (fDigit));
		if (whichDigit > 3) {
			fDigit [i] = 0;
		}
		else {
			fDigit [i] = (fDigit [whichDigit] >> (by % (8*sizeof (fDigit))));
		}
	}
	return (*this);
}

const	UInt64&	UInt64::operator+= (const UInt64& right)
{
	UInt32 s = 0;
	for (int i = 0 ; i < 4 ; i++) {
		s += fDigit[i];
		s += right.fDigit[i];
		fDigit[i] = UInt16 (s);
		s >>= 16;
	}
	//if (s) throw (gOverflowException);
	return (*this);
}

const	UInt64&	UInt64::operator-= (const UInt64& right)
{
	Int32 s = 0;
	for (int i = 0; i < 4; i++) {
		s += fDigit[i];
		s -= right.fDigit[i];
		fDigit[i] = UInt16 (s);
		s >>= 16;
	}
	//if (s) throw (gOverflowException);
	return (*this);
}

const	UInt64&	UInt64::operator*= (const UInt64& right)
{
	UInt64	result = UInt32(0);
	UInt32	s;
	int 	i, j;

	for (i = 0 ; i < 4 ; i++) {
		s = 0;
		for (j = 0 ; j < 4 ; j++) {
			s += UInt32 (fDigit[i]) * UInt32 (right.fDigit[j]);
			if (i + j < 4) {
				s += UInt32 (result.fDigit [i+j]);
				result.fDigit [i+j] = UInt16 (s);
				s >>= 16;
			}
//          else if (s != 0) throw (gOverflowException);
		}
	}

	*this = result;
	return (*this);
}

const UInt32 kUInt64Base = UInt32(1)<<16;

static	const	kUInt64Zero	=	0;
const	UInt64&	UInt64::operator/= (const UInt64& divisor)
{
	// first handle special cases - probably should be ignored cuz would be on real hardware
	// implementation???
	if (divisor == kUInt64Zero) {
		//throw (gZeroDivideException);
		*this = kUInt64Zero;
		return (*this);
	}

	if (&divisor == this) {
		*this = 1;
		return (*this);
	}
	if ((divisor > *this) or (*this == kUInt64Zero)) {
		*this = kUInt64Zero;
		return (*this);
	}


	/*
	 * scan divisor to find top 'digit' of the divisor
	 */
	for (int highOrderDigit = 3; (divisor.fDigit[highOrderDigit] == 0) and (highOrderDigit >= 1) ; highOrderDigit--)
		;

	/*
	 * Most significant 16 bits of the full divisor. This will be used to do repeated
	 * multiply/subtractions.
	 */
	UInt32	divisorTop = divisor.fDigit [highOrderDigit] << 16;
	if (highOrderDigit != 0) {
		divisorTop += divisor.fDigit [highOrderDigit-1];
	}
	for (int divisorTopShift = 0 ; (divisorTop > kUInt64Base) and (divisorTopShift <= 16);
		 divisorTop >>= 1, divisorTopShift++)
		;

	divisorTopShift = 16 - divisorTopShift;	// amount our divisor has been multiplied by


	/*
	 * Now iterate over the digits of the dividend from high order to low order, and
	 * keep subtracting off from the remainder multiples of the divisor, and update
	 * the quotient with the factors applied to the divisor in subtracting from the
	 * remainder (note: here we must take into account the fact that the divisorTop was
	 * shifted).
	 */
	UInt64	remainder	=	*this;
	UInt64	quotient	=	0;

	for (int k = 3 ; k >= highOrderDigit ; k--) {
		/*
		 * the trialQuotient is the quotient of the current digit (with sluff from the next lower
		 * digit) divided by the divisorTop (which is a multiple of the divisor). We put the number
		 * into a UInt32 so that the remainder of the division can be found in the low word of the
		 * trial quotient.
		 */
		UInt32	trialQuotient = UInt32 (remainder.fDigit [k]) << 16;
		if (k >= 1) {
			trialQuotient += fDigit[k-1];
		}

		/*
		 * If trail quotient less then trailDivisor, we'll just get zeros, so save some time, and
		 * try lower order digits.
		 */
// hack for now and do zero - fix optimization to be better later
//		if (trialQuotient < divisor) {
//			continue;
//		}
if (trialQuotient == 0) {continue;}



		trialQuotient /= divisorTop;		// Trial quotient


		/*
		 * Now check our guess.
		 *
		 * Multiply the divisor by the trial quotient.
		 */
Again:
		UInt16	smallTrialQuotient	=	(trialQuotient>>(16-divisorTopShift));
		UInt16	smallTrialRemainder	=	(trialQuotient << divisorTopShift);
		UInt64	trialResult			=	smallTrialQuotient * divisor;
		UInt64	diff				=	remainder - trialResult;


		if (diff >= 0) {
//			if (diff > divisor) {
//				trialQuotient++;
//gDebugStream << "incremting trail quotient and trying again " << trialQuotient << endl;
//				goto Again;
//			}

			/*
			 * Good choice of trialQuotient.
			 */
//			remainder -= trialResult;
//			remainder -= smallTrialRemainder;

// update of quotient TERRIBLE
			quotient.fDigit [k-highOrderDigit] += smallTrialQuotient;
			if (k >= highOrderDigit) {
				quotient.fDigit [k-highOrderDigit-1] += smallTrialRemainder;
			}

remainder = *this - quotient*divisor;
		}
		else {
			/*
			 * We missed, and could have been off by one. Can we ever be off by more ?
			 * I don't see how. Assert not, and investigate later.
			 */
			Assert (trialQuotient != 0);
			trialQuotient--;
			goto Again;
		}
	}


	*this = quotient; /* Here is the side effect. */
	return (*this);
}

const	UInt64&	UInt64::operator %= (const UInt64& right)
{
	// Could do better. Lots...
	*this -= (*this / right) * right;
	Ensure (*this < right);
	return (*this);
}

UInt32	UInt64::Low32 () const
{
	return ((fDigit [1] << 16) + fDigit [0]);
}

Boolean	operator== (const UInt64& left, const UInt64& right)
{
	for (int i = 0 ; i < 4 ; i++) {
		if (left.fDigit[i] != right.fDigit[i]) {
			return (False);
		}
	}
	return (True);
}

Boolean operator!= (const UInt64 left, const UInt64 right)
{
	return (Boolean (not (right == left)));
}

Boolean operator< (const UInt64& left, const UInt64& right)
{
	for (int i = 3 ; i >= 0 ; i--) {
		if (right.fDigit[i] > left.fDigit[i]) {
			return (True);
		}
	}
	return (False);
}

Boolean operator<= (const UInt64 left, const UInt64 right)
{
	return (Boolean((left < right) or (left == right)));
}

Boolean operator> (const UInt64 left, const UInt64 right)
{
	return (not (left <= right));
}

Boolean operator>= (const UInt64 left, const UInt64 right)
{
	return (not (left < right));
}

UInt64	operator<< (const UInt64 left, unsigned by)
{
	UInt64 t = left;
	t <<= by;
	return (t);
}

UInt64	operator>> (const UInt64 left, unsigned by)
{
	UInt64 t = left;
	t >>= by;
	return (t);
}

UInt64	operator+ (const UInt64 left, const UInt64 right)
{
	UInt64 t = left;
	t += right;
	return (t);
}

UInt64	operator- (const UInt64 left, const UInt64 right)
{
	UInt64 t = left;
	t -= right;
	return (t);
}

UInt64	operator* (const UInt64 left, const UInt64 right)
{
	UInt64 t = left;
	t *= right;
	return (t);
}

UInt64	operator/ (const UInt64 left, const UInt64 right)
{
	UInt64 t = left;
	t /= right;
	return (t);
}

UInt64	operator% (const UInt64 left, const UInt64 right)
{
	UInt64 t = left;
	t %= right;
	return (t);
}

ostream&	operator<< (ostream& out, UInt64 s)
{
#if 0
	out << "(";
	for (int i = 3 ; i >= 0 ; i--) {
		out << s.fDigit[i];
		if (i != 0) {
			out << " ";
		}
	}
	out << ")";
#endif
	const	kBase = 10;			// could write in different bases if anyone cared...
	UInt64	tmp	=	s;
	char 	buffer [64];		// really just has to be bigger than 64 decimal digits - in any base...
	char*	cur	=	buffer;
	while (s != 0) {
		*cur++ = ('0' + (s % 10).Low32 ());
		s /= 10;
	}

	// now output the characters in reverse order, since they were put in, high order first.
	// Also, special case zero which would have been Nil string.
	if (cur == buffer) {
		out << '0';
	}
	else {
		while (cur > buffer) {
			out << *--cur;
		}
	}
	return (out);
}

istream&	operator>> (istream& in, UInt64& s)
{
	AssertNotImplemented ();
	return (in);
}

#endif	/*!qLongLongAvailable*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

