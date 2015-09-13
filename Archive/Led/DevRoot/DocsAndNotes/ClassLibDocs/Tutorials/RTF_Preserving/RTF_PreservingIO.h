/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__WWRTFIO_h__
#define	__WWRTFIO_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/DocsAndNotes/ClassLibDocs/Tutorials/RTF_Preserving/RTF_PreservingIO.h,v 2.6 2001/11/27 00:37:16 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: RTF_PreservingIO.h,v $
 *	Revision 2.6  2001/11/27 00:37:16  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/10/15 16:01:05  lewis
 *	get compiling and roughly working with Led 3.0. Maybe delete for future releases of Led - since I think this tutorial is not very good and even less useful
 *	
 *	Revision 2.4  2001/08/30 01:10:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1999/07/19 19:28:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1999/03/08 23:11:02  lewis
 *	react to various led changes over the last year or so
 *	
 *	Revision 2.1  1997/06/18 02:26:12  lewis
 *	Now base Led code reads/writes color table, so we keep it as object, not string.
 *	
 *	Revision 2.0  1996/10/04  16:30:02  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 *
 */

#include	"StyledTextIO_RTF.h"


#if		qLedUsesNamespaces
	namespace	Led {
#endif


/*
 *	Unknown RTF Preserving Support.
 *
 *	Support for a special reader/writer which attempt to preserve all RTF tags, even if Led doesn't
 *	understand them. This isn't necessarily desirable behavior, but some (WordWork) have asked for it.
 *	Watch out for what happens when people edit while using these Reader/Writer classes!
 *
 *	<WARNING!>
 *
 *		This part of the RTF support is provisional, and may not be supported in the future. I'm not
 *	at all happy with how it works. It was done especially for a particular client (WordWork) who
 *	needed functionality like this. Apparently it was sufficient for thier needs. But I'm not satisfied
 *	with how general purpose this code is. It is provided more as an example of how you might use this
 *	RTF support, and as a piece of code I would suggest you build upon.
 *
 *	</WARNING!>
 */
class	RTFInfo_Preserving : public RTFInfo, public MarkerOwner {
	public:
		typedef	RTFIO::ControlWord		ControlWord;
		typedef	RTFIO::FontTable		FontTable;
		typedef	RTFIO::ColorTable		ColorTable;
	public:
		RTFInfo_Preserving (TextStore* textStore = NULL);
		~RTFInfo_Preserving ();

	public:
		nonvirtual	void	SetTextStore (TextStore* textStore);
	private:
		TextStore*	fTextStore;

	public:
		override	TextStore*	PeekAtTextStore () const;


	// During the read process, we create alot of stored RTF info, and associate that info with
	// this object. When we need to RE-READ the document - like on a revert, or when we destroy
	// the object, we must first clean-out all existing memory used to keep track of stuff from
	// the previous read. Thats what DeleteAllInfo () does.
	public:
		nonvirtual	void	DeleteAllInfo ();

	public:
		CodePage		fDocumentCharacterSet;
		int				fDefaultFontNumber;		// -1 ==> none specified
		int				fDefLangNumber;			// -1 ==> none specified
		FontTable		fFontTable;
		ColorTable		fColorTable;
		string			fSavedStyleSheetGroup;
		string			fSavedInfoGroup;
		string			fSavedAuthorGroup;

	public:
		// A marker encompassing some RTF (typically an RTF Control Word & arg) - which we read
		// and chose to preserve. These markers are always zero-length, and so they don't get deleted
		// as the user edits text. This is probably bad, but I can see no rational way around this.
		// Use this with great caution, and probably only in constrained editing situations
		//
		// Keep track of these in a linked list, so we preserve a sense of ordering even if they
		// all get inserted at the same point in the buffer.
		class	HiddenRTFControlMarker : public Marker {
			public:
				HiddenRTFControlMarker (const ControlWord& controlWord);
				HiddenRTFControlMarker (const string& text);
			
			public:
				string					fText;
				HiddenRTFControlMarker* fNext;

			private:
				nonvirtual	void	WriteControlWord (const ControlWord& controlWord);
		};
		HiddenRTFControlMarker* fFirstHiddenControlMarker;
		HiddenRTFControlMarker* fLastHiddenControlMarker;		// keep track of last cuz we want them in FORWARD
																// order (APPEND) not reversed
};




class	StyledTextIOReader_RTF_Preserving : public StyledTextIOReader_RTF {
	public:
		typedef	RTFIO::ControlWord		ControlWord;
	public:
		StyledTextIOReader_RTF_Preserving (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo_Preserving* rtfInfo);

	private:
		typedef	StyledTextIOReader_RTF	inherrited;

	public:
		override	void	Read ();

	protected:
		override	bool	HandleControlWord_author (ReaderContext& readerContext, const ControlWord& controlWord);
		override	bool	HandleControlWord_info (ReaderContext& readerContext, const ControlWord& controlWord);
		override	bool	HandleControlWord_stylesheet (ReaderContext& readerContext, const ControlWord& controlWord);

	protected:
		override	void	AboutToStartBody (ReaderContext& readerContext);

	protected:
		override	void	ReadGroup (ReaderContext& readerContext);
		override	bool	HandleControlWord_row (ReaderContext& readerContext, const ControlWord& controlWord);
		override	bool	HandleControlWord_UnknownControlWord (ReaderContext& readerContext, const ControlWord& controlWord);

	protected:
		nonvirtual	void	AddHiddenRTFMarker (ReaderContext& readerContext, RTFInfo_Preserving::HiddenRTFControlMarker* m);

	private:
		RTFInfo_Preserving*	fRTFInfo;
};




class	StyledTextIOWriter_RTF_Preserving : public StyledTextIOWriter_RTF {
	public:
		StyledTextIOWriter_RTF_Preserving (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo_Preserving* rtfInfo);

	private:
		typedef	StyledTextIOWriter_RTF	inherrited;

	public:
		override	void	WriteHeader (WriterContext& writerContext);
		override	void	WriteBody (WriterContext& writerContext);

	protected:
		override	void	AssureColorTableBuilt ();
		override	void	AssureFontTableBuilt ();

	private:
		RTFInfo_Preserving*	fRTFInfo;
};











/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



#if		qLedUsesNamespaces
}
#endif


#endif	/*__WWRTFIO_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
