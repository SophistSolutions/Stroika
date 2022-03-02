/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Character__
#define	__Character__

/*
 * $Header: /fuji/lewis/RCS/Character.hh,v 1.7 1992/12/10 05:52:42 lewis Exp $
 *
 * Description:
 *
 * Design Notes:
 *
 *
 * TODO:
 *
 * Changes:
 *	$Log: Character.hh,v $
 *		Revision 1.7  1992/12/10  05:52:42  lewis
 *		Move this a little closer to what we eventually want. Add more character
 *		code and character set support.
 *		KDJ seems to think this whole thing is a crock. Discuss with him and Tom.
 *
 *		Revision 1.6  1992/12/07  04:07:50  lewis
 *		Added stream inserter/extractor for hello world demo.
 *
 *		Revision 1.4  1992/09/11  15:00:39  sterling
 *		added op==, op< etc
 *
 *		Revision 1.2  1992/07/22  02:29:48  lewis
 *		Tried to make the Character a real class, with I strong - temporary
 *		- eye toward backward compatability. Added CTOR for ascii char, and
 *		GetAsciiCode () method.
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Language.hh"




typedef	UInt16	CharacterSet;	// qCFront_NestedTypesHalfAssed && qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
								// etc...
class	Character	{
	public:
		enum {
			eAscii,
			eISOLatin1,
		};
		Character (char c);
		Character (wchar_t wc);
		Character (wchar_t wc, CharacterSet cSet);

		nonvirtual	wchar_t			GetCharacterCode () const;
		nonvirtual	CharacterSet	GetCharacterSet () const;


		nonvirtual	char	GetAsciiCode () const;

	private:
		wchar_t			fCharacterCode;
		CharacterSet	fCharacterSet;
};

Boolean	operator== (Character lhs, Character rhs);
Boolean	operator!= (Character lhs, Character rhs);
Boolean	operator>= (Character lhs, Character rhs);
Boolean	operator> (Character lhs, Character rhs);
Boolean	operator<= (Character lhs, Character rhs);
Boolean	operator< (Character lhs, Character rhs);



/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
ostream&	operator<< (ostream& out, Character aChar);
istream&	operator>> (istream& in, Character& aChar);





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Character::Character (char c) :
	fCharacterCode (c),
	fCharacterSet (eAscii)
{
}

inline	Character::Character (wchar_t wc, CharacterSet cSet) :
	fCharacterCode (wc),
	fCharacterSet (cSet)
{
}

inline	wchar_t	Character::GetCharacterCode () const
{
	return (fCharacterCode);
}

inline	CharacterSet	Character::GetCharacterSet () const
{
	return (fCharacterSet);
}

inline	char	Character::GetAsciiCode () const
{
	// not sure what do do here????? What if not ascii???
	return (fCharacterCode);
}

inline	Boolean	operator== (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () == rhs.GetCharacterCode ()));
}

inline	Boolean	operator!= (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () != rhs.GetCharacterCode ()));
}

inline	Boolean	operator>= (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () >= rhs.GetCharacterCode ()));
}

inline	Boolean	operator> (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () > rhs.GetCharacterCode ()));
}

inline	Boolean	operator<= (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () <= rhs.GetCharacterCode ()));
}

inline	Boolean	operator< (Character lhs, Character rhs)
{
	// not sure what do do if characer sets differ?
	return (Boolean (lhs.GetCharacterCode () < rhs.GetCharacterCode ()));
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Character__*/

