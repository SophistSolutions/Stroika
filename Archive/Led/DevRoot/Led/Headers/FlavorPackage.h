/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__FlavorPackage_h__
#define	__FlavorPackage_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/FlavorPackage.h,v 2.20 2003/09/22 22:06:07 lewis Exp $
 */

/*
@MODULE:	FlavorPackage
@DESCRIPTION:
		<p>This module defines the packaged up contents of serialized data. This data is usually used to exchange
	in drag and drop, and clipboard operations. But its sometimes used/saved for other purposes, like UNDO and
	hidable text.
 */


/*
 * Changes:
 *	$Log: FlavorPackage.h,v $
 *	Revision 2.20  2003/09/22 22:06:07  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 2.19  2003/09/22 20:20:53  lewis
 *	for SPR#1552: add StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML. Lose StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes and replace with StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (and base class code to call InternalizeBestFlavor with new suggestedClipType). Moved kRTFClipType from FlavorPackage module to StandardStyledTextInteractor module - since not needed in FlavorPackage code.
 *	
 *	Revision 2.18  2003/09/22 15:46:51  lewis
 *	Added InternalizeFlavor_FILEGuessFormatsFromName().  Changed args to InternalizeFlavor_FILEDataRawBytes and InternalizeFlavor_FILEData (). Much better implemenation - now using FileReader class. Still a bit more to go on this - but now we use same code in FILEIO part of LedLineIt as we have here for internalize for opening files. must do likewise for LedIt (so D&D of file operation works better). Done as part of bug#1552
 *	
 *	Revision 2.17  2002/05/06 21:33:24  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.16  2001/11/27 00:29:38  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.15  2001/10/17 20:42:48  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.14  2001/09/05 16:22:21  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.13  2001/08/29 23:00:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2001/05/25 21:09:20  lewis
 *	SPR#0899- Support RTF clipboard type, and much more (related). Significant reorg of clipboard (Gtk-specific)
 *	support. Now uses more subeventloops (now that I discovered that I can).
 *	
 *	Revision 2.10  2001/05/03 20:56:00  lewis
 *	SPR#0894- added clipboard support for X-Windows - by HERE creating a sPrivateClipData field. Then - in Led_Gtk<>
 *	template - that is munged and read and used to communicate with the XSelection mechanism
 *	
 *	Revision 2.9  2000/06/23 21:42:24  lewis
 *	Broke InternalizeFlavor_FILEData into two - that routine and InternalizeFlavor_FILEDataRawBytes -
 *	so easier to subclass just the one part
 *	
 *	Revision 2.8  2000/04/25 19:00:44  lewis
 *	added kLedPrivateClipType/kRTFClipType. And made GetFlavorSize/ReadFlavorData() CONST
 *	
 *	Revision 2.7  2000/04/25 16:10:06  lewis
 *	fix ReadWriteFlavorPackage code - I now see why I had the private COPY CTOR etc. It was buggy in that case -
 *	but trivial to fix. Just use vector<> for data rather than void* data.
 *	
 *	Revision 2.6  2000/04/24 21:21:40  lewis
 *	lose private Copy/op= CTORs for ReadWriteMemBufferPackage. They can be handy, and seems like
 *	detault implemantion should work fine
 *	
 *	Revision 2.5  2000/04/24 16:47:02  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor - but simply
 *	owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP class etc to make this
 *	work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.4  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.3  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.2  2000/04/14 19:12:20  lewis
 *	SPR#0739- move code from TextInteractor to this separate module- and cleanup a bit
 *	
 *
 *
 *
 */

#include	<map>
#include	<vector>

#include	"CodePage.h"
#include	"Command.h"
#include	"TextImager.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif



class	ReaderFlavorPackage;
class	WriterFlavorPackage;






/*
@CLASS:			FlavorPackageExternalizer
@BASES:			virtual @'MarkerOwner'
@DESCRIPTION:	<p>Helper class for implementing externalizing. Can be subclassed to add new formats.
	Call it with a @'WriterFlavorPackage', and a range to copy from, and the externalizing will be done.</p>
		<p>See also @'FlavorPackageInternalizer'.</p>
*/
class	FlavorPackageExternalizer : public virtual MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		FlavorPackageExternalizer (TextStore& ts);

	public:
		override	TextStore*	PeekAtTextStore () const;

	public:
		virtual	void	ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		virtual	void	ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		virtual	void	ExternalizeFlavor_TEXT (WriterFlavorPackage& flavorPackage, size_t from, size_t to);

	private:
		TextStore&	fTextStore;
};





/*
@CLASS:			FlavorPackageInternalizer
@BASES:			virtual @'MarkerOwner'
@DESCRIPTION:	<p>Helper class for implementing internalizing. Can be subclassed to add new formats.
	Call it with a @'ReaderFlavorPackage', and a range to insert it into, and the internalizing will be done.</p>
		<p>See also @'FlavorPackageExternalizer'.</p>
*/
class	FlavorPackageInternalizer : public virtual MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		FlavorPackageInternalizer (TextStore& ts);

	public:
		override	TextStore*	PeekAtTextStore () const;

	public:
		virtual		bool	InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
													size_t from, size_t to
												);
		nonvirtual	bool	InternalizeFlavor_TEXT (ReaderFlavorPackage& flavorPackage,
														size_t from, size_t to
													);
		virtual		bool	InternalizeFlavor_FILE (ReaderFlavorPackage& flavorPackage,
														size_t from, size_t to
													);
		virtual		bool	InternalizeFlavor_FILEData (
									#if		qMacOS
										const FSSpec* fileName,
									#elif	qWindows || qXWindows
										const Led_SDK_Char* fileName,
									#endif
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										size_t from, size_t to
								);
		virtual		void	InternalizeFlavor_FILEGuessFormatsFromName (
									#if		qMacOS
										const FSSpec* fileName,
									#elif	qWindows || qXWindows
										const Led_SDK_Char* fileName,
									#endif
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage
								);
		virtual		void	InternalizeFlavor_FILEGuessFormatsFromStartOfData (
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										const Byte* fileStart, const Byte* fileEnd
								);
		virtual		bool	InternalizeFlavor_FILEDataRawBytes (
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										size_t from, size_t to,
										const void* rawBytes, size_t nRawBytes
								);

	private:
		TextStore&	fTextStore;
};









