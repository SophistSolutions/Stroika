/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ItemScrap.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemScrap.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */





#if		qMacOS
#include	"OSRenamePre.hh"
#include	<Scrap.h>
#include	"OSRenamePost.hh"
#endif	/* qMacOS */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Command.hh"

#include	"ItemScrap.hh"
#include	"GroupItem.hh"



ItemScrap::ItemScrap (GroupItem& group) :
	fText (kEmptyString)
{
#if qUsePictureInScrap
	Tablet*	oldTablet = group.GetTablet ();
	PictureTablet*	pc	=	new PictureTablet (group.GetSize ());
	group.SetTablet (pc);
	group.ProcessLayout ();

	Point	topLeftOrigin = Point (kMaxCoordinate, kMaxCoordinate);
	ForEach (ViewItemPtr, it, group.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		if (current.GetSelected ()) {
			topLeftOrigin = Min (topLeftOrigin, current.GetOrigin ());
		}
	}

	ForEach (ViewItemPtr, it1, group.MakeItemIterator (eSequenceBackward)) {
		AssertNotNil (it1.Current ());
		ViewItem& current = *it1.Current ();
		if (current.GetSelected ()) {
			Point	oldOrigin = current.GetOrigin ();
			current.SetOrigin (oldOrigin - topLeftOrigin, Panel::eNoUpdate);
			current.SetSelected (False, Panel::eNoUpdate);
			current.Render (kBigRegion);
			current.SetSelected (True, Panel::eNoUpdate);
			current.SetOrigin (oldOrigin, Panel::eNoUpdate);
		}
	}

	fPicture = pc->GetResult ();
	group.SetTablet (oldTablet);
	delete pc;
#endif

	StringStream	to;
	group.WriteSelected (to);
	fText = to;
}

CollectionSize	ItemScrap::CountTypes ()
{
#if qUsePictureInScrap
	return (PictureScrap::CountTypes () + 1);
#else
	return (1);
#endif
}

ClipBoard::ClipType	ItemScrap::GetIthType (CollectionSize i)
{
#if qUsePictureInScrap
	if (i > PictureScrap::CountTypes ()) {
		Require (i == (PictureScrap::CountTypes () + 1));
		return (ClipBoard::ClipType (eView));
	}
	return (PictureScrap::GetIthType (i));
#else
	return (ClipBoard::ClipType (eView));
#endif
}

size_t		ItemScrap::GetIthSize (CollectionSize i)
{
#if qUsePictureInScrap
	if (i > PictureScrap::CountTypes ()) {
		Require (i == (PictureScrap::CountTypes () + 1));
		return (fText.GetLength () + 1);
	}
	return (PictureScrap::GetIthSize (i));
#else
	return (fText.GetLength () + 1);
#endif
}	

Boolean	ItemScrap::GetClip (ClipBoard::ClipType type, void*& data, size_t& size)
{
	if (type == eView) {
		size = GetIthSize (CountTypes ());
		data	=	new char [size];
		fText.ToCString ((char*)data, size);
		return (True);
	}
#if qUsePictureInScrap
	return (PictureScrap::GetClip (type, data, size));
#else
	return (False);
#endif
}

void	ItemScrap::ConvertToPublic ()
{
#if qUsePictureInScrap
	PictureScrap::ConvertToPublic ();
#endif

#if		qMacOS
	CollectionSize typeIndex = PictureScrap::CountTypes () +1;
	size_t size = GetIthSize (typeIndex);
	char*	data	=	new char [size];
	fText.ToCString (data, size);
	long	err	=	::PutScrap (size, eView, data);
	Assert (err == noErr);
	delete data;
#endif	/* qMacOS */
}
