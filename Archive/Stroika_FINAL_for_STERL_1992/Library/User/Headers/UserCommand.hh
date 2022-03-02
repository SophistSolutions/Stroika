/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__UserCommand__
#define	__UserCommand__

/*
 * $Header: /fuji/lewis/RCS/UserCommand.hh,v 1.1 1992/06/20 17:33:49 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: UserCommand.hh,v $
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.8  1992/03/26  16:53:31  sterling
 *		menu and language command numbers
 *
 *
 *
 */

#include	"Command.hh"

enum {
	eSetFont = CommandHandler::eFirstUserCommand,
	eFont,
	eFontSize,
	eUserSpecifyFontSize,
	eFontStyle,
	
	eJustifyLeft,
	eJustifyCenter,
	eJustifyRight,
	eJustifyFull,
	
	ePlainFontCommand,
	eBoldFontCommand,
	eItalicFontCommand,
	eUnderlineFontCommand,
	eOutlineFontCommand,
	eShadowFontCommand,


	eEnglish,
	eFrench,
	eGerman,
	eItalian,
	eSpanish,
	eJapanese,

	eFirstLanguage = eEnglish, 
	eAnyLanguage = CommandHandler::eNoCommand,
	eLastLanguage = eJapanese,
};


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/* __UserCommand__ */

