/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_Win32_FileRegistration_h__
#define	__Led_Win32_FileRegistration_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_Win32_FileRegistration.h,v 2.6 2003/04/16 17:26:46 lewis Exp $
 */


/*
@MODULE:	Led_Win32_FileRegistration
@DESCRIPTION:
		<p>Functionality not directly related to Led - per-se, but often helpful in building Led-based applications.
	This code helps you to update the windows registration to say what file types you support.</p>
 */



/*
 * Changes:
 *	$Log: Led_Win32_FileRegistration.h,v $
 *	Revision 2.6  2003/04/16 17:26:46  lewis
 *	SPR#1440: added new optional edit/open facility to Win32UIFileAssociationInfo. Change backward compatabile - old CTOR sets BOTH open and edit (where it used to just do open)
 *	
 *	Revision 2.5  2002/05/06 21:33:30  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.4  2001/11/27 00:29:42  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/10/17 20:42:51  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.2  2001/09/16 21:13:39  lewis
 *	fix so builds on Win32 FULL_UNICODE
 *	
 *	Revision 2.1  2001/09/16 18:33:06  lewis
 *	SPR#1033 - added new Led_Win32_FileRegistration with classes to support updating file assoc registry
 *	
 *	
 *
 *
 *
 */

#include	"LedSupport.h"





#if		qLedUsesNamespaces
namespace	Led {
#endif




#if		qWindows
class	Win32FileAssociationRegistrationHelper {
	public:
		Win32FileAssociationRegistrationHelper (const Led_SDK_String& fileSuffix);

		nonvirtual	Led_SDK_String	GetAssociatedProgID () const;
		nonvirtual	Led_SDK_String	GetAssociatedDefaultIcon () const;
		nonvirtual	Led_SDK_String	GetAssociatedEditCommand () const;
		nonvirtual	Led_SDK_String	GetAssociatedOpenCommand () const;

	public:
		nonvirtual	void	SetAssociatedProgIDAndOpenCommand (const Led_SDK_String& progID, const Led_SDK_String& progIDPrettyName, const Led_SDK_String& defaultIcon, const Led_SDK_String& editCommandLine, const Led_SDK_String& openCommandLine);

	private:
		struct	KeyHolder {
			HKEY	fKey;
			KeyHolder (HKEY hk);
			enum CreateIfNotThereFlag { eCreateIfNotThere };
			KeyHolder (HKEY baseKey, LPCTSTR lpSubKey);
			KeyHolder (HKEY baseKey, LPCTSTR lpSubKey, CreateIfNotThereFlag createIfNotThereFlag);
			~KeyHolder ();
			operator HKEY ();
		};
	private:
		Led_SDK_String	fFileSuffix;
};
#endif




#if		qWindows
class	Win32UIFileAssociationInfo {
	public:
		Win32UIFileAssociationInfo (
								const Led_SDK_String& fileSuffix,
								const Led_SDK_String& fileProgID,
								const Led_SDK_String& fileProgIDPrettyName,
								const Led_SDK_String& defaultIcon,
								const Led_SDK_String& shellEditNOpenCommandLine
						);
		Win32UIFileAssociationInfo (
								const Led_SDK_String& fileSuffix,
								const Led_SDK_String& fileProgID,
								const Led_SDK_String& fileProgIDPrettyName,
								const Led_SDK_String& defaultIcon,
								const Led_SDK_String& shellEditCommandLine,
								const Led_SDK_String& shellOpenCommandLine
						);
	public:
		Led_SDK_String	fFileSuffix;
		Led_SDK_String	fFileProgID;
		Led_SDK_String	fFileProgIDPrettyName;
		Led_SDK_String	fDefaultIcon;
		Led_SDK_String	fShellEditCommandLine;
		Led_SDK_String	fShellOpenCommandLine;
	public:
		static	Led_SDK_String	kNoChange;
};
#endif





#if		qWindows
class	Win32UIFileAssociationRegistrationHelper {
	public:
		Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance);
		Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance, const vector<Win32UIFileAssociationInfo>& infoRecs);

	public:
		nonvirtual	void	Add (const Win32UIFileAssociationInfo& infoRec);

	public:
		nonvirtual	void	DoIt () throw ();

	public:
		nonvirtual	bool	RegisteredToSomeoneElse () const;
		nonvirtual	void	ApplyChangesSilently ();
		virtual		bool	CheckUserSaysOKToUpdate () const;
		virtual		void	ExpandVariables (Led_SDK_String* valWithVars) const;

	private:
		HINSTANCE							fHINSTANCE;
		vector<Win32UIFileAssociationInfo>	fInfoRecs;
};
#endif






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#if		qLedUsesNamespaces
}
#endif


#endif	/*__Led_Win32_FileRegistration_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
