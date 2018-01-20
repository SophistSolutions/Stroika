/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef __ActiveLedItConfig_h__
#define __ActiveLedItConfig_h__ 1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/ActiveLedItConfig.h,v 2.23 2004/02/09 16:25:07 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *  $Log: ActiveLedItConfig.h,v $
 *  Revision 2.23  2004/02/09 16:25:07  lewis
 *  SPR#1599: Added qKeepListOfALInstancesForSPR_1599BWA  for MSIE quit without closing last control bug. For SPR#1641: added assert to check MODULE::m_nObjectCount = 0. Had  to be much more careful about using OLE_GetXXX functions that return IDispatch*. Instead - do non-OLE_ versions that return CComPtr<IDispatch> - so we don't get redundant refcounts.
 *
 *  Revision 2.22  2004/01/23 23:02:52  lewis
 *  define qActiveLedIt_MajorVersion,qActiveLedIt_MinorVersion, and qActiveLedIt_MajorDotMinorVersion
 *  defines for use in IDL file
 *
 *  Revision 2.21  2003/11/27 03:59:09  lewis
 *  SPR#1565: set maxundos to 512
 *
 *  Revision 2.20  2003/06/03 16:35:19  lewis
 *  SPR#1522: have control goto Active state - rather than UIActive - on OLEVERB_SHOW
 *
 *  Revision 2.19  2003/05/28 16:56:17  lewis
 *  SPR#1509: Added qFunnyDisplayInDesignMode define to ActiveLedItConfig, and ifdefined (yes by default),
 *  then check for AMBIENT_DESIGN_MODE and change the drawing in LedItView::EraseBackground () slightly.
 *  Added a small watermarkdisplay saying 'design mode' and a border in the color opposite to the
 *  background. Hooked OnAmbientPropertyChange() to detect change and refresh screen.
 *
 *  Revision 2.18  2003/03/11 02:33:09  lewis
 *  SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand, and other
 *  cleanups to Led_URLManager calls and context menu
 *
 *  Revision 2.17  2002/05/06 21:34:18  lewis
 *  <=============================== Led 3.0.1 Released ==============================>
 *
 *  Revision 2.16  2001/11/27 00:32:28  lewis
 *  <=============== Led 3.0 Released ===============>
 *
 *  Revision 2.15  2001/08/30 00:35:56  lewis
 *  *** empty log message ***
 *
 *  Revision 2.14  2001/08/28 18:23:19  lewis
 *  set qMagicVersionExtra back to zero
 *
 *  Revision 2.13  2001/08/15 21:08:24  lewis
 *  qMagicVersionExtra so I can send an intermediate build to someone
 *
 *  Revision 2.12  2001/07/31 15:37:56  lewis
 *  *** empty log message ***
 *
 *  Revision 2.11  2001/05/30 16:28:41  lewis
 *  *** empty log message ***
 *
 *  Revision 2.10  2001/05/30 16:23:03  lewis
 *  SPR#0945- new VersionNumber/ShortVersionString OLEAUT APIs
 *
 *  Revision 2.9  2001/04/20 19:10:20  lewis
 *  *** empty log message ***
 *
 *  Revision 2.8  2001/04/12 18:58:11  lewis
 *  cleanup DemoMode warning expired code (so easier to include in other apps)
 *
 *  Revision 2.7  2001/03/28 20:04:23  lewis
 *  added tmphack qMagicVersionExtra so I can solve version# problem for a
 *  cusomer shipping an interum release
 *
 *  Revision 2.6  2000/10/22 17:37:19  lewis
 *  SPR#0841- change version# scheme for ActiveLedIt! binary version#
 *
 *  Revision 2.5  2000/04/14 22:40:44  lewis
 *  SPR#0740- namespace support
 *
 *  Revision 2.4  1999/11/13 16:54:44  lewis
 *  lose hacks for 2.3.2 release - hardwired numbers
 *
 *  Revision 2.3  1999/09/20 13:17:00  lewis
 *  some hacks todo interum 2.3.2 release - but mostly support for new DEMO_MODE build
 *
 *  Revision 2.2  1997/12/24 04:43:52  lewis
 *  *** empty log message ***
 *
 *  Revision 2.1  1997/07/27  15:59:52  lewis
 *  <===== Led 2.2 Released =====>
 *
 *  Revision 2.0  1997/06/18  03:25:57  lewis
 *  *** empty log message ***
 *
 *
 *
 *
 *
 */

