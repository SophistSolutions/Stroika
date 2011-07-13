/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Exception.cc,v 1.4 1992/12/05 17:32:53 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Exception.cc,v $
 *		Revision 1.4  1992/12/05  17:32:53  lewis
 *		Support Throw() with qWinOS
 *
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 */



#include	<stdlib.h>

#include	"Debug.hh"
#include	"Memory.hh"

#include	"Exception.hh"








/*
 ********************************************************************************
 ************************************* Exception ********************************
 ********************************************************************************
 */

Exception*	Exception::_sCurrent		=	Nil;


Exception::Exception ()
{
	fIsAs [0] = Nil;
	fIsAs [1] = Nil;
}

Exception::Exception (const Exception& exception1)
{
	fIsAs [0] = &exception1;
	fIsAs [1] = Nil;
}

Exception::Exception (const Exception& exception1, const Exception& exception2)
{
	fIsAs [0] = &exception1;
	fIsAs [1] = &exception2;
}

Exception&	Exception::operator= (const Exception& exception)
{
	fIsAs [0] = &exception;
	fIsAs [1] = Nil;
	return (*this);
}

Boolean	Exception::Match (const Exception& exception) const
{
	if (this == &exception) {
		return (True);
	}
	if ((fIsAs [0] != Nil) and (fIsAs [0]->Match (exception))) {
		return (True);
	}
	if ((fIsAs [1] != Nil) and (fIsAs [1]->Match (exception))) {
		return (True);
	}
	return (False);
}

void	Exception::Raise ()
{
	if (ExceptionHandler::sTopExceptionHandler == Nil) {
		DefaultHandler ();
	}
	else {
		ExceptionHandler::sTopExceptionHandler->HandleExceptionRaise (*this);
	}
	AssertNotReached ();
}

void	Exception::DefaultHandler ()
{
	/*
	 * Perhaps should consider using the toolbox to put an an alert at this point?
	 * At least beep or something?
	 */
#if		qDebug
	DebugMessage ("Exception raised when no handler present.  Aborting...");
#endif
	abort ();
}








/*
 ********************************************************************************
 ********************************* ExceptionHandler *****************************
 ********************************************************************************
 */

ExceptionHandler*	ExceptionHandler::sTopExceptionHandler	=	Nil;

ExceptionHandler::ExceptionHandler ():
	fNextExceptionHandler (sTopExceptionHandler)
{
	sTopExceptionHandler = this;
}

ExceptionHandler::~ExceptionHandler ()
{
	/*
	 * If we have caght an exception, then we may no longer be the topmost handler,
	 * and yet we may, or may not be destructed, depending on whether or not a further
	 * exception is raised (to pass on exception).
	 *
	 * At any rate, if we are the topmost handler, we want to pass on that task to the next
	 * in the chain.
	 */
	if (sTopExceptionHandler == this) {
		sTopExceptionHandler = fNextExceptionHandler;
	}
}

void	ExceptionHandler::HandleExceptionRaise (Exception& exception)
{
	sTopExceptionHandler = fNextExceptionHandler;		// once handled were not at top
	Exception::_sCurrent = &exception;
#if		qWinOS
	Throw (fJumpBuffer, 1);
#else
	longjmp (fJumpBuffer, 1);
#endif
	AssertNotReached ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

