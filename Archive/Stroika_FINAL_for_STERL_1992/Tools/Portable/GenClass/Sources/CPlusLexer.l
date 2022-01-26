%{
/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CPlusLexer.l,v 1.9 1992/12/07 03:01:25 lewis Exp $
 *
 * Description:
 *		flex based implementation of a CPlusPlus source code lexical analyzer.
 *
 * TODO:
 *
 * Changes:
 *	$Log: CPlusLexer.l,v $
 *		Revision 1.9  1992/12/07  03:01:25  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.8  1992/12/02  20:19:35  lewis
 *		typo.
 *
 *		Revision 1.7  1992/12/02  20:07:44  lewis
 *		Work around qGCC_InserterToSubclassOfOStreamBug, and qGCC_ConversionOperatorCausesHangBug.
 *		Also, use StringRep_BufferedCharArray in a few more places to speed things
 *		up a bit in lexical analysis.
 *
 *		Revision 1.6  1992/11/11  04:04:07  lewis
 *		Use qGCC_NestedClassWithTypedefParseBug to work around new gcc 2.3.1 bug.
 *
 *		Revision 1.5  1992/09/28  17:01:44  lewis
 *		Treat \\ as whitespace.
 *
 *		Revision 1.4  1992/09/28  16:54:19  lewis
 *		Dont include trailing newline as part of // comment.
 *
 *		Revision 1.3  1992/09/28  04:03:12  lewis
 *		Get rid of referneces to kEmptyString - construction order problems.
 *
 *		Revision 1.2  1992/09/25  01:34:00  lewis
 *		Added eTemplate, eLessOrEqual, and eGreaterOrEqual.
 *		Added typedefs to work around qCFront_NestedTypesHalfAssed problem.
 *		Added class for LexicalException.
 *		Other stuff to port to gcc, and update since cloned from old version
 *		of PCXLate.
 *
 *
 *		<COPIED FROM PCXLATE>
 *
 *		Revision 1.2  1992/07/06  20:58:23  lewis
 *		Add #if !qMacOS then define yyinput as input. This
 *		is to give compat between new and old flexes. They have different names for
 *		this function.
 *		ALso, added yy_inputter() as extra level of indirection between YY_INPUT and
 *		(either input or yyinput - depending on which flex we use). This is what
 *		we make a friend in the header.
 *		Also, used String s=String(s::eBuffered,...) with C comment reading code,
 *		and #define reading code - makes HUGE performance difference, and really
 *		critical while I'm still forced to suffer with a MacII ci.
 *
 *		Revision 1.9  92/04/02  11:46:10  11:46:10  lewis (Lewis Pringle)
 *		Add operator ^ to lexicon.
 *
 */
#include	<stdlib.h>

#include	"Debug.hh"
#include	"Math.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"


/*
 * Global variable used to communicate between global functions called by lex
 * (eg YY_INPUT()) to say what instance to really get the text from.
 */
static	CPlusLexer*	sLexer	=	Nil;


/*
 * Override the usual flex routines to do lexical analysis with our methods.
 *		YY_DECL is what lex will call the routine to get the next token, and
 *			the routine which contains the productions enumerated below.
 *		YY_INPUT() is used by flex to get source characters from an input source.
 */
#undef	YY_DECL
#undef	YY_INPUT

#define YY_DECL							Token	CPlusLexer::yylex ()
#define	YY_INPUT(buf,result,max_size)	{ yy_inputter (buf,result,max_size); }

// hack so we can define YY_INPUT macro to call this, and it will
// in turn, be able to call yy_input () - a private function.
void	yy_inputter (void* buf, int& result, size_t max_size)
{
	AssertNotNil (sLexer);
	sLexer->yy_input (buf,result,max_size);
}

#if		qMacOS
// We are using old flex, and it uses name input - for some reason, new versions change name
// to yyinput if using c++ - so we use that name here, and macro hack for backward compat - port
// new flex soon...
#define	yyinput()	input()
#endif

LexicalException	CPlusLexer::sBadInput;

%}

DIGIT		[0-9]

%%

[ \t\r\n]*											{	if (not GetSkipWhiteSpace ()) { return (mkToken (yytext, Token::eWhiteSpace)); } 	}

'\\'	{
		/*
		 * Not too sure we should count this as whitespace. Its probably a bug
		 * for us to encounter one of these. Maybe we should issue a warning?
		 */
		if (not GetSkipWhiteSpace ()) { return (mkToken (yytext, Token::eWhiteSpace)); }
	}

