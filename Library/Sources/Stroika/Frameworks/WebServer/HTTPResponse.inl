/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Frameworks_WebServer_HTTPResponse_inl_
#define	_Stroika_Frameworks_WebServer_HTTPResponse_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Foundation/Containers/Common.h"

namespace	Stroika {
	namespace	Frameworks	{
		namespace	WebServer {


		//	class	HTTPResponse
			inline	HTTPResponse::State	HTTPResponse::GetState () const
				{
					return fState_;
				}
			inline	InternetMediaType	HTTPResponse::GetContentType () const
				{
					return fContentType_;
				}
			inline	void	HTTPResponse::write (const wchar_t* e)
				{
					RequireNotNull (e);
					write (e, e + ::wcslen (e));
				}
			inline	void	HTTPResponse::write (const wstring& e)
				{
					if (not e.empty ()) {
						write (Containers::Start (e), Containers::End (e));
					}
				}
			inline	void	HTTPResponse::writeln (const wchar_t* e)
				{
					RequireNotNull (e);
					const	wchar_t	kEOL[]	=	L"\r\n";
					write (e, e + ::wcslen (e));
					write (StartOfArray (kEOL), EndOfArray (kEOL));
				}
			inline	void	HTTPResponse::writeln (const wstring& e)
				{
					const	wchar_t	kEOL[]	=	L"\r\n";
					if (not e.empty ()) {
						write (Containers::Start (e), Containers::End (e));
					}
					write (StartOfArray (kEOL), EndOfArray (kEOL));
				}
			inline	void	HTTPResponse::clear ()
				{
					fBytes_.clear ();
				}
			inline	bool	HTTPResponse::empty () const
				{
					return fBytes_.empty ();
				}
			inline	const vector<Byte>&	HTTPResponse::GetBytes () const
				{
					return fBytes_;
				}

		}
	}
}
#endif	/*_Stroika_Frameworks_WebServer_HTTPResponse_inl_*/
