/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Transfer_Client_h_
#define	_Stroika_Foundation_IO_Network_Transfer_Client_h_	1

#include	"../../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../../Configuration/Common.h"


/*
 * TODO:
 *		(o)		Probably princiapply a wrapper on CURL - but also support wrapper on WinHTTP
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {



					// SUPER PRIMITIVE fetcher - must do MUCH better, much more elaborate!
					vector<Byte>	Fetch (const wstring& url);


				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Client.inl"
