/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Exception__
#define	__Exception__

/*
 * $Header: /fuji/lewis/RCS/Exception.hh,v 1.6 1992/12/08 21:15:39 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *		->	For now, assume we can use setjmp/longjmp for WindowsNT. Til proven otherwise!
 *
 *
 * Changes:
 *	$Log: Exception.hh,v $
 *		Revision 1.6  1992/12/08  21:15:39  lewis
 *		Looked at exception stuff for qWinNT, and decided to try
 *		setjmp/longjmp.
 *
 *		Revision 1.5  1992/12/07  11:43:14  lewis
 *		Call in try Macro sb Catch() - not catch.
 *
 *		Revision 1.4  1992/12/05  17:31:30  lewis
 *		Support qWinOS Catch/Throw instead of setjmp/longjmp - not sure
 *		why they don't use the standard, but I'm told they don't work right.
 *		Also, because of conflicts, and because lowercase names closer
 *		to eventual standard, I switched to try (from Try) and catch* (from
 *		Catch*).
 *
 *		Revision 1.3  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *
 *
 */

#include	"Config-Foundation.hh"

#if		qWinOS
	/*
	 *	Not sure If I should include bits of Window.h here, or just include it
	 *	all, or use undefs and do partial include. For now, take the easy road,
	 *	and think it out later.
	 */
	typedef	int	_CATCHBUF_[9];
	extern	"C"	int		_far _pascal Catch(int _far*);
	extern	"C"	void	_far _pascal Throw(const int _far*, int);
#else
	#include	<setjmp.h>
#endif


class	Exception {
	public:
		Exception ();
		Exception (const Exception& exception1);
		Exception (const Exception& exception1, const Exception& exception2);
		/* ETC... */

		nonvirtual	Exception&	operator= (const Exception& exception);

		nonvirtual	Boolean	Match (const Exception& exception) const;

		virtual	void	Raise ();
		virtual	void	DefaultHandler ();

	public:
		/*
		 *		This is used in the try macro, and in exceptions when
		 *	re-raised to see if they need to reset internal data or not
		 *	(default arg processing).
		 */
		static	Exception*	GetCurrent ();

	public:				// conceptually private, but made public so we could
						// access thru try macro
		static	Exception*	_sCurrent;

	private:
		Exception const* 	fIsAs [2];

	friend	class	ExceptionHandler;
};


/*
 *		Expception handling is done by keeping a linked list starting at
 *	sTopExceptionHandler, and following next pointers.  They are automatically
 *	maintained by ctors/dtors and raises.
 *
 * We make big assumption that <> about ctor order, etc... FILL IN!
 */
class	ExceptionHandler {
	public:
		ExceptionHandler ();
		~ExceptionHandler ();

	private:
		static	ExceptionHandler*	sTopExceptionHandler;

		ExceptionHandler*	fNextExceptionHandler;

		nonvirtual	void	HandleExceptionRaise (Exception& exception);

	public:	//	CONCEPTUALLY PRIVATE, But must be public so try () macro can fill stuff in.
#if		qWinOS
		_CATCHBUF_	fJumpBuffer;
#else
		jmp_buf		fJumpBuffer;
#endif

	friend	class	Exception;
};


/*
 * Macros to simulate the exception stuff listed in Nov 89 Excepction
 * Handling paper by Stroustrup/Koenig
 */
#if		qWinOS
	#define	try	\
					ExceptionHandler	_this_exception_handler_;\
					Exception*&			_this_catch_	=	Exception::_sCurrent;\
					if (Catch (_this_exception_handler_.fJumpBuffer) == 0)
#else
	#define	try	\
					ExceptionHandler	_this_exception_handler_;\
					Exception*&			_this_catch_	=	Exception::_sCurrent;\
					if (setjmp (_this_exception_handler_.fJumpBuffer) == 0)
#endif

#define	catch1(_EXCEPTION_)								else if (_this_catch_->Match (_EXCEPTION_))

#define	catch2(_EXCEPTION1_,_EXCEPTION2_)				else if (_this_catch_->Match (_EXCEPTION1_) or _this_catch_->Match (_EXCEPTION2_))

#define	catch3(_EXCEPTION1_,_EXCEPTION2_,_EXCEPTION3_)	else if (_this_catch_->Match (_EXCEPTION1_) or _this_catch_->Match (_EXCEPTION2_) or _this_catch_->Match (_EXCEPTION3_))

#define	catch()								else




// LGP Jan 29 - macro added for ted at marcam to make things look a little more like
// new builtin-exception support in some cases... (trouble is case where raise takes args!)
//
#define	throw(e)	((e).Raise ())





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Exception*	Exception::GetCurrent ()	{	return (_sCurrent);	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Exception__*/

