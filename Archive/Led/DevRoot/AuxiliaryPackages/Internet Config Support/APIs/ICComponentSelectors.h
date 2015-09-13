/* ���Start Header��� */

/* File:         ICComponentSelectors.h
 * Generated by: 1.0d4
 * For:          IC 1.3
 * On:           Sunday, 14 July 1996, 20:19:55
 * 
 * This file is part of the Internet Configuration system and
 * is placed in the public domain for the benefit of all.
 */

/* ���End Header��� */

/*
	IMPORTANT NOTES ABOUT THE C CONVERSION
	--------------------------------------
	
	o Pascal "var" parameters are converted from "var x : y" to "y *x".  This
	  means that when you see the parameter "y *x" you should be aware that
		you *cannot pass in nil*.  In future this restriction may be eased,
		especially for the attr parameter to ICGetPref.  Parameters where nil
		is legal are declared using the explicit pointer type, ie "yPtr x".

	o Strings are *Pascal* strings.  This means that they must be word aligned.
		MPW and Think C do this automatically.  The last time I check, Metrowerks
		C does not.  If it still doesn't, then IMHO it's a bug in their compiler
		and you should report it to them.

	o The canonical Internet Config interface is defined in Pascal.  These
		headers have not been thoroughly tested.  If there is a conflict between
		these headers and the Pascal interfaces, the Pascal should take precedence.
*/

/* ///////////////////////////////////////////////////////////////////////////////// */

#ifndef __ICCOMPONENTSELECTORS__
#define __ICCOMPONENTSELECTORS__

#ifndef __TYPES__
#include <Types.h>
#endif

/* ///////////////////////////////////////////////////////////////////////////////// */

enum {
/* ���Start ICCSel.h��� */

  kICCStart = 0,
  kICCStop = 1,
  kICCFindConfigFile = 2,
  kICCFindUserConfigFile = 14,
  kICCGeneralFindConfigFile = 30,
  kICCChooseConfig = 33,
  kICCChooseNewConfig = 34,
  kICCGetConfigName = 35,
  kICCGetConfigReference = 31,
  kICCSetConfigReference = 32,
  kICCSpecifyConfigFile = 3,
  kICCGetSeed = 4,
  kICCGetPerm = 13,
  kICCDefaultFileName = 11,
  kICCBegin = 5,
  kICCGetPref = 6,
  kICCSetPref = 7,
  kICCFindPrefHandle = 36,
  kICCGetPrefHandle = 26,
  kICCSetPrefHandle = 27,
  kICCCountPref = 8,
  kICCGetIndPref = 9,
  kICCDeletePref = 12,
  kICCEnd = 10,
  kICCEditPreferences = 15,
  kICCParseURL = 16,
  kICCLaunchURL = 17,
  kICCMapFilename = 24,
  kICCMapTypeCreator = 25,
  kICCMapEntriesFilename = 28,
  kICCMapEntriesTypeCreator = 29,
  kICCCountMapEntries = 18,
  kICCGetIndMapEntry = 19,
  kICCGetMapEntry = 20,
  kICCSetMapEntry = 21,
  kICCDeleteMapEntry = 22,
  kICCAddMapEntry = 23,
  kICC_first_select = kICCStart,
  kICC_last_select = 36

/* ���End ICCSel.h��� */
};

#endif
