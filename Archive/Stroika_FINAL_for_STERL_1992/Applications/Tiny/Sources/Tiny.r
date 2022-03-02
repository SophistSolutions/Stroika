/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Tiny.r,v 1.2 1992/09/01 18:00:14 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tiny.r,v $
 *		Revision 1.2  1992/09/01  18:00:14  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/20  18:40:22  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/05/01  23:15:01  lewis
 *		Initial revision
 *
 *		Revision 1.5  92/02/18  17:15:14  17:15:14  lewis (Lewis Pringle)
 *		*** empty log message ***
 *		
 *		Revision 1.4  1992/02/17  23:44:15  lewis
 *		Use new version support.
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
	1024	*	1024						/* minimum mem size		*/
#else	qDebug
	1024	*	1024,						/* preferred mem size	*/
	512		*	1024						/* minimum mem size		*/
#endif	qDebug
};


type qCreator as 'STR ';
resource qCreator (0) {
	"Tiny"
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

