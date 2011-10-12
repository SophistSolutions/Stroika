/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPResponse_h_
#define	_Stroika_Framework_WebServer_HTTPResponse_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include	"../../Foundation/Memory/SharedPtr.h"



/*
 * TODO:
 *		(o)		REDO THE HTTPRESPONSE USING A BINARY OUTPUT STREAM.
 *				INTERNALLY - based on code page - construct a TEXTOUTPUTSTREAM wrapping that binary output stream!!!
 *
 *		(o)		Have output CODEPAGE param - used for all unincode-string writes. Create Stream wrapper than does the downshuft
 *				to right codepage.
 */

namespace	Stroika {	
	namespace	Frameworks {
		namespace	WebServer {

			using	namespace	Stroika::Foundation;
			using	Characters::String;
			using	DataExchangeFormat::InternetMediaType;

			struct	HTTPResponse {
				public:
					HTTPResponse (const InternetMediaType& ct);

				public:
					nonvirtual	InternetMediaType	GetContentType () const;

				public:
					nonvirtual	void	write (const Byte* start, const Byte* end);
					nonvirtual	void	write (const wchar_t* e);
					nonvirtual	void	write (const wchar_t* s, const wchar_t* e);
					nonvirtual	void	write (const wstring& e);
					nonvirtual	void	printf (const wchar_t* format, ...);
					nonvirtual	void	writeln (const wchar_t* e);
					nonvirtual	void	writeln (const wstring& e);

				public:
					virtual		void	clear ();
					nonvirtual	bool	empty () const;


// REDO USING BINARY STREAM (CTOR SHOULD TAKE BINARY STREAM CTOR)
				public:
					nonvirtual	const vector<Byte>&	GetBytes () const;

				private:
					InternetMediaType	fContentType_;
				protected:
					vector<Byte>		_fBytes;
			};

		}
	}
}
#endif	/*_Stroika_Framework_WebServer_HTTPResponse_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"HTTPResponse.inl"
