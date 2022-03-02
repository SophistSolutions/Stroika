/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DoTranslate.cc,v 1.12 1992/12/07 03:06:18 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DoTranslate.cc,v $
 *		Revision 1.12  1992/12/07  03:06:18  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.11  1992/12/03  19:34:32  lewis
 *		Work around qGCC_InserterToSubclassOfOStreamBug.
 *
 *		Revision 1.10  1992/11/19  22:34:47  lewis
 *		KeepComments = True.
 *
 *		Revision 1.9  1992/10/22  18:16:27  lewis
 *		Got rid of extra unused variable.
 *
 *		Revision 1.8  1992/10/20  21:30:03  lewis
 *		Fix hacks for autoincluding <T> stuff after class name in template
 *		declaration, and for CTOR/DTORs.
 *
 *		Revision 1.7  1992/10/16  22:59:06  lewis
 *		Two main sets of changes - the first tested, and the second not.
 *		First, introduced new helper proc MappedName(), and used it in processing
 *		include names, and got rid of extra '(' and ')' in include names if we
 *		had them in the original argument type.
 *		Second, and untested, is a set of fixes to allow now <T> after the
 *		template <class T> class Fred; Fred should no longer need the <T> at the end.
 *		Also, once we've seen this, later occurences like Fred<T>::Fred () will automatically
 *		get the <T> stuck in properly.
 *
 *		Revision 1.6  1992/10/10  04:34:20  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/10  03:14:18  lewis
 *		Get rid of Declares.
 *
 *		Revision 1.4  1992/10/08  02:10:18  sterling
 *		Lots of cleanups - use (conditionally) GenClass stuff to bootstrap.
 *		Got rid of some obsolete parts (like RawText - toiugh still supported
 *		in file format). Got basically all the Foundation, this guy and testsuites
 *		working on mac.
 *
 *		Revision 1.3  1992/10/02  04:17:51  lewis
 *		Added Parameterize.
 *
 *		<Copied From GenClass.cc>
 *
 */
#include	<ctype.h>
#include	<iostream.h>

#include	"Date.hh"
#include	"Debug.hh"
#include	"Format.hh"
#include	"Memory.hh"
#include	"Sequence.hh"
#include	"StreamUtils.hh"

#include	"File.hh"

#include	"CPlusLexer.hh"
#include	"FileNameUtils.hh"

#include	"DoTranslate.hh"


// This should be done autmatically!!!
extern	ostream& operator << (ostream& out, const Sequence(String)& s);// Implementation in GenClass.cc



typedef	LexicalException	ParseException;
static	ParseException		sParseException	=	CPlusLexer::sBadInput;

static	void	ProcessInclude (String text, ostream& to, 
								const Sequence(String)& arguments, const Sequence(String)& argNames);

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	// define this cuz already pulled in by InfoDB.cc
	#define		qIteratorOfStringCCDefined
	#include	MappingOfString_SequenceOfString_cc
#endif


static	String	MappedName (const Sequence(String)& arguments,
						    const Sequence(String)& argNames,
							String name);


