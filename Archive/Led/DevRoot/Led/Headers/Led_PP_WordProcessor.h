/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_PP_WordProcessor_h__
#define	__Led_PP_WordProcessor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_PP_WordProcessor.h,v 2.10 2003/11/06 16:34:26 lewis Exp $
 */


/*
@MODULE:	Led_PP_WordProcessor
@DESCRIPTION:
		<p>Led_PP_WordProcessor.</p>
 */


/*
 * Changes:
 *	$Log: Led_PP_WordProcessor.h,v $
 *	Revision 2.10  2003/11/06 16:34:26  lewis
 *	fix typo
 *	
 *	Revision 2.9  2003/11/06 16:14:10  lewis
 *	SPR#1560: added LStream* CTOR overload
 *	
 *	Revision 2.8  2003/04/04 14:41:42  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.7  2003/04/03 16:41:23  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead
 *	of template params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.6  2002/05/06 21:33:30  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.5  2001/11/27 00:29:42  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.3  2001/09/26 15:41:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2001/08/28 18:43:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  2000/10/03 21:48:39  lewis
 *	moved WordProcessorCommonCommandHelper_PP/WP specific stuff from Led_PP module to
 *	this new module (and greatly reduced what was needed as part of SPR#0839
 *	
 *
 *
 *
 *
 */
#include	"Led_PP.h"
#include	"WordProcessor.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif






/*
@CLASS:			WordProcessorCommonCommandHelper_PP<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>This is a simple wrapper to apply special PowerPlant specific changes to
			use of the Led @'WordProcessor' class with @'Led_PPView'.</p>
				<p>Note that this class does nothing (and so really doesn't need to be used), but is a
			placeholder in case there are WP-specific
			and PP-specific functionality (as there is for MFC and WP).</p>
*/
template	<class	BASECLASS>	class	WordProcessorCommonCommandHelper_PP :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;
	public:
		WordProcessorCommonCommandHelper_PP ();
		WordProcessorCommonCommandHelper_PP (LStream* stream);
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	template	<class	BASECLASS>
		inline	WordProcessorCommonCommandHelper_PP<BASECLASS>::WordProcessorCommonCommandHelper_PP ():
			inherited ()
			{
			}
	template	<class	BASECLASS>
		inline	WordProcessorCommonCommandHelper_PP<BASECLASS>::WordProcessorCommonCommandHelper_PP (LStream* stream):
			inherited (stream)
			{
			}



#if		qLedUsesNamespaces
}
#endif


#endif	/*__Led_PP_WordProcessor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