/*
@CLASS:			ReaderFlavorPackage
@DESCRIPTION:	<p>Abstraction wrapping both Drag&Drop packages, and clipboard access. Used by @'FlavorPackageInternalizer'.</p>
*/
class	ReaderFlavorPackage {
	public:
		nonvirtual	bool	GetFlavorAvailable_TEXT () const;
		virtual		bool	GetFlavorAvailable (Led_ClipFormat clipFormat) const						=	0;

	public:
		/*
		@METHOD:		ReaderFlavorPackage::GetFlavorSize
		@DESCRIPTION:	<p>Return an upper bound on the size of the given flavor element. Try to get the right size, but
			the OS frequently makes this impossible. Just garuantee that when you do a ReaderFlavorPackage::ReadFlavorData
			you get the right size, and that is smaller or equal to what this returns.</p>
		*/
		virtual		size_t	GetFlavorSize (Led_ClipFormat clipFormat) const								=	0;

	public:
		/*
		@METHOD:		ReaderFlavorPackage::ReadFlavorData
		@DESCRIPTION:	<p>Return the data of a given clip format, copied into the passed in buffer. The caller must allocate/free
			the buffer. An upper bound on the size needed for the buffer can be retrieved with @'ReaderFlavorPackage::GetFlavorSize'.</p>
		*/
		virtual		size_t	ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const	=	0;
};




/*
@CLASS:			ReaderClipboardFlavorPackage
@BASES:			@'ReaderFlavorPackage'
@DESCRIPTION:	<p>NB: On windows - it is REQUIRED the ClipboardFlavorPackage objects only be
	created in the context of Open/Close clipboard operations (for example done in
	OnPasteCommand_Before/OnPasteCommand_After - so typically no problem).</p>
				<p>NB: For X-Windows, the clip data is just stored in the global variable ReaderClipboardFlavorPackage::sPrivateClipData.</p>
*/
class	ReaderClipboardFlavorPackage : public ReaderFlavorPackage {
	public:
		override	bool	GetFlavorAvailable (Led_ClipFormat clipFormat) const;
		override	size_t	GetFlavorSize (Led_ClipFormat clipFormat) const;
		override	size_t	ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const;
#if		qXWindows
	public:
		static	map<Led_ClipFormat,vector<char> >	sPrivateClipData;
#endif
};




/*
@CLASS:			WriterFlavorPackage
@DESCRIPTION:	<p>Abstraction wrapping both Drag&Drop packages, and clipboard access. Used by @'FlavorPackageExternalizer'.</p>
*/
class	WriterFlavorPackage {
	public:
		virtual	void	AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)	=	0;
};




/*
@CLASS:			WriterClipboardFlavorPackage
@BASES:			@'WriterFlavorPackage'
@DESCRIPTION:	<p>NB: On windows - it is REQUIRED the ClipboardFlavorPackage objects only be
	created in the context of Open/Close clipboard operations (for example done in
	OnCopyCommand_Before/OnCopyCommand_After - so typically no problem).</p>
				<p>See also @'ReaderClipboardFlavorPackage'.</p>
*/
class	WriterClipboardFlavorPackage : public WriterFlavorPackage {
	public:
		override	void	AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf);
};




/*
@CLASS:			ReadWriteMemBufferPackage
@BASES:			@'ReaderFlavorPackage', @'WriterFlavorPackage'
@DESCRIPTION:	<p>Dual purpose, store-and-reuse package. Useful for undo.</p>
*/
class	ReadWriteMemBufferPackage : public ReaderFlavorPackage, public WriterFlavorPackage {
	public:
		ReadWriteMemBufferPackage ();
		~ReadWriteMemBufferPackage ();

	//	ReaderFlavorPackage
	public:
		override	bool	GetFlavorAvailable (Led_ClipFormat clipFormat) const;
		override	size_t	GetFlavorSize (Led_ClipFormat clipFormat) const;
		override	size_t	ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const;

	//	WriterFlavorPackage
	public:
		override	void	AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf);

	private:
		struct	PackageRecord {
			Led_ClipFormat	fFormat;
			vector<char>	fData;
		};
		vector<PackageRecord>	fPackages;
};












/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	FlavorPackageExternalizer
	inline	FlavorPackageExternalizer::FlavorPackageExternalizer (TextStore& ts):
		inherited (),
		fTextStore (ts)
		{
		}


//	class	FlavorPackageInternalizer
	inline	FlavorPackageInternalizer::FlavorPackageInternalizer (TextStore& ts):
		inherited (),
		fTextStore (ts)
		{
		}




//	class	ReaderFlavorPackage
	inline	bool	ReaderFlavorPackage::GetFlavorAvailable_TEXT () const
		{
			if (GetFlavorAvailable (kTEXTClipFormat)) {
				return true;
			}
			#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
			if (GetFlavorAvailable (CF_TEXT)) {
				return true;
			}
			#endif
			return false;
		}



#if		qLedUsesNamespaces
}
#endif


#endif	/*__FlavorPackage_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