auto												{	return (mkToken (yytext, Token::eAuto)); }
char												{	return (mkToken (yytext, Token::eChar)); }
class												{	return (mkToken (yytext, Token::eClass)); }
const												{	return (mkToken (yytext, Token::eConst)); }
double												{	return (mkToken (yytext, Token::eDouble)); }
enum												{	return (mkToken (yytext, Token::eEnum)); }
extern												{	return (mkToken (yytext, Token::eExtern)); }
float												{	return (mkToken (yytext, Token::eFloat)); }
int													{	return (mkToken (yytext, Token::eInt)); }
long												{	return (mkToken (yytext, Token::eLong)); }
register											{	return (mkToken (yytext, Token::eRegister)); }
short												{	return (mkToken (yytext, Token::eShort)); }
signed												{	return (mkToken (yytext, Token::eSigned)); }
static												{	return (mkToken (yytext, Token::eStatic)); }
struct												{	return (mkToken (yytext, Token::eStruct)); }
template											{	return (mkToken (yytext, Token::eTemplate)); }
typedef												{	return (mkToken (yytext, Token::eTypedef)); }
union												{	return (mkToken (yytext, Token::eUnion)); }
unsigned											{	return (mkToken (yytext, Token::eUnsigned)); }
void												{	return (mkToken (yytext, Token::eVoid)); }
volatile											{	return (mkToken (yytext, Token::eVolatile)); }
public												{	return (mkToken (yytext, Token::ePublic)); }
protected											{	return (mkToken (yytext, Token::eProtected)); }
private												{	return (mkToken (yytext, Token::ePrivate)); }

[a-zA-Z_][a-zA-Z0-9_]*								{	return (mkToken (yytext, Token::eSymbol)); 	}

\/\*	{
	/*
	 * Parse, and save up, and optionally return the standard C comment. This
	 * could be much more efficient if we didn't save the comment in such a
	 * slow way, and if we didn't save it if we aren't going to use it.
	 */
	String	s	=	String_BufferedCharArray ("/*");
	int	c;

	while ( (c = yyinput ()) != EOF) {
		s += c;
		if (c == '*') {
			if ((c = yyinput ()) == '/') {
				s += c;
				break;
			}
			else {
				unput (c);
			}
		}
	}
	if (not GetSkipComments ()) { return (mkToken (s, Token::eComment)); } 	
}

\/\/	{
	String	s	=	String_BufferedCharArray ("//");
	int	c;

	while ( (c = yyinput ()) != EOF) {
		/*
		 * Read 'til the terminating newline/cr, and take that as the
		 * comment. Dont include the terminating nl/cr as part of the comment
		 * so that if we strip comments, you still get the newline
		 * out.
		 */
		if ((c == '\n') or (c == '\r')) {
			unput (c);
			break;
		}
		s += c;
	}
	if (not GetSkipComments ()) { return (mkToken (s, Token::eComment)); } 	
}





[ \t]*#	{
	/*
	 * Parse, and save up a preprocessor directive. Be careful of line continuation characters...
	 */
	String	s	=	String_BufferedCharArray (yytext);
	int		c;
	Boolean	quoting	=	False;
	while ( (c = yyinput ()) != EOF) {
		s += c;
		if (c == '\\') {
			quoting = not quoting;
			continue;
		}
		if (not quoting and ((c == '\n') or (c == '\r'))) {
			break;
		}
		quoting = False;
	}
	return (mkToken (s, Token::ePreProcessorDirective));
}

"{"													{	return (mkToken (yytext, Token::eLBrace)); 	}
"}"													{	return (mkToken (yytext, Token::eRBrace)); 	}
"("													{	return (mkToken (yytext, Token::eLParen)); 	}
")"													{	return (mkToken (yytext, Token::eRParen)); 	}
"["													{	return (mkToken (yytext, Token::eLBracket)); 	}
"]"													{	return (mkToken (yytext, Token::eLBracket)); 	}
"<"													{	return (mkToken (yytext, Token::eLessThan)); 	}
"<="												{	return (mkToken (yytext, Token::eLessOrEqual)); 	}
">"													{	return (mkToken (yytext, Token::eGreaterThan)); 	}
">="												{	return (mkToken (yytext, Token::eGreaterOrEqual)); 	}
"#"													{	return (mkToken (yytext, Token::eSharp)); 	}
"##"												{	return (mkToken (yytext, Token::eSharpSharp)); 	}
"::"												{	return (mkToken (yytext, Token::eScopeResolutionOperator)); 	}
"&"													{	return (mkToken (yytext, Token::eAmpersand)); 	}
"|"													{	return (mkToken (yytext, Token::eVerticalBar)); 	}
"*"													{	return (mkToken (yytext, Token::eAsterisk)); 	}
";"													{	return (mkToken (yytext, Token::eSemiColon)); 	}
","													{	return (mkToken (yytext, Token::eComma)); 	}
"?"													{	return (mkToken (yytext, Token::eQuestionMark)); 	}
":"													{	return (mkToken (yytext, Token::eColon)); 	}
"="													{	return (mkToken (yytext, Token::eEqual)); 	}
"."													{	return (mkToken (yytext, Token::eDot)); 	}
"+"													{	return (mkToken (yytext, Token::ePlus)); 	}
"-"													{	return (mkToken (yytext, Token::eMinus)); 	}
"*"													{	return (mkToken (yytext, Token::eTimes)); 	}
"/"													{	return (mkToken (yytext, Token::eDivide)); 	}
"%"													{	return (mkToken (yytext, Token::eMod)); 	}
"!"													{	return (mkToken (yytext, Token::eBang)); 	}
"~"													{	return (mkToken (yytext, Token::eTwiddle)); 	}
"^"													{	return (mkToken (yytext, Token::eCircumflex)); 	}


