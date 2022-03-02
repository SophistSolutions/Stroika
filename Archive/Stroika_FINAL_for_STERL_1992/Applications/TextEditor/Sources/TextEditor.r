/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditor.r,v 1.1 1992/06/20 18:37:39 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditor.r,v $
 *		Revision 1.1  1992/06/20  18:37:39  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/03/26  17:58:56  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/02/18  00:22:34  lewis
 *		Use new version number support.
 *
 *
 *
 */

#include	"ApplicationVersion"


resource 'vers' (1) {
	kMajorVersion,
	kMinorVersion,
	kStage,
	kStageVersion,
	verUS,
	kShortVersionString,
	kLongVersionString,
};


resource 'SIZE' (-1) {
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,
	multiFinderAware,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	is32BitCompatible,
	notHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
#if		qDebug
	1524	*	1024,						/* preferred mem size	*/
	1024	*	1024						/* minimum mem size		*/
#else
	750		*	1024,						/* preferred mem size	*/
	512		*	1024						/* minimum mem size		*/
#endif
};


type qCreator as 'STR ';
resource qCreator (0) {
	"TextEditor"
};

resource 'BNDL' (128) {
	qCreator,
	0,
	{
		'ICN#',
		{
			0, 128
		},
		'FREF',
		{
			0, 128
		}
	}
};

resource 'FREF' (128) {
	'APPL',
	0,
	"Application"
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 FE0E 0000 1010 0000 10E0 0C00 1080 1000 1080 20F8 1040 234C 107C 4040"
		$"1020 4040 1020 2040 0820 2040 0820 1040 003E 1040 0003 1040 0000 7040",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};




/* For gnuemacs:
 * Local Variables: ***
 * mode:C ***
 * tab-width:4 ***
 * End: ***
 */
