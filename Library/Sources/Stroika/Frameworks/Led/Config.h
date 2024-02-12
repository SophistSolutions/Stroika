/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_Config_h_
#define _Stroika_Framework_Led_Config_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Configuration/StroikaVersion.h"
#include "../../Foundation/Configuration/VersionDefs.h"

/*
@MODULE:    LedConfig
@DESCRIPTION:
        <p>Support basic configuration defines for the operating system, and compiler, and some basic features
    (like character set).
    </p>
 *
 *  TODO:
 *      @todo   MAJOR CLAENAUP here required. Still alot of this overlaps with Stroika stuff (like qDebug) - LOSE
 *              overalaps and configs no longer used.
 *
 */

#if qPlatform_MacOS
#if defined(__cplusplus) && !defined(__SOMIDL__)
#include <ConditionalMacros.h>
#endif
#endif

/*
 *  This Numeric Led version is intended to allow you to conditionally compile code
 *  based on different versions of Led. The Led::kVersion string is a symbolic representation
 *  of this version.
 *
 *  Don't count on the particular hard-wirded version number. Instead - compare as follows:
 *      #if     qLed_FullVersion > MAKE_LED_FULL_VERSION (1, 0, qLed_Version_Stage_Beta, 3, 1)
 *          DOTHIS ();
 *      #else
 *          DOTHAT ();
 *      #endif
 */

// This part is where we actually update the version#
#define qLed_Version_Major 4
#define qLed_Version_Minor 0
#define qLed_Version_MajorMinor 0x40
#define qLed_Version_Stage kStroika_Version_Stage
#define qLed_Version_SubStage kStroika_Version_SubStage
#define qLed_Version_FinalBuild kStroika_Version_FinalBuild

// MAJOR VERSION OF LED is 2 + VERSION OF STROIKA
#if kStroika_Version_Major == 3 && kStroika_Version_Minor == 0
#define qLed_MajorMinorVersionString "5.0"
#else
#warning "Led version needs manual update here..."
#define qLed_MajorMinorVersionString "4.1"
#endif
#if kStroika_Version_Stage == kStroika_Version_Stage_Dev
#define __vLet1__ "d"
#elif kStroika_Version_Stage == kStroika_Version_Stage_Alpha
#define __vLet1__ "a"
#elif kStroika_Version_Stage == kStroika_Version_Stage_Beta
#define __vLet1__ "b"
#elif kStroika_Version_Stage == kStroika_Version_Stage_ReleaseCandidate
#define __vLet1__ "rc"
#else
#define __vLet1__ ""
#endif

#define _STR_HELPER(x) #x
#define _STR(x) _STR_HELPER (x)
#define __vLet2__ _STR (kStroika_Version_SubStage)

#if qLed_Version_FinalBuild
#define qLed_ShortVersionString qLed_MajorMinorVersionString __vLet1__ __vLet2__
#else
#define qLed_ShortVersionString qLed_MajorMinorVersionString __vLet1__ __vLet2__ "x"
#endif

#define qLed_FullVersion                                                                                                                   \
    Stroika_Make_FULL_VERSION (qLed_Version_Major, qLed_Version_Minor, qLed_Version_Stage, qLed_Version_SubStage, qLed_Version_FinalBuild)

// #if qPlatform_MacOS + qPlatform_Windows + qStroika_FeatureSupported_XWindows > 1
// #error "Only one of these should be defined"
// #endif
// #if !qPlatform_MacOS && !qPlatform_Windows && !qStroika_FeatureSupported_XWindows
// #error "One of these should be defined - right now thats all we support"
// #endif

/*
 *  Do things like memcpy () get inlined by the compiler, or should we do so ourselves
 *  manually for cases that matter.
 */
#ifndef qComplilerDoesNotInlineIntrinsics
#define qComplilerDoesNotInlineIntrinsics 0
#endif

/*
 *  Not a bug to have this - I think - but doesn't appear part of any ansi spec???
 */
//#ifndef qHasIsAscii
///#define qHasIsAscii 0
//#endif

/*
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 */

/*
@CONFIGVAR:     qHeavyDebugging
@DESCRIPTION:   <p>Led contains lots of internal debugging code which is mainly intended to find bugs
    in Led itself. Its less valuable for finding bugs in your programs' usage of Led. Much of that debugging
    code makes Led quite slow (for larger documents).</p>
        <p>The Led sample applications are all built with qHeavyDebugging when qDebug is on. But Led defaults
    to having this value false, so that your applications won't be needlessly slow.</p>
        <p>If you run into some subtle bug, or if you aren't worried about the speed of Led with large documents
when debugging is ON, then you may want to turn this flag ON.</p>
        <p>See also @'qDebug'</p>
 */
