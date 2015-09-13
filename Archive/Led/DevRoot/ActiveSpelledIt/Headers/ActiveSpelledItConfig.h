/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveSpelledItConfig_h__
#define	__ActiveSpelledItConfig_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Headers/ActiveSpelledItConfig.h,v 1.4 2003/11/03 20:16:37 lewis Exp $
 */

/*
 * Changes:
 *	$Log: ActiveSpelledItConfig.h,v $
 *	Revision 1.4  2003/11/03 20:16:37  lewis
 *	add Win32ErrorException/HRESULTErrorException catch support
 *	
 *	Revision 1.3  2003/06/10 16:30:31  lewis
 *	*** empty log message ***
 *	
 *
 *
 *
 */

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_ALL_WARNINGS					// turns off ATL's hiding of some common and often safely ignored warning messages

#include	<atlbase.h>
#include	<atlcom.h>

#include	"LedConfig.h"
#include	"LedSupport.h"

#include	"Resource.h"

#if		qDemoMode
	#include	<time.h>
	#include	"LedOptionsSupport.h"
#endif


using	namespace	ATL;
using	namespace	Led;

#if		qDemoMode
#define	kDemoString " - Demo Version"
#else
#define	kDemoString ""
#endif

#ifndef	qDemoMode
#define	qDemoMode	0
#endif


#define	CATCH_AND_HANDLE_EXCEPTIONS()\
	catch (Win32ErrorException& we) {\
		return HRESULT_FROM_WIN32 (we);\
	}\
	catch (HRESULTErrorException& h) {\
		return static_cast<HRESULT> (h);\
	}\
	catch (HRESULT hr) {\
		return hr;\
	}\
	catch (...) {\
		return E_FAIL;\
	}\



#if		qDemoMode
	#define	kVERSResourceVersionNumber	qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,0
#else
	#define	kVERSResourceVersionNumber	qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,1
#endif
#if		defined (__cplusplus)
inline	unsigned long	mkActiveSpelledItVersion_ (unsigned int majorMinor, unsigned int versStage, unsigned int subStage, unsigned int nonDemoFlag)
	{
		Led_Require (majorMinor <= 256);
		Led_Require (versStage <= 256);
		Led_Require (subStage <= 256);
		Led_Require (nonDemoFlag <= 256);
		return
				(static_cast<unsigned long> (majorMinor)<<24) |
				(static_cast<unsigned long> (versStage)<<16) |
				(static_cast<unsigned long> (subStage)<<8) |
				(static_cast<unsigned long> (nonDemoFlag)<<0)
			;
	}
const unsigned long kActiveSpelledItDWORDVersion = mkActiveSpelledItVersion_ (qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,!qDemoMode);
#endif

const	char	kAppName[]				=	"ActiveSpelledIt";
#if		qDemoMode
	const	char	kURLDemoFlag[]			=	"&DemoMode=true";
#else
	const	char	kURLDemoFlag[]			=	"";
#endif
const	char	kDemoExpiredExtraArgs[]	=	"&DemoExpired=true";



#if		qDemoMode
class	DemoPrefs : public OptionsFileHelper {
	public:
		class	RegistryEntryNotFound {};
		enum	{ kDaysToExpire = 7 };
		enum	{ kDaysToExpireForNewVersion = 1 };	// if they had an old version and IT expired, and tried a new version - give shorter deadline

	public:
		DemoPrefs ():
			OptionsFileHelper (OpenTheKey ())
			{
			}
		HKEY	OpenTheKey ()
			{
				CRegKey	softwarePrefs;
				softwarePrefs.Open (HKEY_CURRENT_USER, _T ("Software"));
				if (softwarePrefs == NULL) {
					throw RegistryEntryNotFound ();
				}
				CRegKey	sophistsKey;
				sophistsKey.Create (softwarePrefs, _T ("Sophist Solutions, Inc."));
				if (sophistsKey == NULL) {
					throw RegistryEntryNotFound ();
				}
				CRegKey	commonPrefsKey;
				commonPrefsKey.Create (sophistsKey, _T ("ActiveSpelledIt"));
				if (commonPrefsKey == NULL) {
					throw RegistryEntryNotFound ();
				}
				return commonPrefsKey.Detach ();
			}
		static	inline	unsigned	GetDemoDaysLeftHelper (unsigned demoExpiresAt)
			{
				time_t		curTime		=	time (NULL);
				if (static_cast<time_t> (demoExpiresAt) < curTime) {
					return 0;
				}
				unsigned	dayCount	=	(demoExpiresAt - curTime)/ (60*60*24);
				dayCount += 1;	// cuz above code rounds down
				if (dayCount > kDaysToExpire+1) {
					// cheaters never propser!
					return 0;
				}
				return dayCount;
			}
		UINT	GetDemoDaysLeft ()
			{
				int		demoVersion		=	0;
				int		demoExpiresAt	=	0;
				if (LookupPref ("DemoExpiresAt", &demoExpiresAt) and LookupPref ("DemoVersionNumber", &demoVersion)) {
					UINT	daysLeft	=	GetDemoDaysLeftHelper (demoExpiresAt);
					if (daysLeft > 0) {
						return daysLeft;
					}

					/*
					 *	If they install a NEWER version - then given them a little extra time.
					 */
					if (static_cast<unsigned int> (demoVersion) < kActiveSpelledItDWORDVersion) {
						demoExpiresAt = time (NULL) + kDaysToExpireForNewVersion*24*60*60;
						StorePref ("DemoExpiresAt", demoExpiresAt);
						StorePref ("DemoVersionString", qLed_ShortVersionString);
						StorePref ("DemoVersionNumber", static_cast<int> (kActiveSpelledItDWORDVersion));
						return kDaysToExpireForNewVersion;
					}
					return daysLeft;
				}
				else {
					/*
					 *	New installation.
					 */
					demoExpiresAt = time (NULL) + kDaysToExpire*24*60*60;
					StorePref ("DemoExpiresAt", demoExpiresAt);
					StorePref ("DemoVersionString", qLed_ShortVersionString);
					StorePref ("DemoVersionNumber", static_cast<int> (kActiveSpelledItDWORDVersion));
					return kDaysToExpire;
				}
			}
};
#endif


#if		qDemoMode
class	DemoModeAlerter {
	public:
		static	void	ShowAlert (HWND parentWnd = 0);
		static	void	ShowAlertIfItsBeenAWhile (HWND parentWnd = 0);
		static	int		sNextWarningAfter;
		static	const	float	kTimeBetweenWarnings;	// n SECONDs minimum delay between warnings 
};
#endif


#if		qDemoMode
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(w)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlert (w);\
			return;\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK(w,x)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlert (w);\
			return (x);\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX(w,x)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlertIfItsBeenAWhile (w);\
			return (x);\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN(w)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlertIfItsBeenAWhile (w);\
			return;\
		}
	#define	CHECK_DEMO_AND_BEEP_AND_RETURN()\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			Led_BeepNotify ();\
			return;\
		}
#else
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(w)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK(w,x)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN(w)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX(w,x)
	#define	CHECK_DEMO_AND_BEEP_AND_RETURN()
#endif




#endif
