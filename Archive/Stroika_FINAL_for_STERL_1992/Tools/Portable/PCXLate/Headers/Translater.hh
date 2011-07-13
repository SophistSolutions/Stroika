/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Translater__
#define	__Translater__

/*
 * $Header: /fuji/lewis/RCS/Translater.hh,v 1.3 1992/09/26 22:24:08 lewis Exp $
 *
 * Description:
 *
 * A Translater is a wrapper on the C++ lexer that takes an input file, and 
 * goes over each token in-turn, and passes it to a virtual method (HandleToken), and
 * this, by default, just writes the token to the output stream. But, you could do
 * some mapping action, or build a data structure sumarizing what you've seen
 * go by for later consumption.
 *
 *	The dstFile in 		Translater::TranslateFile (const PathName& srcFile, const PathName& dstFile);
 * is allowed to be a badpathname, in which case output is ignored/not done.
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Translater.hh,v $
 *		Revision 1.3  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *
 *
 *
 */

#include	"Config-Foundation.hh"

#include	"CPlusLexer.hh"
class istream;
class ostream;

class PathName;

class	Translater {
	public:
		Translater ();

		nonvirtual	void	TranslateFile (const PathName& srcFile, const PathName& dstFile);

	protected:
		virtual		void	HandleToken (const Token& t);
		nonvirtual	void	_HandleToken (const Token& t);

		nonvirtual	istream&	_GetIn () const;
		nonvirtual	ostream&	_GetOut () const;

	private:
		istream*	fInFile;
		ostream*	fOutFile;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Translater__*/

