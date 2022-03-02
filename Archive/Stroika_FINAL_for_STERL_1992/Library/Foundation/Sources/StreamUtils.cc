/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/StreamUtils.cc,v 1.11 1992/11/12 08:15:09 lewis Exp $
 *
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: StreamUtils.cc,v $
 *		Revision 1.11  1992/11/12  08:15:09  lewis
 *		#if 0'd real stuff for reading writing reals.
 *
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.8  1992/09/11  13:14:58  lewis
 *		Get rid of newline. Use endl instead.
 *
 *		Revision 1.7  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.5  1992/07/08  01:29:57  lewis
 *		Use RequireNotReached instead of SwitchFallThru.
 *
 *		Revision 1.4  1992/07/03  07:10:52  lewis
 *		Changed gcc compiler ifdefs from qGCC to qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
 *		and added bug defined to Config-Foundation.hh.
 *
 *		Revision 1.3  1992/06/22  15:44:39  lewis
 *		Commented out two procedures which called blen () - not defined in gcc implmentation
 *		of iostream. Fix later.
 *
 *		Revision 1.2  1992/06/20  19:33:34  lewis
 *		Hack around gcc troubles under aux. Mostly having to do with manipulators. Not sure whose right???
 *
 *		Revision 1.16  1992/03/26  17:02:51  lewis
 *		Worked around maybe funky bug with XXStringStream::opererator String () - sterl reported.
 *
 *		Revision 1.14  1992/02/18  05:09:04  lewis
 *		Add workaround for qHPCPlus_StreamLibTrompingAtEndBug.
 *
 *		Revision 1.13  1992/01/27  06:15:05  lewis
 *		Got rid of old stream support, and 2.0 suppport.
 *
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if 	qMacOS
#include	<Types.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"

#include	"Math.hh"
#include	"StreamUtils.hh"






/*
 ********************************************************************************
 ********************************* StringStreamBuf ******************************
 ********************************************************************************
 */
StringStreamBuf::StringStreamBuf ():
	streambuf (),
	fBlockSize (32)
{
}
	
StringStreamBuf::StringStreamBuf (char* str, int size, char* pStart):
	streambuf (),
	fBlockSize (32)
{
	RequireNotNil (str);
	Require (size >= 0);
	RequireNotNil (pStart);
	
	int	length =  (size == 0) ? strlen (str) : size;
	char*	newString = new char [length];
	memcpy (newString, str, length);

	setb (str, str + length);

	AssertNotNil (base ());
	Assert (base () == newString);
	Assert (pStart <= ebuf ());

	setp (pStart, ebuf());
	setg (base (), base (), ebuf());
}

StringStreamBuf::~StringStreamBuf()
{
	delete base ();
}
		
char*	StringStreamBuf::Peek ()
{
	return (base ());
}

int		StringStreamBuf::doallocate ()
{
	char* newspace =  new char[fBlockSize];

	if (newspace == Nil) return EOF;

	setb (newspace, newspace+fBlockSize, 0);

	setp (newspace,newspace+fBlockSize);
	setg (newspace,newspace,newspace);

	return 0;
}

int		StringStreamBuf::overflow (int c)
{
#if		qGCC
	AssertNotImplemented ();
#else
	if (base () == Nil) {
		allocate();
		if (base () == Nil) {
			return EOF;
		}
	}
	else if (epptr() > (pptr() + 1)) {
		// There is space already available so we
		// don't have to do anything.  This is presumably
		// triggered by an explicit flush or something like
		// that. 
		}
	else {
		size_t inserted = pptr() - base (); // number of chars previously inserted into buffer
		size_t extracted=gptr() - base ();  // number of chars previously extracted from buffer

		// after we copy chars from current space to a new
		// (larger) area we have to adjust pointers to take into
		// acount previous activities.

		static const size_t increment = 2; 	
		size_t newsize = increment * blen () + 4;
		if (newsize < fBlockSize) {
			newsize = fBlockSize;
		}
		
		char* newspace = new char[newsize];

		if (newspace == Nil) return EOF;

		memcpy (newspace, base (),inserted);

		delete base ();

		setb (newspace, newspace+newsize, 0);

		setp (base ()+inserted, ebuf());
		setg (base (), base ()+extracted, pptr());
	}
	if (c != EOF) {
		return sputc (c);
	}
#endif
	return (EOF);
}

