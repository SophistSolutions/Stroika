/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/LedPartVers.h,v 2.24 1996/12/13 18:09:57 lewis Exp $
 *
 * Description:
 *		LedPart OpenDoc part editor class definition.
 *
 * TODO:
 *
 * Notes:
 *
 *	There are three sets of version numbers you need to maintain.
 *	The CFM version numbers in the 'cfrg' resource, the SOM
 *	class version numbers in the IDL file, and the Finder file
 *	version number.
 *	
 *	Fortunately, all three version schemes require a "major" and "minor"
 *	number to generate the release version. This makes things
 *	easier, but not simple. SOM classes only use those two pieces,
 *	while CFM and the Finder define "development stage" and
 *	"prerelease" version; the "prerelease" version can basically be
 *	ignored, but be forewarned that CFM will use them, if provided.
 *	
 *	In all cases, a version number can be defined as 3 digits seperated
 *	by 2 periods. The "major" version number has a range of 0-99, while
 *	the two minor version numbers have a range of 0-9. An example of
 *	a version number is '2.3.1'. All digits in the version number are
 *	represented in Binary-Coded-Decimal (BCD) format even though they
 *	appear in hexadecimal. The above example version number is 0x0231
 *	in hexadecimal (note that the 3 and the 1 are in the same byte).
 *	
 *	Below is a set of defines to generate the version numbers for
 *	all three instances. This file should be included in your .r and
 *	.idl file to keep your part versioning in sync.
 *	
 *	Each time you need to update the version of the part generated, you
 *	should find the appropriate constants and change them.
 *	
 *	** WARNING **: Each linker that you run will require the same numbers you
 *	specify here. Be sure to update your project preferences, for
 *	integrated environments, and your makefile for MPW to ensure the version
 *	numbers are the same everywhere.
 *	
 *	For complete explanation of version numbers, read the:
 *	1) Finder Interface Chapter, Inside Macintosh: Macintosh Toolbox Essentials.
 *	2) Code Fragment Chapter (pg 3-7), Inside Macintosh: PowerPC System Software
 *	3) SOM Developer's Guide
 *
 * Changes:
 *	$Log: LedPartVers.h,v $
 *	Revision 2.24  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.23  1996/12/05  23:00:31  lewis
 *	*** empty log message ***
 *
 *	Revision 2.22  1996/12/05  21:11:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.21  1996/10/15  19:25:35  lewis
 *	led 2.1b1.
 *
 *	Revision 2.20  1996/09/30  14:43:18  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.18  1996/07/03  01:26:40  lewis
 *	redefine preReleaseNumber to always be zero, so we don't need to update
 *	this file as often (nor the project files).
 *
 *	Revision 2.17  1996/06/01  02:28:45  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1996/05/23  20:34:32  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1996/05/14  20:58:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1996/05/05  15:03:16  lewis
 *	up version
 *
 *	Revision 2.13  1996/04/18  16:14:05  lewis
 *	up version
 *
 *	Revision 2.12  1996/03/16  19:29:08  lewis
 *	Up version #
 *
 *	Revision 2.11  1996/03/05  18:46:19  lewis
 *	2.0b5
 *
 *	Revision 2.10  1996/03/04  08:11:50  lewis
 *	2.0b4
 *
 *	Revision 2.9  1996/02/26  23:05:46  lewis
 *	Upped version#
 *
 *	Revision 2.8  1996/02/05  05:05:31  lewis
 *	2.0b2
 *
 *	Revision 2.7  1996/01/22  05:50:21  lewis
 *	2.0b1
 *
 *	Revision 2.6  1996/01/11  08:49:55  lewis
 *	inc version#
 *
 *	Revision 2.5  1996/01/04  00:22:54  lewis
 *	2.0a7(_hex_version_ etc).
 *
 *	Revision 2.4  1995/12/16  05:03:14  lewis
 *	updated som version numbers
 *
 *	Revision 2.3  1995/12/15  04:02:07  lewis
 *	New numbering scheme for SOM version IDs so I sholdn't have to checkin
 *	new project files for every subrelease anymore.
 *
 *
 *
 *
 *
 */
#ifndef _LEDPARTVERS_
#define _LEDPARTVERS_



#include	"LedConfig.h"


// Development Stages
#define dsUndefined		0x00
#define dsPreAlpha		0x20
#define dsAlpha			0x40
#define dsBeta			0x60
#define dsFinal			0x80
#define dsReleased		dsFinal
#define dsGoldenMaster	dsFinal


// Current Major Version (version = major.minor.fix)
#define currentMajorVersion		qLed_Version_Major
#define currentMinorVersion		qLed_Version_Minor
#define currentFixVersion		0
#if		qLed_Version_Stage == qLed_Version_Stage_Dev
	#define developmentStage		dsPreAlpha
#elif	qLed_Version_Stage == qLed_Version_Stage_Alpha
	#define developmentStage		dsAlpha
#elif	qLed_Version_Stage == qLed_Version_Stage_Beta
	#define developmentStage		dsBeta
#elif	qLed_Version_Stage == qLed_Version_Stage_ReleaseCandidate
	#define developmentStage		dsFinal
#elif	qLed_Version_Stage == qLed_Version_Stage_Release
	#define developmentStage		dsFinal
#else
	#error	"bad stage?"
#endif
// define preReleaseNumber to zero to avoid always having to rebuild/change SOM version#!
// LGP 960702
//#define preReleaseNumber		qLed_Version_SubStage
#define preReleaseNumber		0



// Old Compatibility Definition Major Version (for CFM only)
#define oldCompDefnMajorVersion	0x00
// Old Compatibility Definition Minor Version (for CFM only)
#define oldCompDefnMinorVersion	0x00
// Old Compatibility Definition Fix Version (for CFM only)
#define oldCompDefnFixVersion	0x00
// Pre-release Number
#define oldCompDefnPreRelNumber	0x00
// Development Stage
#define oldCompDefnDevStage		dsUndefined



// ¥ Generated Version Numbers ¥
//       (Don't Change!!)
//#define currentVersion	(currentMajorVersion<<24)+(currentMinorVersion<<20)+(currentFixVersion<<16) +(developmentStage<<8)+preReleaseNumber
#define currentVersion	0xff
#define compatibleVersion	(oldCompDefnMajorVersion<<24)+(oldCompDefnMinorVersion<<20)	\
							+(oldCompDefnFixVersion<<16)+(oldCompDefnDevStage<<8)+oldCompDefnPreRelNumber
#define finderMinorVersion	(currentMinorVersion<<4)+(currentFixVersion<<0)

#if		defined (__SOMIDL__) || defined (__cplusplus)
	#define	_hex_version_		0xff
	#define	_decimal_version_	255
	//#define	_hex_version_		0x02106000
	//#define	_decimal_version_	34627584
	#if		_hex_version_ != _decimal_version_
		#error	"whoops, safety check! - need to put the DECIMAL version into PEF settings in CW IDE"
	#endif
	#if		currentVersion != _decimal_version_
		#error	"must update project file as well - version should be " currentVersion
	#endif
#endif


#endif // _LedPartVERSION_