#ifndef qHeavyDebugging
#define qHeavyDebugging 0
#endif

/*
@CONFIGVAR:     qNestedTablesSupported
@DESCRIPTION:   <p>This define controls whether or not the Led WordProcessor/StyledTextIO class support nested tables.
            This will <em>not</em> be supported for Led 3.1, but probably will be in a release thereafter.
            There is SOME code available for both cases, but just a little (as of 2003-04-12).</p>
 */
#ifndef qNestedTablesSupported
#define qNestedTablesSupported 0
#endif

/*
 *  -----------------------        Character set Settings/Specifications         ---------------------------
 */

/*
@CONFIGVAR:     qSingleByteCharacters
@DESCRIPTION:   <p>Define this if you will only be editing 8-bit ascii or extended ASCII characters, and don't
            want to deal with UNICODE.</p>
                <p>See also @'qMultiByteCharacters' and @'qWideCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Defaults OFF</p>
 */
#ifndef qSingleByteCharacters
#define qSingleByteCharacters 0
#endif

/*
@CONFIGVAR:     qMultiByteCharacters
@DESCRIPTION:   <p>Define this if you will only be editing multibyte text (e.g. SJIS) and don't
            want to deal with UNICODE.</p>
                <p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Note that this is really only partially still supported, and may not be supported in the future (as of Led 3.1a4).
            To make this define work - you must implement yourself @'Led_IsLeadByte' and @'Led_IsValidSecondByte' and perhaps
            a few other small matters. The problem with doing this is that its not clear WHICH mutibyte code set you are compiling for.
            Led's multibyte support is designed to only work for a particular multibyte code set at a time (though this could
            be a global variable - perhaps - rather than a compile-time constant). Anyhow - if @'qSingleByteCharacters' isn't good
            enough - we strongly recomend using UNICODE (@'qWideCharacters').</p>
                <p>Defaults OFF</p>
 */
#ifndef qMultiByteCharacters
#define qMultiByteCharacters 0
#endif

/*
@CONFIGVAR:     qWideCharacters
@DESCRIPTION:   <p>Define this to work with the widest variety of characters - UNICODE. This is the preferred way
            to build Led, and as a result - is now the default (as of Led 3.1a4).</p>
                <p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Defaults TRUE (if @'qMultiByteCharacters', and @'qSingleByteCharacters' are false).</p>


                \NOTE - as of Stroika 3.0d2 - we will continue supporting this flag, but will switch to always using UNICODE.
                       Just using char16_t for led_tChar vs. char8_t. NOT NOW - big change. But worthwhile improvement.
*/
#ifndef qWideCharacters
#if qMultiByteCharacters || qSingleByteCharacters
#define qWideCharacters 0
#else
#define qWideCharacters 1
#endif
#endif

// Be sure ONLY ONE of these flags set
#if qSingleByteCharacters && (qMultiByteCharacters || qWideCharacters)
#error "Only one can be set at a time"
#endif
#if qMultiByteCharacters && (qSingleByteCharacters || qWideCharacters)
#error "Only one can be set at a time"
#endif
#if qWideCharacters && (qSingleByteCharacters || qSingleByteCharacters)
#error "Only one can be set at a time"
#endif
#if !qSingleByteCharacters && !qMultiByteCharacters && !qWideCharacters
#error "At least one should be set"
#endif

/*
@CONFIGVAR:     qProvideIMESupport
@DESCRIPTION:   <p>Do we need to explictly support the IME?</p>
        <p>NB: qProvideIMESupport used to be called qRequireIMESupport, but now we
    simply provide it, and it essentailly becomes a no-op if you've no IME installed.</p>
        <p>This is ON by default (for windows). It degrades gracefully if no IME found.</p>
 */
#ifndef qProvideIMESupport
#if qPlatform_Windows
#define qProvideIMESupport 1
#else
#define qProvideIMESupport 0
#endif
#endif

/*
@CONFIGVAR:     qPeekForMoreCharsOnUserTyping
@DESCRIPTION:   <p>A trick to get better (worse?) interactivity in typing.</p>
        <p>Defaults to true.</p>
 */
#ifndef qPeekForMoreCharsOnUserTyping
#define qPeekForMoreCharsOnUserTyping 1
#endif

/*
@CONFIGVAR:     qAllowRowsThatAreLongerThan255
@DESCRIPTION:   <p>This define allows control over whether we perform a packing optimization to save memory.
    If we do (qAllowRowsThatAreLongerThan255 FALSE) then we asssert were are never
    given a row > 255 pixels. Similarly, we disalow rows of more than 255 characters.
    If qAllowRowsThatAreLongerThan255 - fine - we just don't optimize packing.</p>
        <p>For now, not fully implemented. Only for RowHeights, not for RowStarts.</p>
        <p>See SPR#0257</p>
 */