{DIGIT}+											{	return (mkToken (yytext, Token::eInteger));	}

{DIGIT}+"."{DIGIT}+([ \t]*[eE][+-]?{DIGIT}+)?		{	return (mkToken (yytext, Token::eReal));	}


\"	{
	/*
	 * Parse, and save up a C String. Be careful of quoted quotes....
	 */
	String	s	=	String_BufferedCharArray ("\"");
	int		c;
	Boolean	quoting	=	False;
	while ( (c = yyinput ()) != EOF) {
		s += c;
		if (c == '\\') {
			quoting = not quoting;
			continue;
		}
		if (not quoting and (c == '"')) {
			break;
		}
		quoting = False;
	}
	return (mkToken (s, Token::eConstantString));
}

\'	{
	/*
	 * Parse, and save up a C String. Be careful of quoted quotes....
	 */
	String	s	=	String_BufferedCharArray ("'");
	int		c;
	Boolean	quoting	=	False;
	while ( (c = yyinput ()) != EOF) {
		s += c;
		if (c == '\\') {
			quoting = not quoting;
			continue;
		}
		if (not quoting and (c == '\'')) {
			break;
		}
		quoting = False;
	}
	if (s.GetLength () > 6) {
		cerr << "Warning: long character constant: length=" << s.GetLength () << ", s=" << s << endl;
	}
	return (mkToken (s, Token::eConstantChar)); 	
}





\/\/[^\r^\n]*[\r\n]									{	if (not GetSkipComments ()) { return (mkToken (yytext, Token::eComment)); } }


.	{
		Assert (yyleng == 1);
		static	String	mess;	// static to avoid dtor problems with exception...
		{
			// extra scope to get dtor called before exception...
			StringStream	sbuf;
			#if		qGCC_InserterToSubclassOfOStreamBug
				((ostream&)sbuf) << "Got bad character in the stream: 0x" << hex << (unsigned int)yytext[0] << "(" << yytext[0] << ")  raising exception";
			#else
				sbuf << "Got bad character in the stream: 0x" << hex << (unsigned int)yytext[0] << "(" << yytext[0] << ")  raising exception";
			#endif
			#if		qGCC_ConversionOperatorCausesHangBug
				mess = String (sbuf);
			#else
				mess = sbuf;
			#endif
		}
		sBadInput.Raise (fLastTokenEnd, mess);
	}

%%


/*
 ********************************************************************************
 ******************************** LexicalException ******************************
 ********************************************************************************
 */
LexicalException::LexicalException ():
	Exception (),
	fOffset (0)//,
//	fMessage ()
{
}

LexicalException::LexicalException (const Exception& exception):
	Exception (exception),
	fOffset (0)//,
//	fMessage ()
{
}

#if		qGCC_NestedClassWithTypedefParseBug
LexicalException::LexicalException (const CPlusLexer::LexicalException& lexicalException):
#else
LexicalException::LexicalException (const LexicalException& lexicalException):
#endif
	Exception (lexicalException),
	fOffset (lexicalException.fOffset)//,
//	fMessage (lexicalException.fMessage)
{
	memcpy (fMessage, lexicalException.fMessage, sizeof (fMessage));
}

void	LexicalException::Raise (UInt32 charOffset, const String& message)
{
	fOffset = charOffset;
	message.ToCStringTrunc (fMessage, sizeof (fMessage));
	Exception::Raise ();
}

void	LexicalException::Raise ()
{
	if (Exception::GetCurrent () != this) {	// if were not the current exception return something
											// indicating unknown error, otherwise preserve our
											// error #
		fOffset = 0;
		fMessage[0] = '\0';
	}
	Exception::Raise ();
}

UInt32	LexicalException::GetOffset () const
{
	return (fOffset);
}

void	LexicalException::SetOffset (UInt32 offset)
{
	fOffset = offset;
}

String	LexicalException::GetMessage () const
{
	return (fMessage);
}

void	LexicalException::SetMessage (const String& message)
{
	message.ToCStringTrunc (fMessage, sizeof (fMessage));
}




