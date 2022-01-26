/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ClipBoard__
#define	__ClipBoard__

/*
 * $Header: /fuji/lewis/RCS/ClipBoard.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ClipBoard.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  03:45:58  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/07/01  01:37:47  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *		Condition Declare macros, etc for qRealTemplatesAvailable.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/01/28  20:51:09  lewis
 *		Change the way of constructing ClipBoards, and allow static method to Set the clipboard. No longer
 *		handle by class Application - all is handled here. Including Import/Export clipboard messages now
 *		handled by our EventHandler.
 *
 *
 *
 */

#include	"Bag.hh"

#include	"Picture.hh"

#include	"Config-Framework.hh"



class	ClipCamera;
#if		!qRealTemplatesAvailable
	typedef	ClipCamera*	ClipCamPtr;
	Declare (Iterator, ClipCamPtr);
	Declare (Collection, ClipCamPtr);
	Declare (AbBag, ClipCamPtr);
	Declare (Array, ClipCamPtr);
	Declare (Bag_Array, ClipCamPtr);
	Declare (Bag, ClipCamPtr);
#endif	/*qRealTemplatesAvailable*/


// need to decide how to deal with accessing available scrap types
// need to decide how to deal with private scraps, and setting multiple scap types.
// lots of ideas but not sure whats best.

// Can only be one ClipBoard in existence at a time...

class	PrivateScrap;
class	ClipBoard {
	public:
		enum	{
			eText		=	'TEXT',
			ePicture	=	'PICT',
		};
		typedef	int	ClipType;

		ClipBoard ();
		virtual ~ClipBoard ();

		/*
		 * Querying what is on the clipboard.  Probably should use iterators?
		 */
		nonvirtual	CollectionSize	CountTypes ();
		nonvirtual	ClipType		GetIthType (CollectionSize i);
		nonvirtual	size_t			GetIthSize (CollectionSize i);
		nonvirtual	size_t			GetSize ();			// overall size... (ie size of my overalls?)

		/*
		 * Retrieving what is on the clipboard.   Return true if able to retrieve
		 * clip item of given type.  Fill in data with newly new'ed pointer.  Callers
		 * responsibilty to free.  Things returned in low level format, regardless
		 * of type.  Callers responsibility to convert.
		 */
		nonvirtual	Boolean	GetClip (ClipType type, void*& data, size_t& size);

		nonvirtual	Boolean	Contains (ClipType type);

		/*
		 * Placing new things on the clipboard.
		 *
		 * First, there is the standard way:
		 * 		Before adding to the clipboard, first clear it out, and then call AddClip
		 * as often as desired, with new clip types.  Priority in display and usage
		 * will be given to the ones passed along first.
		 *
		 * Then, there is the private scrap mechanism.  You place an object on the clip
		 *		with methods that can be called to flush itself onto the real scrap as needed, or
		 * not at all, as the case may be.  This allows for more efficient use of the scap when
		 * converting from internal to external formats is expensive and unnessary.
		 */
		nonvirtual	void	ClearClip ();
		nonvirtual	void	AddClip (ClipType type, const void* data, size_t size);

		nonvirtual	void	SetPrivateScrap (PrivateScrap* ps);

		/*
		 * For generating clipboard view.
		 */
		nonvirtual	Picture		RenderAsPicture ();
		nonvirtual	Boolean		RenderTypeAsPicture (ClipType type, const void* data, size_t size, Picture& picture);

		/*
		 * Managing clipboard types known about. 
		 */
		nonvirtual	void	AddClipCamera (ClipCamera* newCamera);
		nonvirtual	void	RemoveClipCamera (ClipCamera* oldCamera);

		// called by eventmanager on switchins/outs to flush to real scrap..
		nonvirtual	void	CheckOSScrap ();
		nonvirtual	void	UpdateOSScrap ();


		nonvirtual	UInt32	GetScrapChangeCount () const;


		/*
		 * Usually you just call get, and the first call will automatically build the clipboard object,
		 * but if you want to override behavior of the clipboard, you can call set (Nil) to delete the clipboard,
		 * and then build your own clipboard (eg say ClipBoard::Set (new MyClipBoard ()); does the whole job).
		 * ClipBoard::Set() deletes the existing one, regardless of who created it, and replaces it with the given
		 * one which better be a new clipboard object.
		 *
		 * NB: Funny name SetThe to avoid conflict with MACRO Set() in foundation!!!
		 */
		static	ClipBoard&	Get ();
		static	void		SetThe (ClipBoard* newClipBoard);

	private:
		Bag(ClipCamPtr)						fClipCameras;
		PrivateScrap*						fPrivateScrap;
		UInt32								fLastChangeCountWeKnewAbout;
		class	ClipBoardEventHandler*		fEventHandler;
		static	ClipBoard*					sThe;
};



class	PrivateScrap {
	protected:
		PrivateScrap ();

	public:
		virtual	~PrivateScrap ();

		/*
		 * Querying what is on the clipboard.  Probably should use iterators?
		 */
		virtual	CollectionSize		CountTypes () = Nil;
		virtual	ClipBoard::ClipType	GetIthType (CollectionSize i) = Nil;
		virtual	size_t				GetIthSize (CollectionSize i) = Nil;

		virtual	size_t				GetSize ();			// overall size... (ie size of my overalls?)

		nonvirtual	Boolean			Contains (ClipBoard::ClipType type);

		// caller must delete data, if returned true...
		virtual	Boolean				GetClip (ClipBoard::ClipType type, void*& data, size_t& size)	=	Nil;

		virtual	void				ConvertToPublic ()			=	Nil;
};



class	PictureScrap : public PrivateScrap {
	public:
		PictureScrap ();

		override	CollectionSize		CountTypes ();
		override	ClipBoard::ClipType	GetIthType (CollectionSize i);
		override	size_t				GetIthSize (CollectionSize i);
		override	Boolean				GetClip (ClipBoard::ClipType type, void*& data, size_t& size);
		override	void				ConvertToPublic ();

	protected: 
		Picture		fPicture;
};




/*
 * The point of ClipCamera classes is to encapsulate the conversion of a given clipboard
 * item into a picture for rendering in the "Show Clipboard" window.
 */
class	ClipCamera {
	public:
		ClipCamera (ClipBoard::ClipType typeSupported);

		nonvirtual	ClipBoard::ClipType	GetTypeSupported () const;

		virtual		Picture				ClickClick (const void* clipItem, size_t size)		=	Nil;

	private:
		ClipType fTypeSupported;
};

class	TEXTClipCamera : public ClipCamera {
	public:
		TEXTClipCamera ();

		override	Picture		ClickClick (const void* clipItem, size_t size);
};

class	PICTClipCamera : public ClipCamera {
	public:
		PICTClipCamera ();

		override	Picture		ClickClick (const void* clipItem, size_t size);
};




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


#endif	/*__ClipBoard__*/