int		StringStreamBuf::underflow ()
{
	if (pptr () == Nil) return EOF;
	if (egptr () == Nil) return EOF;

	setg (base (), egptr (), pptr ());
	
	if (egptr () <= gptr ())	{
		return EOF;
	}
	
	return *gptr ();
}

streambuf* StringStreamBuf::setbuf (char*  p, int /*l*/)
{
	if (p == Nil) {
		return this;
	}
	return Nil;
}

streampos	StringStreamBuf::seekoff (streamoff o, ios::seek_dir d, int m)
{
#if		qGCC
	AssertNotImplemented ();
#else
	switch (d) {
		case ios::beg: 
			break;
		case ios::cur:
			if (m & ios::in) {
				o += gptr() - base ();
			}
			else  {
				o += pptr() - base ();
			}
			break;
		case ios::end:
			if ((gptr() < egptr()) and (egptr() > pptr())) {
				o += egptr() - base ();
			}
			else if (pptr() < epptr()) {
				o += pptr() - base ();
			}
			break;
		default:	
			RequireNotReached ();
	}
	if ((o < 0) or (o >= blen ())) {
		return streampos (EOF);
	}
	if (m & ios::in) {
		setg (base (), base ()+o, egptr ());
	}
	if (m & ios::out) {
		setp (base () + o, epptr ());
	}
#endif
	return o;
}








/*
 ********************************************************************************
 *********************************** OStringStream ******************************
 ********************************************************************************
 */
OStringStream::OStringStream ():
	ostrstream ()
{
}

OStringStream::OStringStream (char* str, int size, int mode):
	ostrstream (str, size, mode)
{
}

OStringStream::	operator String ()
{
// I think this can happen if we insert nothing?? LGP March 26, 1992
if (tellp () == -1) {
	return ("");
}

	String	s	=	String (str(), size_t (tellp ()));
	AssertNotNil (rdbuf ());
	rdbuf ()->freeze (0);		// say not referencing str anymore

#if		qHPCPlus_StreamLibTrompingAtEndBug
	for (size_t i = s.GetLength (); i > 0; i--) {
		if (s[i] == 0) {
			Assert (i >= 1);
			s.SetLength (i-1);
		}
	}
#endif

	return (s);
}






/*
 ********************************************************************************
 *********************************** StringStream *******************************
 ********************************************************************************
 */

StringStream::StringStream ():
	strstream ()
{
}

StringStream::StringStream (char* str, int size, int mode):
	strstream (str, size, mode)
{
}

StringStream::	operator String ()
{
// I think this can happen if we insert nothing?? LGP March 26, 1992
if (tellp () == -1) {
	return ("");
}

	String	s	=	String (str(), size_t (tellp ()));
	AssertNotNil (rdbuf ());
	rdbuf ()->freeze (0);		// say not referencing str anymore

#if		qHPCPlus_StreamLibTrompingAtEndBug
	for (size_t i = s.GetLength (); i > 0; i--) {
		if (s[i] == 0) {
			Assert (i >= 1);
			s.SetLength (i-1);
		}
	}
#endif

	return (s);
}








/*
 ********************************************************************************
 ***************************** Boolean Insert/Extractors ************************
 ********************************************************************************
 */

ostream&	operator<< (ostream& out, Boolean b)
{
	out << ((b) ? 'T' : 'F');
	return (out);
}

istream&	operator>> (istream& in, Boolean& b)
{
	char c;
	in >> c;
	switch (c) {
		case	'T':
		case	't':	b = True; break;
		case	'F':
		case	'f':	b = False; break;
		default:		in.clear (ios::badbit | in.rdstate ());  break;
	}
	return (in);
}





