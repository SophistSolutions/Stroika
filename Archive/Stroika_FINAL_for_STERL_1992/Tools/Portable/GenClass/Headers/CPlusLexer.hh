/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CPlusLexer__
#define	__CPlusLexer__

/*
 * $Header: /fuji/lewis/RCS/CPlusLexer.hh,v 1.2 1992/09/25 01:32:32 lewis Exp $
 *
 * Description:
 *
 * A C/CPP lexical analyzer.
 *
 *		Not quite sure yet where CPP fits in here, but we probably want to assume that this can
 *	either be used as a front end for CPP, or perhaps we build some of its functions
 *	in at this level.
 *
 *		Anyway, the main goal of this code is to build a Token stream from C++ text.
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: CPlusLexer.hh,v $
 *		Revision 1.2  1992/09/25  01:32:32  lewis
 *		Added eTemplate, eLessOrEqual, and eGreaterOrEqual.
 *		Added typedefs to work around qCFront_NestedTypesHalfAssed problem.
 *		Added class for LexicalException.
 *
# Revision 1.1  1992/09/24  00:49:30  lewis
# Initial revision
#
 *		Revision 1.2  1992/07/06  20:54:13  lewis
 *		Got rid of hack support for old flex - different versions of input, etc
 *		as friends - add yy_inputter () as freind so we can access yy_input
 *		from YY_INPUT() macro. This gets around problem of different versions
 *		of flex (with different names) calling yy_input.
 *
 *		Revision 1.1  1992/06/20  18:16:50  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/05/01  23:20:23  lewis
 *		Initial revision
 *
 *
 *
 *
 */

#include	"StreamUtils.hh"


class	istream;
class	CPlusLexer {
	public:
		class	LexicalException : public Exception {
			public:
				LexicalException ();
				LexicalException (const Exception& exception);
				LexicalException (const LexicalException& lexicalException);

				nonvirtual	void	Raise (UInt32 charOffset, const String& message);
				override	void	Raise ();

				nonvirtual	UInt32	GetOffset () const;
				nonvirtual	void	SetOffset (UInt32 offset);

				nonvirtual	String	GetMessage () const;
				nonvirtual	void	SetMessage (const String& message);

			private:
				UInt32	fOffset;
				char	fMessage[1024];	// use fixed size buf since must be CTORd before main!
		};
		static	LexicalException	sBadInput;
		struct	Token {
			enum KeyWords {
				eAuto,
				eChar,
				eClass,
				eConst,
				eDouble,
				eEnum,
				eExtern,
				eFloat,
				eInt,
				eLong,
				eRegister,
				eShort,
				eSigned,
				eStatic,
				eStruct,
				eTemplate,
				eTypedef,
				eUnion,
				eUnsigned,
				eVoid,
				eVolatile,
				ePublic,
				eProtected,
				ePrivate
			};
			enum TokenKind {
				eEOF,
				eKeyWord,
				eLParen,
				eRParen,
				eLBrace,
				eRBrace,
				eLBracket,
				eRBracket,
				eLessThan,
				eLessOrEqual,
				eGreaterThan,
				eGreaterOrEqual,
				eSharp,
				eSharpSharp,
				eScopeResolutionOperator,
				eAmpersand,
				eVerticalBar,
				eAsterisk,
				eSemiColon,
				eComma,
				eQuestionMark,
				eColon,
				eEqual,
				eDot,
				ePlus,
				eMinus,
				eTimes,
				eDivide,
				eMod,
				eBang,
				eTwiddle,
				eCircumflex,
				eInteger,
				eReal,
				eConstantString,
				eConstantChar,			// could be long char
				eSymbol,
				ePreProcessorDirective,
				eComment,
				eWhiteSpace,
			};

			static	const	Token	kEOF;

			Token (const Token& from);
			Token (int tokenFromFLEX);									// Should only be called from FLEX generated code and always assumed to be EOF
			Token (const String& text, TokenKind tokenKind);
			Token (const String& text, KeyWords keyWord);
			const Token& operator= (const Token& rhs);

			String		fText;
			TokenKind	fTokenKind;
			union {
				KeyWords	fKeyWord;
			};
		};
		CPlusLexer (istream& characterSource);


		/*
		 * Main interface to retrieving, and replacing Tokens.
		 */
		virtual	Token	GetNextToken ();
		virtual	void	PutBack (const Token& old);



		/*
		 * If fSkipWhiteSpace is True, then whitespace will not be returned. Otherwise, they will.
		 */
		nonvirtual	Boolean	GetSkipWhiteSpace () const;
		nonvirtual	void	SetSkipWhiteSpace (Boolean skipWhiteSpace);


		/*
		 * If fSkipComments is True, then comments will not be returned. Otherwise, they will.
		 */
		nonvirtual	Boolean	GetSkipComments () const;
		nonvirtual	void	SetSkipComments (Boolean skipComments);

	private:
		istream&		fCharacterSource;
		Boolean			fSkipWhiteSpace;
		Boolean			fSkipComments;
		Token			fOldToken;				// Later replace this with a Stack implementation...
		Boolean			fOldTokenValid;
		streampos		fLastTokenEnd;

		/*
		 * Stuff to hook into flex.
		 */
		nonvirtual	Token	yylex ();						// Implemented by Flex, no us...
		nonvirtual	void	yy_input (void* buf, int& result, size_t max_size);

		nonvirtual	Token	mkToken (const String& text, Token::TokenKind tokenKind);
		nonvirtual	Token	mkToken (const String& text, Token::KeyWords keyWord);


	/*
	 * Hack so we can define YY_INPUT macro to call this, and it will
	 * in turn, be able to call yy_input () - a private function.
	 */
	friend	void	yy_inputter (void* buf, int& result, size_t max_size);
};

#if		!qCFront_NestedTypesHalfAssed
	// We need this typedef for several reasons. One, it would be nightmarishly
	// complex to use two different syntaxes in the code that uses this stuff - one
	// for real nested types, and one for CFront 2.1 -half-assed. Further, gcc 2.2.1
	// has a bug which this works around. When all the various compilers come up to
	// speed here, we can get rid of this, and use the nested types stuff directly
	// in the source too...
	typedef	CPlusLexer::Token				Token;
	typedef	Token::TokenKind				TokenKind;
	typedef	Token::KeyWords					KeyWords;
	typedef	CPlusLexer::LexicalException	LexicalException;
	
#endif


extern	Boolean	operator== (const CPlusLexer::Token& lhs, const CPlusLexer::Token& rhs);
extern	Boolean	operator!= (const CPlusLexer::Token& lhs, const CPlusLexer::Token& rhs);




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


#endif	/*__CPlusLexer__*/
