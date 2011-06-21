/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_URL_h_
#define	_Stroika_Foundation_IO_Network_URL_h_	1

#include	"../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../Configuration/Common.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {

			const	int	kDefaultPort	=	-1;

			// probably should define standard protos here - with named constnats - like http/ftp/https etc
			int		GetDefaultPortForProtocol (const wstring& proto);


			class	URLCracker {
				public:
					URLCracker (const wstring& w = wstring ());

				public:
					nonvirtual	bool	IsSecure () const;
					nonvirtual	wstring	GetURL () const;
					nonvirtual	wstring	GetHostRelPathDir () const;
					nonvirtual	int		GetEffectivePortNumber () const;	// if port# not specified, returns detault given the protocol
					nonvirtual	int		GetPortNumber () const;
					nonvirtual	void	SetPortNumber (int portNum = kDefaultPort);

				public:
					wstring	fProtocol;
					wstring	fHost;
					wstring	fRelPath;
					wstring	fQuery;
					wstring	fFragment;
				private:
					int		fPort;
			};
			bool	operator== (const URLCracker& lhs, const URLCracker& rhs);


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


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Characters/StringUtils.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {

//	class	URLQueryDecoder
	inline	const map<wstring,wstring>&	URLQueryDecoder::GetMap () const
		{
			return fMap;
		}
	inline	wstring		URLQueryDecoder::operator () (const wstring& idx) const
		{
			map<wstring,wstring>::const_iterator	i	=	fMap.find (idx);
			if (i == fMap.end ()) {
				return wstring ();
			}
			else {
				return (*i).second;
			}
		}
	inline	wstring		URLQueryDecoder::operator () (const string& idx) const
		{
			return operator () (Characters::ACPStringToWide (idx));
		}
	inline	bool		URLQueryDecoder::HasField (const wstring& idx) const
		{
			map<wstring,wstring>::const_iterator	i	=	fMap.find (idx);
			return (i != fMap.end ());
		}
	inline	bool		URLQueryDecoder::HasField (const string& idx) const
		{
			return HasField (Characters::ACPStringToWide (idx));
		}
	inline	void	URLQueryDecoder::AddField (const wstring& idx, const wstring& value)
		{
			fMap.insert (map<wstring,wstring>::value_type (idx, value));
		}
	inline	void	URLQueryDecoder::RemoveFieldIfAny (const string& idx)
		{
			RemoveFieldIfAny (Characters::ACPStringToWide (idx));
		}


//	class	URLCracker
	inline	int		URLCracker::GetEffectivePortNumber () const
		{
			if (fPort == kDefaultPort) {
				return GetDefaultPortForProtocol (fProtocol);
			}
			else {
				return fPort;
			}
		}
	inline	int		URLCracker::GetPortNumber () const
		{
			return fPort;
		}
	inline	void	URLCracker::SetPortNumber (int portNum)
		{
			fPort = portNum;
		}



//	class	LabeledURL
	inline	LabeledURL::LabeledURL (const wstring& url, const wstring& label)
		: fURL (url)
		, fLabel (label)
		{
		}

			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_URL_h_*/