#if defined(__cplusplus)
#include "Stroika/Foundation/StroikaPreComp.h"
#endif

#include "Stroika/Frameworks/Led/Config.h"

#if defined(__cplusplus)
#include "Stroika/Frameworks/Led/Support.h"
#endif

#if defined(__cplusplus)
// Due to work on SPR#1564 and 1565 - up this undo count from 3 to something effectively infinite.
const unsigned int kMaxNumUndoLevels = 512;
#endif

#if qDemoMode
#define kDemoString " - Demo Version"
#else
#define kDemoString ""
#endif

//TMP HACK - to workaround problem introduced by interum release with ActiveLedIt! 3.0b5 - get rid of after b5 release
#define qMagicVersionExtra 0

#ifndef qDemoMode
#define qDemoMode 0
#endif

#define qActiveLedIt_MajorVersion qLed_Version_Major
#define qActiveLedIt_MinorVersion qLed_Version_Minor

#if qActiveLedIt_MajorVersion == 3 && qActiveLedIt_MinorVersion == 1
#define qActiveLedIt_MajorDotMinorVersion 3.1
#elif (qActiveLedIt_MajorVersion == 4) && (qActiveLedIt_MinorVersion == 0)
#define qActiveLedIt_MajorDotMinorVersion 4.0
#else
//tmphack - LGP 2012-09-04 - cuz we've let yet to fix version# stuff
#error "OOOPS"
#endif

#if qDemoMode
#define kVERSResourceVersionNumber qLed_Version_MajorMinor, qLed_Version_Stage, qLed_Version_SubStage, 0 + qMagicVersionExtra
#else
#define kVERSResourceVersionNumber qLed_Version_MajorMinor, qLed_Version_Stage, qLed_Version_SubStage, 1 + qMagicVersionExtra
#endif
#if defined(__cplusplus)
static_assert (qHasFeature_ATLMFC, "Error: ActiveLedIt requires the ATLMFC feature to be set true when building Stroika");

inline unsigned long mkActiveLedItVersion_ (unsigned int majorMinor, unsigned int versStage, unsigned int subStage, unsigned int nonDemoFlag)
{
    Require (majorMinor <= 256);
    Require (versStage <= 256);
    Require (subStage <= 256);
    Require (nonDemoFlag <= 256);
    return (static_cast<unsigned long> (majorMinor) << 24) |
           (static_cast<unsigned long> (versStage) << 16) |
           (static_cast<unsigned long> (subStage) << 8) |
           (static_cast<unsigned long> (nonDemoFlag) << 0);
}
const unsigned long kActiveLedItDWORDVersion = mkActiveLedItVersion_ (qLed_Version_MajorMinor, qLed_Version_Stage, qLed_Version_SubStage, !qDemoMode + qMagicVersionExtra);
#endif

#if defined(__cplusplus)
const char kAppName[] = "ActiveLedIt";
#if qDemoMode
const char kURLDemoFlag[] = "&DemoMode=true";
#else
const char kURLDemoFlag[] = "";
#endif
const char kDemoExpiredExtraArgs[] = "&DemoExpired=true";
#endif

#ifndef qKeepListOfALInstancesForSPR_1599BWA
#define qKeepListOfALInstancesForSPR_1599BWA 1
#endif

#ifndef qFunnyDisplayInDesignMode
#define qFunnyDisplayInDesignMode 1
#endif

/*
 *  Somewhat risky change for SPR#1522. By default - MFC does a UI-Activate when a OLEIVERB_SHOW
 *  mesage is sent. That causes some problems for some folks, so disable it and see if anybody
 *  yells (and I did a lot of testing myself - see the SPR for details) - LGP 2003-06-03.
 */
#ifndef qDontUIActivateOnOpen
#define qDontUIActivateOnOpen 1
#endif

#endif /*__ActiveLedItConfig_h__*/
