/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPResponse_h_
#define	_Stroika_Framework_WebServer_HTTPResponse_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<map>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include	"../../Foundation/Memory/SharedPtr.h"
#include	"../../Foundation/IO/Network/HTTP/Status.h"
#include	"../../Foundation/Streams/BinaryOutputStream.h"



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
			using	namespace	Stroika::Foundation::IO::Network::HTTP;
			
			using	Characters::String;
			using	DataExchangeFormat::InternetMediaType;


			/*
			 * As of yet to specify FLUSH semantics - when we flush... Probably need options (ctor/config)
			 */
			struct	HTTPResponse {
				public:
					HTTPResponse (Streams::BinaryOutputStream& outStream, const InternetMediaType& ct);

				public:
					nonvirtual	InternetMediaType	GetContentType () const;
					nonvirtual	void				SetContentType (const InternetMediaType& contentType);


// ADD REPSONSE STATEz? GUESS at states we want...
public: enum State { eInProgress, eInProgressHeaderGone, eCompleted };

				public:
					/*
					 * This begins sending the parts of the message which have already been accumulated to the client.
					 * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
					 * if we are NOT in automatic-include-Content-Length mode (NYI).
					 *
					 * This does NOT End the repsonse, and it CAN be called arbitrarily many times (as long as the response hasn't been ended).
					 */
					nonvirtual	void	Flush ();

					/*
					 * This signifies that the given request has been handled. Its illegal to write to this request object again, or modify
					 * any aspect of it.
					 */
					nonvirtual	void	End ();

					nonvirtual	void	Redirect (const wstring& url);

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

				public:
					nonvirtual	Status	GetStatus () const {return fStatus; }
					nonvirtual	void	SetStatus (Status newStatus) { fStatus = newStatus; }

				private:
					Status	fStatus;

				public:
					// LEGAL to call anytime before FLush. Illegal to call after flush. Can call to replace existing headers values -
					nonvirtual	void	AddHeader (String headerName, String value);
					nonvirtual	void	ClearHeader ();
					nonvirtual	void	ClearHeader (String headerName);

				private:
					Streams::BinaryOutputStream&	fOutStream_;
					map<String,String>				fHeaders_;
					bool							fAnyWritesDone_;
					InternetMediaType				fContentType_;
					vector<Byte>					fBytes_;
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
