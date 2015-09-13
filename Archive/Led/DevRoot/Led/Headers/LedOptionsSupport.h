/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	____Led_OptionsSupport_h__
#define	____Led_OptionsSupport_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/LedOptionsSupport.h,v 2.7 2004/02/11 01:07:26 lewis Exp $
 */


/*
@MODULE:	LedOptionsSupport
@DESCRIPTION:
		<p></p>
 */

/*
 * Changes:
 *	$Log: LedOptionsSupport.h,v $
 *	Revision 2.7  2004/02/11 01:07:26  lewis
 *	SPR#1635: work around bugs qDefaultValueInTemplateParemeterUsingTemplatedTypeBug and qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
 *	
 *	Revision 2.6  2003/12/31 03:36:17  lewis
 *	SPR#1602: major improvements. Added lots of overloaded set/get methods, including arrays
 *	of elts. Added method to compute new key with a path - construcing elts along the way.
 *	Support UNICODE/non-UNICODE strings much better
 *	
 *	Revision 2.5  2002/05/06 21:33:26  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.4  2001/11/27 00:29:39  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/10/17 00:30:50  lewis
 *	Doc Comments
 *	
 *	Revision 2.2  2001/08/28 18:43:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  2001/04/12 18:57:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  2000/10/03 12:53:23  lewis
 *	added new LedOptionsSupport module for helper classes like Watermark
 *	drawing class that didnt really fit anyplace else
 *	
 *
 *
 */


#include	"LedSupport.h"





#if		qLedUsesNamespaces
namespace	Led {
#endif


/*
@CLASS:			OptionsFileHelper
@DESCRIPTION:	<p>Helper class to access options files (on Win32 -
			registry based, and on MacOS - resource file based).</p>
				<p>NB: only Windows is implemented so far (2003-12-30).</p>
*/
class	OptionsFileHelper {
	public:
	#if		qMacOS
		OptionsFileHelper (int resFile);
	#elif	qWindows
		OptionsFileHelper (HKEY hkey);
	#endif
		virtual ~OptionsFileHelper ();

#if		qWindows
	public:
		static	HKEY	OpenWithCreateAlongPath (HKEY parentKey, const Led_SDK_String& path, REGSAM samDesired = KEY_READ | KEY_WRITE);
#endif

	public:
#if		qDefaultValueInTemplateParemeterUsingTemplatedTypeBug
		template	<typename T>
			nonvirtual	T	GetPref (const Led_SDK_Char* prefName, T defaultValue);
		template	<typename T>
			nonvirtual	T	GetPref (const Led_SDK_Char* prefName)
				{
					T	defValue;
					return GetPref (prefName, defValue);
				}
#else
		template	<typename T>
			nonvirtual	T	GetPref (const Led_SDK_Char* prefName, T defaultValue = T ());
#endif


#if		qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
	public:
		nonvirtual	bool	GetPref (const Led_SDK_Char* prefName, bool defaultValue);
		nonvirtual	string	GetPref (const Led_SDK_Char* prefName, string defaultValue);
		nonvirtual	wstring	GetPref (const Led_SDK_Char* prefName, wstring defaultValue);
		nonvirtual	vector<string>	GetPref (const Led_SDK_Char* prefName, vector<string> defaultValue);
		nonvirtual	vector<wstring>	GetPref (const Led_SDK_Char* prefName, vector<wstring> defaultValue);
#endif

	public:
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, string* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, wstring* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, bool* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, int* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, vector<Byte>* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, vector<string>* value);
		nonvirtual	bool	LookupPref (const Led_SDK_Char* prefName, vector<wstring>* value);

	public:
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, const string& value);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, const wstring& value);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, bool value);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, int value);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, size_t nBytes, const Byte* data);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, const vector<string>& value);
		nonvirtual	void	StorePref (const Led_SDK_Char* prefName, const vector<wstring>& value);

	private:
	#if		qMacOS
		int		fResFile;
	#elif	qWindows
		HKEY	fKey;
	#endif
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	template	<typename T>
		inline	T	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, T defaultValue)
			{
				T	result;
				if (LookupPref (prefName, &result)) {
					return result;
				}
				else {
					return defaultValue;
				}
			}


#if		qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
	inline	bool	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, bool defaultValue)
		{
			bool	result;
			if (LookupPref (prefName, &result)) {
				return result;
			}
			else {
				return defaultValue;
			}
		}
	inline	string	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, string defaultValue)
		{
			string	result;
			if (LookupPref (prefName, &result)) {
				return result;
			}
			else {
				return defaultValue;
			}
		}
	inline	wstring	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, wstring defaultValue)
		{
			wstring	result;
			if (LookupPref (prefName, &result)) {
				return result;
			}
			else {
				return defaultValue;
			}
		}
	inline	vector<string>	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, vector<string> defaultValue)
		{
			vector<string>	result;
			if (LookupPref (prefName, &result)) {
				return result;
			}
			else {
				return defaultValue;
			}
		}
	inline	vector<wstring>	OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, vector<wstring> defaultValue)
		{
			vector<wstring>	result;
			if (LookupPref (prefName, &result)) {
				return result;
			}
			else {
				return defaultValue;
			}
		}
#endif


#if		qLedUsesNamespaces
}
#endif


#endif	/*____Led_OptionsSupport_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
