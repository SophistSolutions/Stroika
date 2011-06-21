/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BadFormatException_inl_
#define	_Stroika_Foundation_Streams_BadFormatException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {

			inline	wstring	Streams::BadFormatException::GetDetails () const
				{
					return fDetails_;
				}
	}
}
#endif	/*_Stroika_Foundation_Streams_BadFormatException_inl_*/
