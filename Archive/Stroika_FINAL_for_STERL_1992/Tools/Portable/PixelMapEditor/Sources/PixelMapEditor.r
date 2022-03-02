/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditor.r,v 1.1 1992/06/20 18:22:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditor.r,v $
 *		Revision 1.1  1992/06/20  18:22:29  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/05/02  23:01:43  lewis
 *		Increase memory sizes in SIZE resource.
 *
 *		Revision 1.1  92/04/20  11:06:04  11:06:04  lewis (Lewis Pringle)
 *		Initial revision
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
	1524	*	1024,						/* preferred mem size	*/
	750		*	1024						/* minimum mem size		*/
#endif	qDebug
};


type qCreator as 'STR ';
resource qCreator (0) {
	"Pixel Map Editor"
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
	"Pixel Map Editor"
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

