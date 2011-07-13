/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Small.r,v 1.3 1992/09/01 17:44:49 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Small.r,v $
 *		Revision 1.3  1992/09/01  17:44:49  sterling
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/07/03  04:33:53  lewis
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/20  18:24:12  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/02/18  17:28:50  lewis
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/02/17  23:36:59  lewis
 *		*** empty log message ***
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
	2048	*	1024,						/* preferred mem size	*/
	2048	*	1024						/* minimum mem size		*/
#else
	1024	*	1024,						/* preferred mem size	*/
	1024	*	1024						/* minimum mem size		*/
#endif
};


type qCreator as 'STR ';
resource qCreator (0) {
	"Small"
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
	"Small"
};

resource 'ICN#' (128) {
	{
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