void	BuildBoilerplate (const String& fromFileName, istream& from, ostream& to,
						  const Sequence(String)& arguments,
						  const Sequence(String)& argNames, Boolean doingMapping)
{
	Require (arguments.GetLength () == argNames.GetLength ());
	CPlusLexer lexer (from); 

	try {
		#if		qRealTemplatesAvailable
			Mapping<String, Sequence<String> >	templateClasses;
		#else
			Mapping(String,Sequence(String))	templateClasses;
		#endif
		unsigned	bracketCount	=	0;
		Token		t =	lexer.GetNextToken ();
		for (;t != Token::kEOF; ) {
			switch (t.fTokenKind) {
				case	Token::eComment: 
					{
						/*
						 * Optionally retain comments.
						 */
						if (doingMapping) {
							Boolean keepComments = True;
							if (keepComments) {
								to << t.fText;
							}
						}
					}
					break;

				case	Token::eLessThan: 
					{
						// Really this is a crock, but since we dont have
						// a list of all templates, this will do for now...
						if (doingMapping) {
							Token xxx =	lexer.GetNextToken ();
							lexer.PutBack (xxx);
							if (xxx.fTokenKind == Token::eWhiteSpace) {
								to << t.fText;
							}
							else {
								bracketCount++;
								to << "(";
							}
						}
					}
					break;

				case	Token::eGreaterThan: 
					{
						// See Less than comment
						if (doingMapping) {
							if (bracketCount == 0) {
								to << ">";
							}
							else {
								bracketCount--;
								to << ")";
							}
						}
					}
					break;

				case	Token::eSymbol:
					{
						/*
						 * Perform parameter substitution.
						 */
						if (doingMapping) {
							to << MappedName (arguments, argNames, t.fText);
							/*
							 * If the symbol is a template name, and is not qualified with an
							 * <> pair, we must insert them.
							 */
							Sequence(String)	tmpArgList;
							if (templateClasses.Lookup (t.fText, &tmpArgList)) {
								/*
								 * Now if there are not <> here already, we must insert them.
								 */
								Token	bracketToken = lexer.GetNextToken ();
								lexer.PutBack (bracketToken);
								if (bracketToken.fTokenKind != Token::eLessThan) {
									to << "(";
									ForEachT (SequenceIterator, String, it, tmpArgList) {
										to << MappedName (arguments, argNames, it.Current ());
										if (it.CurrentIndex () != tmpArgList.GetLength ()) {
											to << ",";
										}
									}
									to << ")";
								}
							}
						}
					}
					break;

				case	Token::eKeyWord:
					{
						if (t.fKeyWord == Token::eTemplate) {
							#if		qRealTemplatesAvailable
								Sequence<String>	args;
							#else
								Sequence(String)	args;
							#endif
							/*
							 * strip "template <class T...>"
							 *
							 * Do this by reading at least one '<' and then keep reading til
							 * we get enuf balancing right braces. Ignore anything in-between,
							 * though perhaps we should have output comments/whitespace?
							 */
							if (not doingMapping) {
								sParseException.Raise (from.tellg (), "template keyword outside of #if qTemplatesAvailable");
							}
							unsigned braceLevel = 0;
							for (Token nt =	lexer.GetNextToken (); ; ) {
								if (nt.fTokenKind == Token::eLessThan) {
									braceLevel++;
									break;
								}
								nt = lexer.GetNextToken ();
							}
							Assert (braceLevel == 1);
							String	thisTypeArg = "";
							for (nt =	lexer.GetNextToken (); braceLevel != 0;) {
								if (nt.fTokenKind == Token::eLessThan) {
									braceLevel++;
								}
								else if (nt.fTokenKind == Token::eGreaterThan) {
									braceLevel--;
									if (braceLevel == 0 and (thisTypeArg != "")) {
										args += thisTypeArg;
										thisTypeArg = "";
									}
								}
								else if (nt.fTokenKind == Token::eComma) {
									args += thisTypeArg;
									thisTypeArg = "";
								}
								else if (nt.fTokenKind != Token::eWhiteSpace) {
									// overwrite previous token - it was probably a type name or the class keyword
									thisTypeArg = nt.fText;
								}
								nt = lexer.GetNextToken ();
							}
							/*
							 * Now, peek at the next two tokens. If they are class FRED, then
							 * we probably have a class template being declared (though not
							 * necessarily, we screw up if not true - not too hard to guarnatee
							 * in ones we do in Stroika). If not, then we are probably doing a member
							 * function. At any rate, scoop up the name, and save away the arg list
							 * for later usage if it is a class declaration.
							 */
							Token	classToken = lexer.GetNextToken ();
							if (classToken.fTokenKind == Token::eKeyWord and
								classToken.fKeyWord == Token::eClass) {
								to << " " << classToken.fText << " ";
								Token	nameToken = lexer.GetNextToken ();
								while (nameToken.fTokenKind == Token::eWhiteSpace) {
									to << nameToken.fText;
									nameToken = lexer.GetNextToken ();
								}
								if (nameToken.fTokenKind != Token::eSymbol) {
									sParseException.Raise (from.tellg (), "expected symbol after template <class ...> class");
								}
								to << nameToken.fText;
								templateClasses.Enter (nameToken.fText, args);
								/*
								 * Now if there are not <> here already, we must insert them.
								 */
								Token	bracketToken = lexer.GetNextToken ();
								lexer.PutBack (bracketToken);
								if (bracketToken.fTokenKind != Token::eLessThan) {
									to << "(";
									ForEachT (SequenceIterator, String, it, args) {
										to << MappedName (arguments, argNames, it.Current ());
										if (it.CurrentIndex () != args.GetLength ()) {
											to << ",";
										}
									}
									to << ")";
								}
							}
							else {
								lexer.PutBack (classToken);
							}
						}
						else {
							if (doingMapping) {
								to << t.fText;
							}
						}
					}
					break;

				case Token::ePreProcessorDirective:
					{
						/*
						 * Look specifically for #if\tqRealTemplatesAvailable, and only
						 * in this context do we do name substitution, and here we also
						 * rename the #define...
						 */
						if (t.fText == "#if\t\tqRealTemplatesAvailable\n") {
							if (doingMapping) {
								sParseException.Raise (from.tellg (), "nested #if qTemplatesAvailable");
							}
							doingMapping = True;
						}
						else if (t.fText == "#endif\t/*qRealTemplatesAvailable*/\n") {
							if (not doingMapping) {
								sParseException.Raise (from.tellg (), "#endif qTemplatesAvailable without matching #if");
							}
							doingMapping = False;
						}
						else if (t.fText.Contains ("#include")) {
							if (doingMapping) {
								ProcessInclude (t.fText, to, arguments, argNames);
							}
						}
						else {
						// skip #defines - we'll get them from including our real header (not great solution,
						// but will do for now)...
							if (doingMapping) {
								if (not t.fText.Contains ("#define")) {
									to << t.fText;
								}
							}
						}
					}
					break;

				default:
					/*
					 * Everything else we let pass thru.
					 */
					if (doingMapping) {
						to << t.fText;
					}
			}
			t = lexer.GetNextToken ();
		}
	}
	catch1 (sParseException) {
		cout << "Parse Exception: File: " << fromFileName << ", CharOffset: " << sParseException.GetOffset () << ". " << sParseException.GetMessage () << endl;
		_this_catch_->Raise ();
	}
	catch1 (CPlusLexer::sBadInput) {
		cout << "Lexical Exception: File: " << fromFileName << ", CharOffset: " << CPlusLexer::sBadInput.GetOffset () << ". " << CPlusLexer::sBadInput.GetMessage () << endl;
		_this_catch_->Raise ();
	}
	catch () {
		cout << "Unknown Exception: File: " << fromFileName << endl;
		_this_catch_->Raise ();
	}
}

