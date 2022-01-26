/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__NodeViewCommands__
#define	__NodeViewCommands__
/*
 * $Header: /fuji/lewis/RCS/NodeViewCommands.hh,v 1.2 1992/09/01 17:30:27 sterling Exp $
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
 * Changes:
 *	$Log: NodeViewCommands.hh,v $
 *		Revision 1.2  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 *
 *
 */

enum {
	eConfigureNodes			= CommandHandler::eFirstApplicationCommand,
	eAddNode,
	eRemoveNode,
	eShiftLeft,
	eShiftRight,
};

#endif /* __NodeViewCommands__ */