/*
 ********************************************************************************
 ************************************** Token ***********************************
 ********************************************************************************
 */

#if		qGCC_SimpleClassStaticInitializerBug
const	CPlusLexer::Token	Token::kEOF(0);
#else
const	CPlusLexer::Token	Token::kEOF		=	CPlusLexer::Token (0);
#endif

#if		qGCC_NestedClassWithTypedefParseBug
Token::Token (const CPlusLexer::Token& from):
#else
Token::Token (const Token& from):
#endif
	fText (from.fText),
	fTokenKind (from.fTokenKind)
{
	// not realy right!!!
	fKeyWord = from.fKeyWord;
}

Token::Token (int tokenFromFLEX):
	fText (""),
	fTokenKind (eEOF)
{
	Assert (tokenFromFLEX == YY_END_TOK);
}

#if		qGCC_NestedClassWithTypedefParseBug
Token::Token (const String& text, CPlusLexer::Token::TokenKind tokenKind):
#else
Token::Token (const String& text, Token::TokenKind tokenKind):
#endif
	fText (text),
	fTokenKind (tokenKind)
{
}

#if		qGCC_NestedClassWithTypedefParseBug
Token::Token (const String& text, CPlusLexer::Token::KeyWords keyWord):
#else
Token::Token (const String& text, Token::KeyWords keyWord):
#endif
	fText (text),
	fTokenKind (eKeyWord)
{
	fKeyWord = keyWord;
}

const CPlusLexer::Token&	Token::operator= (const CPlusLexer::Token& rhs)
{
	fText = rhs.fText;
	fTokenKind = rhs.fTokenKind;

	// not realy right!!!
	fKeyWord = rhs.fKeyWord;
	return (*this);
}



Boolean	operator== (const CPlusLexer::Token& lhs, const CPlusLexer::Token& rhs)
{
	if (lhs.fTokenKind != rhs.fTokenKind) {
		return (False);
	}

// really we need more descriminition than this...
	return (True);
}

Boolean	operator!= (const CPlusLexer::Token& lhs, const CPlusLexer::Token& rhs)
{
	return (not (lhs == rhs));
}







/*
 ********************************************************************************
 ********************************* CPlusLexer ***********************************
 ********************************************************************************
 */
CPlusLexer::CPlusLexer (istream& characterSource):
	fCharacterSource (characterSource),
	fSkipWhiteSpace (False),
	fSkipComments (False),
	fOldToken (Token::kEOF),
	fOldTokenValid (False)
{
	yy_init = 1;
}

CPlusLexer::Token	CPlusLexer::GetNextToken ()
{
	if (fOldTokenValid) {
		fOldTokenValid = False;
		return (fOldToken);
	}
	else {
		Assert (sLexer == Nil);
		sLexer = this;
		try {
			Token	t	=	yylex ();
			Assert (sLexer == this);
			sLexer = Nil;
			return (t);
		}
		catch () {
			sLexer = Nil;
			_this_catch_->Raise ();
			AssertNotReached (); return (Token::kEOF);
		}
	}
}

void	CPlusLexer::PutBack (const CPlusLexer::Token& old)
{
	if (fOldTokenValid) {
		AssertNotReached ();		// Need a real stack to implement that...
	}
	else {
		fOldTokenValid = True;
		fOldToken = old;
	}
}

Boolean	CPlusLexer::GetSkipWhiteSpace () const
{
	return (fSkipWhiteSpace);
}

void	CPlusLexer::SetSkipWhiteSpace (Boolean skipWhiteSpace)
{
	fSkipWhiteSpace = skipWhiteSpace;
}

Boolean	CPlusLexer::GetSkipComments () const
{
	return (fSkipComments);
}

void	CPlusLexer::SetSkipComments (Boolean skipComments)
{
	fSkipComments = skipComments;
}

void	CPlusLexer::yy_input (void* buf, int& result, size_t max_size)
{
	if (fCharacterSource) {
		fCharacterSource.read ((char*)buf, max_size);
		result = fCharacterSource.gcount ();
	}
	else {
		result = 0;
	}

	// Since we seem to get in trouble with flex and Nul characters in the input, raise an exception here if that happens..
	for (int i = 0; i < result; i++) {
		if (((char*)buf)[i] == '\0') {
			sBadInput.Raise (fLastTokenEnd, "Got null character in the stream");
		}
	}
}

Token	CPlusLexer::mkToken (const String& text, TokenKind tokenKind)
{
	Token	t (text,tokenKind);
	return (t);
//	return (Token (text, tokenKind));
}

Token	CPlusLexer::mkToken (const String& text, KeyWords keyWord)
{
	Token	t (text, keyWord);
	return (t);
//	return (Token (text, keyWord));
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

