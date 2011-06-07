/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Configbasics_inl
#define	_Configbasics_inl	1


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
#endif	/*_Configbasics_inl*/



