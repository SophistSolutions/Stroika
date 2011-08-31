/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_
#define	_Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {

			inline	wstring	DataExchangeFormat::BadFormatException::GetDetails () const
				{
					return fDetails_;
				}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_*/
