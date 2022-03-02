/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__CommandNumbers__
#define	__CommandNumbers__

/*
 * $Header: /fuji/lewis/RCS/CommandNumbers.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: CommandNumbers.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"Language.hh"

#include	"Command.hh"
#include	"UserCommand.hh"

enum {
	eArrow = CommandHandler::eFirstApplicationCommand,
	eThumb,

	eFirstBuildItem,
	
	eBuildSlider = eFirstBuildItem,
	eBuildButton,
	eBuildTextEdit,
	eBuildTextView,
	eBuildMenu,
	eBuildPickList,
	eBuildGroup,
	eBuildLine,
	eBuildUser1,
	eBuildUser2,
	eBuildUser3,
	eBuildUser4,
	
	eLastBuildItem = eBuildUser4,
	
	eSetPreferences,

	eSizeToFit,
	
	eSetClassInfo,
	eSetItemInfo,	
	
	eGroupItems,
	eUngroupItems,
	
	eAlignLeft,
	eAlignCenterH,
	eAlignRight,
	eAlignTop,
	eAlignCenterV,
	eAlignBottom,
	
	eSpaceVertically,
	eUserSpecifyVerticalSpacing,
	eUniformSpacesVertically,
	eSpaceHorizontally,
	eUserSpecifyHorizontalSpacing,
	eUniformSpacesHorizontally,
	
	eConstrainLeft,
	eConstrainTop,
	eConstraintWidth,
	eConstrainHeight,

	eInheritFont,

	eEnabled,
	
	eCustomizeOnly,
	
	eFirstGUI, 
	eAnyGUI = CommandHandler::eNoCommand,
	eMacUI = eFirstGUI,
	eMotifUI,
	eWindowsGUI,
	eLastGUI = eWindowsGUI,
		
	eDeleteCustomization,
	eAutoCustomize,
	eReorderFocusItems,
};

#endif	/* __CommandNumbers__ */