static	void	ProcessInclude (String text, ostream& to, 
								const Sequence(String)& arguments, const Sequence(String)& argNames)
{
	const String	kInclude = "#include";
	if (text.Contains ('"')) {
		// ordinary include, and hence not by problem
		to << text;
	}
	else {
		for (size_t index = 1; index <= text.GetLength (); index++) {
			// scan for '<'
			while (text[index].GetAsciiCode () != '<') { 
				to << text[index].GetAsciiCode();
				if (index++ >= text.GetLength ()) {
					break;
				}
			}
			index++;
			if (index < text.GetLength ()) {
				// grab arguments (process till comma or greater than
				String tok;
				while (isalnum (text[index].GetAsciiCode ())) {
					tok += text[index];
					if (index++ >= text.GetLength ()) {
						break;
					}
				}
				to << TypeNameToFileName (MappedName (arguments, argNames, tok));
			}
		}
		to << endl;
	}
}

String	Parameterize (const String& s,  const Sequence(String)& arguments, const Sequence(String)& argNames)
{
	StringStream to;
	StringStream from;
#if		qGCC_InserterToSubclassOfOStreamBug
	((ostream&)from) << s;
#else
	from << s;
#endif
	from.seekg (0);
#if		qGCC_InserterToSubclassOfOStreamBug
	BuildBoilerplate ("BOGUS", (istream&)from, (ostream&)to, arguments, argNames, True);
#else
	BuildBoilerplate ("BOGUS", from, to, arguments, argNames, True);
#endif
	return (String(to));
}

static	String	MappedName (const Sequence(String)& arguments,
						    const Sequence(String)& argNames,
							String name)
{
	ForEachT (SequenceIterator, String, it, argNames) {
		if (name == it.Current ()) {
			return (arguments[it.CurrentIndex ()]);
		}
	}
	return (name);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

