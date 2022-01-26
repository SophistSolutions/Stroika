/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__Language__
#define	__Language__

/*
 * $Header: /fuji/lewis/RCS/Language.hh,v 1.4 1992/10/10 04:21:25 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Language.hh,v $
 *		Revision 1.4  1992/10/10  04:21:25  lewis
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/05/23  00:11:34  lewis
 *		Dont include UserCommand.hh since never used, and we've moved down
 *		to the foundation and cannot.
 *
 *
 *
 */

#include	"Config-Foundation.hh"


#ifndef qDynamicLanguages
#define	qDynamicLanguages	0
#endif

#if		qDynamicLanguages

extern	Boolean	qEnglish;
extern	Boolean	qFrench;
extern	Boolean	qGerman;
extern	Boolean	qItalian;
extern	Boolean	qSpanish;
extern	Boolean	qJapanese;

#else /* qDynamicLanguages */

#ifndef qEnglish
#define	qEnglish	1
#endif

#ifndef qFrench
#define	qFrench		0
#endif

#ifndef qGerman
#define	qGerman		0
#endif

#ifndef qItalian
#define	qItalian	0
#endif

#ifndef qSpanish
#define	qSpanish	0
#endif

#ifndef qJapanese
#define	qJapanese	0
#endif

#endif /* qDynamicLanguages */

#endif	/* __Language__ */
