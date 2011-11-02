/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__StyledTextIO_PlainText_h__
#define	__StyledTextIO_PlainText_h__	1

/*
@MODULE:	StyledTextIO_PlainText
@DESCRIPTION:
		<p>Support reading/writing plain text to/from word-processing Src/Sink streams.</p>
 */

#include	"Led_StyledTextIO.h"


#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			StyledTextIOReader_PlainText
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:
*/
class	StyledTextIOReader_PlainText : public StyledTextIOReader {
	public:
		StyledTextIOReader_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		virtual		void	Read () override;
		virtual		bool	QuickLookAppearsToBeRightFormat () override;
};




/*
@CLASS:			StyledTextIOWriter_PlainText
@BASES:			@'StyledTextIOWriter'
@DESCRIPTION:
*/
class	StyledTextIOWriter_PlainText : public StyledTextIOWriter {
	public:
		StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		virtual		void	Write () override;
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif


#endif	/*__StyledTextIO_PlainText_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
