/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Cryptography_Base64_h_
#define	_Stroika_Foundation_Cryptography_Base64_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../Configuration/Basics.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Cryptography {

			vector<Byte>	DecodeBase64 (const string& s);
			
			enum	LineBreak { eLF_LB, eCRLF_LB, eAuto_LB = eCRLF_LB };
			string			EncodeBase64 (const vector<Byte>& b, LineBreak lb = eAuto_LB);

		}
	}
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Base64.inl"
#endif	/*_Stroika_Foundation_Cryptography_Base64_h_*/