#ifndef qAllowRowsThatAreLongerThan255
#define qAllowRowsThatAreLongerThan255 1
#endif

/*
@CONFIGVAR:     qDoubleClickSelectsSpaceAfterWord
@DESCRIPTION:   <p>qDoubleClickSelectsSpaceAfterWord is a very silly idea, imho, but it is a standard UI feature
    on MS Windows editors, and not uncommon on the Mac. As far as I can tell, the idea
    is just to select a word, plus all trailing whitespace on double clicks. The idea is
    that this somehow makes cut/paste work more easily. So it is sometimes refered
    to as smart cut and paste or intelligent cut and paste.</p>
        <p>By default I USED TO enable this only for windows.
    Now it is always OFF by default, since I think its too stupid, and annoying.
    Someday soon I'll implement better smart-cut/paste like Style(on Mac) does.</p>
 */
#ifndef qDoubleClickSelectsSpaceAfterWord
#if qPlatform_Windows
#define qDoubleClickSelectsSpaceAfterWord 0
#else
#define qDoubleClickSelectsSpaceAfterWord 0
#endif
#endif

#ifndef qStroika_Frameworks_Led_SupportGDI
#define qStroika_Frameworks_Led_SupportGDI qPlatform_Windows
#endif

/*
@CONFIGVAR:     qDynamiclyChooseAutoScrollIncrement
@DESCRIPTION:   <p>On slower computers, this can make scrolling appear a bit faster. At some risk to
    predictablity...</p>
 */
#ifndef qDynamiclyChooseAutoScrollIncrement
#define qDynamiclyChooseAutoScrollIncrement 1
#endif

/*
@CONFIGVAR:     qUseOffscreenBitmapsToReduceFlicker
@DESCRIPTION:   <p>Led already has very little flicker. This is because we are very careful to
    draw as little as possible, and to draw quickly. But some cases still exist.
    Like large pictures being drawn are flicker, cuz we must erase the bounds and then
    draw the picture.</p>
        <p>Using this gets rid of these few cases of flicker, but at a small performance cost
    in overall draw speed.</p>
        <P> THIS FLAG IS NOW OBSOLETE (as of Led 3.0b6). Its still supported for backwards compatability.
    But now all it does is set the default VALUE for the ImageUsingOffscreenBitmaps property.</p>
        <p>Instead of using this, use @'TextImager::SetImageUsingOffscreenBitmaps' ().</p>
 */
#ifndef qUseOffscreenBitmapsToReduceFlicker
#define qUseOffscreenBitmapsToReduceFlicker 1
#endif

#if defined(__cplusplus)
namespace Stroika::Frameworks::Led {
}
#endif
// Be sure namespace is defined for later use.

/*
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 */
#if qPlatform_MacOS

#ifndef TARGET_CARBON
#define TARGET_CARBON 0
#endif

/*
@CONFIGVAR:     qPeekAtQuickDrawGlobals
@DESCRIPTION:   <p><b>MAC ONLY</b></p>
 */
#if defined(__cplusplus)
#ifndef qPeekAtQuickDrawGlobals
#if TARGET_CARBON
#define qPeekAtQuickDrawGlobals 0
#else
#if GENERATINGCFM
#define qPeekAtQuickDrawGlobals 0
#else
#define qPeekAtQuickDrawGlobals 1
#endif
#endif
#endif
#endif

/*
@CONFIGVAR:     qUseMacTmpMemForAllocs
@DESCRIPTION:   <p><b>MAC ONLY</b></p>
        <p>Use of TmpMemory on the mac allows for the editor to run in a small partition
    but to use all available system resources (memory) when the user requests it
    for a large clipbaord operation, or opening a large file, or whatever.</p>
        <p>I believe it generally makes sense to leave this on.</p>
        <p>LGP - 960314</p>
 */
#if !defined(qUseMacTmpMemForAllocs)
#define qUseMacTmpMemForAllocs 1
#endif

#endif /*qPlatform_MacOS*/

/*
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 */
#if qPlatform_Windows

/*
@CONFIGVAR:     qUseActiveXToOpenURLs
@DESCRIPTION:   <p><b>Win32 ONLY</b></p>
        <p>For supporting openening URLs.</p>
 */
#ifndef qUseActiveXToOpenURLs
#define qUseActiveXToOpenURLs 1
#endif

#endif /*qPlatform_Windows*/

/*
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 */

#endif /*_Stroika_Framework_Led_Config_h_*/
