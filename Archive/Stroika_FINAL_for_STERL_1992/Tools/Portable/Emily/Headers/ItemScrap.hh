/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ItemScrap__
#define	__ItemScrap__

/*
 * $Header: /fuji/lewis/RCS/ItemScrap.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemScrap.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.12  92/05/13  18:38:26  18:38:26  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.8  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 */

#include	"ClipBoard.hh"
#include	"String.hh"

#define	qUsePictureInScrap	qMacToolkit

class	GroupItem;
#if 	qUsePictureInScrap
class	ItemScrap : public PictureScrap {
#else
class	ItemScrap : public PrivateScrap {
#endif

	public:
		enum {
			eView		=	'VIEW',
		};

		ItemScrap (GroupItem& group);

		override	CollectionSize	CountTypes ();
		override	ClipBoard::ClipType		GetIthType (CollectionSize i);
		override	size_t			GetIthSize (CollectionSize i);

		override	Boolean	GetClip (ClipBoard::ClipType type, void*& data, size_t& size);
		override	void	ConvertToPublic ();

	private:
		String			fText;
};

#endif	/* __ItemScrap__ */