#if		0

/*
 ********************************************************************************
 *************************** Real Number Insert/Extractors **********************
 ********************************************************************************
 */

ostream&	WriteReal (ostream& out, const Real& r)
{
	if (IsBadNumber (r)) {
		out << '?';
	}
	else {
		out << '#' << r;
	}
	return (out);
}

istream&	ReadReal (istream& in, Real& r)
{
	char c;
	in >> c;
	switch (c) {
		case '?':
			r = kBadNumber;
			break;
			
		case '#':
			in >> r;
			break;

		default:		
			in.clear (ios::badbit | in.rdstate ());  
			break;
	}
	return (in);
}
#endif







/*
 ********************************************************************************
 *********************************** Read/Write String **************************
 ********************************************************************************
 */

ostream&	WriteString (ostream& out, const String& s)
{
	out << s.GetLength () << ':' << s;
	return (out);
}

istream&	ReadString (istream& in, String& s)
{
	String			temp;
	size_t	length;

	in >> length;
	temp.SetLength (length);
	char c;
	in >> c;
	if (!in or (c != ':')) {
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	for (register size_t i = 1; i <= length; i++) {
		in.get (c);
		temp.SetCharAt (c, i);
	}
	s = temp;
	return (in);
}









/*
 ********************************************************************************
 *********************************** Read/Write Bytes ***************************
 ********************************************************************************
 */

/*
 * I would like to have read/write bytes work with hex data, but for some reason does
 * not work.  Decimal data looks worse, but does work.
 */
ostream&	WriteBytes (ostream& out, size_t size, const void* data)
{
	/*
	 * write out in binary format...
	 */

	out << lparen;
	out << size << ":" << endl << tab;

	long	oldFlags	=	out.flags();
	for (size_t i = 1; i <= size; i++) {
		out << UInt32 (((UInt8*)data)[i-1]);	// write out #
		if (i != size) {
			out << " ";		// not sure extractor for int will skip...
		}
		if (i % 20 == 0) {	// a bit of formatting...
			out << endl << tab;
		}
	}
	(void)out.flags (oldFlags);
	out << endl << tab << rparen;

	return (out);
}

istream&	ReadBytes (istream& in, size_t& size, void*& data)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	size_t	resultingSize;
	in >> resultingSize;

	char c;
	in >> c;
	if (!in or (c != ':')) {
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	UInt8*	resultingData	=	new UInt8 [resultingSize];

	// now do forloop reading bytes in, decoding, and appending to array...
	for (size_t i = 1; i <= resultingSize; i++) {
		UInt32	x;
		in >> x;
		resultingData [i-1] = UInt8 (x);
	}

	in >> ch;
	if (!in or (ch != rparen)) {				// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		delete (resultingData);
		return (in);
	}

	/*
	 * We win.
	 */
	Assert (in);
	data = resultingData;
	size = resultingSize;
	return (in);
}







/*
 ********************************************************************************
 ******************************************* tab ********************************
 ********************************************************************************
 */

ostream&	tab (ostream& out)
{
	return (out << '\t');
}

#if		!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
OMANIP(int)	tab(int n)	
{ 
	return OMANIP(int) (tab, n); 
}
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/

ostream&	tab (ostream& out, int n) 
{
	for (int i = 1; i <= n; i++) {
		out << '\t';
	}
	return (out);
}








/*
 ********************************************************************************
 ************************************** space ***********************************
 ********************************************************************************
 */

#if		!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
OMANIP(int)	space(int n)	
{ 
	return OMANIP(int) (space, n); 
}
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/

ostream&	space (ostream& out, int n) 
{
	for (int i = 1; i <= n; i++) {
		out << ' ';
	}
	return (out);
}




/*
 ********************************************************************************
 *************************************** quote **********************************
 ********************************************************************************
 */
ostream&	quote (ostream& out)
{
	return (out << '"');
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


