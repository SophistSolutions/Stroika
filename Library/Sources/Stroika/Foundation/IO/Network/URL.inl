/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_URL_inl_
#define	_Stroika_Foundation_IO_Network_URL_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Characters/CodePage.h"

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
						return operator () (Characters::UTF8StringToWide (idx));
					}
				inline	bool		URLQueryDecoder::HasField (const wstring& idx) const
					{
						map<wstring,wstring>::const_iterator	i	=	fMap.find (idx);
						return (i != fMap.end ());
					}
				inline	bool		URLQueryDecoder::HasField (const string& idx) const
					{
						return HasField (Characters::UTF8StringToWide (idx));
					}
				inline	void	URLQueryDecoder::AddField (const wstring& idx, const wstring& value)
					{
						fMap.insert (map<wstring,wstring>::value_type (idx, value));
					}
				inline	void	URLQueryDecoder::RemoveFieldIfAny (const string& idx)
					{
						RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
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
#endif	/*_Stroika_Foundation_IO_Network_URL_inl_*/
