/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_URL_h_
#define	_Stroika_Foundation_IO_Network_URL_h_	1

#include	"../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../Characters/String.h"
#include	"../../Configuration/Common.h"


/*
 * TODO:
 *		o	Redo using Stroika String class
 *		o	Add overload for URL with map<> for query.
 *		o	Add PREDEFINED namespace and list predefined protocols
 *				HTTP
 *				HTTPS
 *				FTP
 *				FILE
 *			(only do with new stroika string class so we can use low-cost constants)
 */



namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {

				const	int	kDefaultPort	=	-1;

				// probably should define standard protos here - with named constnats - like http/ftp/https etc
				int		GetDefaultPortForProtocol (const wstring& proto);


				class	URL {
					public:
						URL ();
						explicit URL (const wstring& fullURL);
						explicit URL (const wstring& protocol, const wstring& host, int portNumber = kDefaultPort, const wstring& relPath = wstring (), const wstring& query = wstring (), const wstring& fragment = wstring ());
						explicit URL (const wstring& protocol, const wstring& host, const wstring& relPath, const wstring& query = wstring (), const wstring& fragment = wstring ());

					public:
						// For example - the arg to GET in HTTP request - a regular URL but the part after the hostname
						static	URL	ParseHostRelativeURL (const wstring& w);

					public:
						nonvirtual	bool	IsSecure () const;
						nonvirtual	wstring	GetURL () const;
						nonvirtual	wstring	GetHostRelPathDir () const;
						nonvirtual	int		GetEffectivePortNumber () const;	// if port# not specified, returns detault given the protocol
						nonvirtual	int		GetPortNumber () const;
						nonvirtual	void	SetPortNumber (int portNum = kDefaultPort);

					public:
						nonvirtual	void	clear ();
						nonvirtual	bool	empty () const;

					public:
						wstring	fProtocol;
						wstring	fHost;
						wstring	fRelPath;
						wstring	fQuery;
						wstring	fFragment;
					private:
						int		fPort;
				};
				bool	operator== (const URL& lhs, const URL& rhs);


				// See http://tools.ietf.org/html/rfc3986
				// This doesn't encode an entire URL, just a particular field
				string	EncodeURLQueryStringField (const wstring& s);

				class	URLQueryDecoder {
					public:
						URLQueryDecoder (const string& query);
						URLQueryDecoder (const wstring& query);

					public:
						nonvirtual	const map<wstring,wstring>&	GetMap () const;

					public:
						nonvirtual	wstring	operator () (const string& idx) const;
						nonvirtual	wstring	operator () (const wstring& idx) const;

						nonvirtual	bool	HasField (const string& idx) const;
						nonvirtual	bool	HasField (const wstring& idx) const;

						nonvirtual	void	AddField (const wstring& idx, const wstring& value);

						nonvirtual	void	RemoveFieldIfAny (const string& idx);
						nonvirtual	void	RemoveFieldIfAny (const wstring& idx);

						// Return wide string, but all ascii characters
						// http://tools.ietf.org/html/rfc3986
						nonvirtual	wstring	ComputeQueryString () const;

					private:
						map<wstring,wstring>	fMap;
				};



				class	LabeledURL {
					public:
						LabeledURL (const wstring& url, const wstring& label = wstring ());

					public:
						wstring	fURL;
						wstring	fLabel;
				};

			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_URL_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"URL.inl"
