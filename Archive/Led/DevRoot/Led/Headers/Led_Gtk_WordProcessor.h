/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_Gtk_WordProcessor_h__
#define	__Led_Gtk_WordProcessor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_Gtk_WordProcessor.h,v 2.13 2003/09/22 22:06:08 lewis Exp $
 */


/*
@MODULE:	Led_Gtk_WordProcessor
@DESCRIPTION:
 */



/*
 * Changes:
 *	$Log: Led_Gtk_WordProcessor.h,v $
 *	Revision 2.13  2003/09/22 22:06:08  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 2.12  2003/09/22 20:20:54  lewis
 *	for SPR#1552: add StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML. Lose 
 *	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes and replace with 
 *	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (and base 
 *	class code to call InternalizeBestFlavor with new suggestedClipType). Moved kRTFClipType 
 *	from FlavorPackage module to StandardStyledTextInteractor module - since not needed in
 *	FlavorPackage code.
 *	
 *	Revision 2.11  2003/04/04 14:41:40  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.10  2003/04/04 00:55:21  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly
 *	MacOS/Linux now)
 *	
 *	Revision 2.9  2003/04/03 16:41:22  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead of
 *	template params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.8  2003/01/29 19:15:06  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.7  2002/05/06 21:33:28  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.6  2001/11/27 00:29:40  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/10/17 21:46:40  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.4  2001/09/26 15:41:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2001/08/28 18:43:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2001/05/25 21:09:21  lewis
 *	SPR#0899- Support RTF clipboard type, and much more (related). Significant reorg of
 *	clipboard (Gtk-specific) support. Now uses more subeventloops (now that I discovered that I can).
 *	
 *	Revision 2.1  2001/04/27 15:22:39  lewis
 *	SPR#0885- Led_Gtk command support
 *	
 *	
 *
 *
 *
 *	<========== CODE based on Led_MFC_WordProcessor.h - moved some code here ==========>
 *
 */

#include	"Led_Gtk.h"
#include	"WordProcessor.h"



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)
#endif



#if		qLedUsesNamespaces
namespace	Led {
#endif





/*
@CLASS:			WordProcessorCommonCommandHelper_Gtk<BASECLASS,CMD_INFO>
@BASES:			@'WordProcessorCommonCommandHelper<BASECLASS,CMD_INFO>' where BASECLASS=@'TextInteractorCommonCommandHelper_Gtk<BASECLASS,CMD_INFO>', where BASECLASS is the baseclass arg to this template.
@DESCRIPTION:	<p>This is a simple wrapper to apply special Gtk specific changes to
			use of the Led @'WordProcessor' class with @'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>'.</p>
*/
template	<typename	BASECLASS>	class	WordProcessorCommonCommandHelper_Gtk :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;
	public:
		WordProcessorCommonCommandHelper_Gtk ();
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	WordProcessorCommonCommandHelper_Gtk<BASECLASS>
template	<typename	BASECLASS>
	WordProcessorCommonCommandHelper_Gtk<BASECLASS>::WordProcessorCommonCommandHelper_Gtk ()
		{
			if (kLedPrivateClipFormat == 0) {
				kLedPrivateClipFormat = gdk_atom_intern ("Led Private Clipboard Format", false);
				Led_Assert (kLedPrivateClipFormat != None);
			}
			if (kRTFClipFormat == 0) {
				kRTFClipFormat = gdk_atom_intern ("text/rtf", false);
				Led_Assert (kRTFClipFormat != None);
			}
			if (kHTMLClipFormat == 0) {
				kHTMLClipFormat = gdk_atom_intern ("text/html", false);
				Led_Assert (kHTMLClipFormat != None);
			}
			static	GdkAtom	clipboard_atom = gdk_atom_intern ("CLIPBOARD", false);
			Led_Assert (clipboard_atom != None);
			gtk_selection_add_target (Get_GtkWidget (), clipboard_atom,(GdkAtom)kRTFClipFormat, 0);
		}



#if		qLedUsesNamespaces
}
#endif

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif


#endif	/*__Led_Gtk_WordProcessor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
