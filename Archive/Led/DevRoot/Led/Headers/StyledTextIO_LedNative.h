/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_LedNativeFileFormat_h__
#define	__StyledTextIO_LedNativeFileFormat_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO_LedNative.h,v 2.8 2002/05/06 21:33:35 lewis Exp $
 */


/*
@MODULE:	StyledTextIO
@DESCRIPTION:
		<p>A portable attempt at abstracting away the details of styled text file IO and all
	the different formats for styled text.</p>
		<p>This code defines APIs which should allow for reading any different styled text format.
	And dumping it to any different output (eg. text buffer).</p>
		<p>The only real LED-SPECIFIC parts of this are that I only provide concrete output (aka sinks)
	implementations to Led StandardStyledTextImagers. And that some of the Src/Sink APIs are oriented towards what would be
	helpful for a Led-based editor (in other words, features not supported by Led aren't communicated to/from the src/sinks).</p>
		<p>The <em>big picture</em> for this module is that there are two main basic subdivisions. There are
	@'StyledTextIOReader' subclasses, and @'StyledTextIOWriter' subclasses. The readers are for READING some file format,
	and converting it to a stream of method calls (on a sink to be described later). And writers are for writing those
	formatted data files, based on results of method calls on an abstract source class.</p>
		<p>Though @'StyledTextIOReader' and @'StyledTextIOWriter' share no common base class, they <em>do</em> follow
	a very similar design pattern. They both define abstract 'sources' and 'sinks' for their operation.</p>
		<p>For a @'StyledTextIOReader', it reads its data from a @'StyledTextIOReader::SrcStream' (typically maybe a file),
	and writes it to a @'StyledTextIOReader::SinkStream' (typically a Led-text-buffer/view).</p>
		<p>A @'StyledTextIOWriter', writes data extracted from a @'StyledTextIOWriter::SrcStream'
	(typically view/textstore, much like a @'StyledTextIOReader::SinkStream'),
	and writes it to a @'StyledTextIOWriter::SinkStream' (typically an output file).</p>
		<p>These abstract sources and sinks are defined to just the minimal pure virtual APIs needed to extract/write bare bytes,
	or formatted text in a format Led can understand. Several concrete instantiations of each are provided by Led (some here, and
	some in other modules, as appropriate).</p>
		<p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
	For example, the knowledge of how to read RTF is in @'StyledTextIOReader_RTF' and the knowledge of how to write HTML is in
	@'StyledTextIOWriter_HTML'.</p>
 */

/*
 * Changes:
 *	$Log: StyledTextIO_LedNative.h,v $
 *	Revision 2.8  2002/05/06 21:33:35  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.7  2001/11/27 00:29:45  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.6  2001/10/17 20:42:53  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.5  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.3  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.2  2000/03/13 17:36:34  lewis
 *	Support reading/writing with qWideCharacters turned on
 *	
 *	Revision 2.1  1999/12/18 03:56:46  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *
 *	<========== CODE MOVED HERE FROM StyledTextIO.cpp ==========>
 *
 *
 *
 *
 *
 */

#include	"StyledTextIO.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			StyledTextIOReader_LedNativeFileFormat
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:
*/
class	StyledTextIOReader_LedNativeFileFormat : public StyledTextIOReader {
	public:
		StyledTextIOReader_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Read ();
		override	bool	QuickLookAppearsToBeRightFormat ();

	protected:
		nonvirtual	void	Read_Version4 (const char* cookie);
		nonvirtual	void	Read_Version5 (const char* cookie);		// Led 2.1 file format
		nonvirtual	void	Read_Version6 (const char* cookie);		// Introduced for Led 2.2a2

	// handles default ones Led knows about. You must override to handle your own private types..
	protected:
		virtual		SimpleEmbeddedObjectStyleMarker*	InternalizeEmbedding (Led_PrivateEmbeddingTag tag, size_t howManyBytes);
};

/*
@CLASS:			StyledTextIOWriter_LedNativeFileFormat
@BASES:			@'StyledTextIOWriter'
@DESCRIPTION:
*/
class	StyledTextIOWriter_LedNativeFileFormat : public StyledTextIOWriter {
	public:
		StyledTextIOWriter_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Write ();

	protected:
#if		!qWideCharacters
		nonvirtual	void	Write_Version5 ();						// Led 2.1 file format
#endif
		nonvirtual	void	Write_Version6 ();						// Introduced for Led 2.2a2

	// handles default ones Led knows about. You must override to handle your own private types..
	protected:
		virtual		void	ExternalizeEmbedding (SimpleEmbeddedObjectStyleMarker*	embedding);
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif


#endif	/*__StyledTextIO_LedNativeFileFormat_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
