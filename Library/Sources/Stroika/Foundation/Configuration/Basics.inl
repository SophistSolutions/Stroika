/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Basics_inl_
#define	_Stroika_Foundation_Configuration_Basics_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Configuration {

			template	<typename	ENUM>
				inline	ENUM	Inc (typename ENUM e)
					{
						return static_cast<ENUM> (e + 1);
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Configuration_Basics_inl_*/



