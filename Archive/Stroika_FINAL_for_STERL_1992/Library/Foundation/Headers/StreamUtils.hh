/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__StreamUtils__
#define	__StreamUtils__

/*
 * $Header: /fuji/lewis/RCS/StreamUtils.hh,v 1.8 1992/11/12 08:12:35 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: StreamUtils.hh,v $
 *		Revision 1.8  1992/11/12  08:12:35  lewis
 *		Get rid of Real read/write stuff (#if 0'd out).
 *
 *		Revision 1.7  1992/09/11  13:03:24  lewis
 *		Get rid of newline function. Use endl instead.
 *
 *		Revision 1.6  1992/07/22  02:30:56  lewis
 *		Make paren/brace constants be char rather than the new type Character.
 *
 *		Revision 1.5  1992/07/03  07:09:24  lewis
 *		Got rid of opertor << () sterl had commented that broke apples compiler since I saw
 *		no reason for them in the first place.
 *		Commented back in Read/Write reals.
 *		Changed qGCC compile ifdef to qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport.
 *
 *		Revision 1.4  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.2  1992/06/20  19:25:21  lewis
 *		Some tmp hacks to get things to compile with gcc-aux.
 *
 *		Revision 1.12  1992/03/11  23:06:36  lewis
 *		Got rid of old support for qGPlus.
 *
 *		Revision 1.10  1992/01/27  06:12:07  lewis
 *		Got rid of old stream lib support.
 *
 *
 *
 */

#include	<iostream.h>
#include	<iomanip.h>
#include	<strstream.h>

#include	"String.hh"



const	char	lbrace	=	'{';	// references in lippman in ioconst.h - not sure why missing?
const	char	rbrace	=	'}';

const	char	lparen	=	'(';	// refrences in lippman in ioconst.h - not sure why missing?
const	char	rparen	=	')';




class	StringStreamBuf : public streambuf {
	public:
		StringStreamBuf ();
		StringStreamBuf (char* str, int size, char* pStart);
		~StringStreamBuf ();

		override int		doallocate ();
		override int		overflow (int);
		override int		underflow ();
		override streambuf* setbuf (char*  p, int l);
		override streampos	seekoff (streamoff,ios::seek_dir,int);

		nonvirtual	char*	Peek ();

	private:
		Int16	fBlockSize;
};


class	OStringStream : public ostrstream {
	public:
		OStringStream ();
		OStringStream (char* str, int size, int mode);

		operator String ();
};

class	StringStream : public strstream {
	public:
		StringStream ();
		StringStream (char* str, int size, int mode);

		operator String ();
};



extern	ostream&	operator<< (ostream& out, Boolean b);
extern	istream&	operator>> (istream& in, Boolean& b);

#if	0
extern	ostream&	WriteReal (ostream& out, const Real& r);
extern	istream&	ReadReal (istream& in, Real& r);
#endif

#if		0 && !qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
// fails with gcc - needs more research - also ask sterl what he's really trying to do here...
typedef	Real&			RealRef;
typedef	const Real&		ConstRealRef;

IOMANIPdeclare(RealRef);
IOMANIPdeclare(ConstRealRef);

OMANIP(ConstRealRef)	WriteReal (const Real& r);
IMANIP(RealRef)			ReadReal (Real& r);

inline	OMANIP(ConstRealRef)	WriteReal (const Real& r)
{
	return OMANIP(ConstRealRef) (WriteReal, r); 
}

inline	IMANIP(RealRef)		ReadReal (Real& r)
{
	return IMANIP(RealRef) (ReadReal, r); 
}
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/



extern	ostream&	WriteString (ostream& out, const String& s);
extern	istream&	ReadString (istream& in, String& s);

#if		!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
// really just a compiler bug, possibly only in Apple's version, prohibits use with earlier CFront
typedef	String&			StringRef;
typedef	const String&	ConstStringRef;

IOMANIPdeclare(StringRef);
IOMANIPdeclare(ConstStringRef);

OMANIP(ConstStringRef)	WriteString (const String& s);
IMANIP(StringRef)		ReadString (String& s);

inline	OMANIP(ConstStringRef)	WriteString (const String& s)
{
	return OMANIP(ConstStringRef) (WriteString, s); 
}

inline	IMANIP(StringRef)		ReadString (String& s)
{
	return IMANIP(StringRef) (ReadString, s); 
}
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/

extern	ostream&	WriteBytes (ostream& out, size_t size, const void* data);
extern	istream&	ReadBytes (istream& in, size_t& size, void*& data);


extern	ostream&	quote (ostream& out);

extern	ostream&	tab (ostream& out);
extern	ostream&	tab (ostream& out, int n);
#if		!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
extern	OMANIP(int)	tab(int n);	
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/

extern	ostream&	space (ostream& out, int n);
#if		!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
extern	OMANIP(int)	space(int n);
#endif	/*!qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__StreamUtils__*/
