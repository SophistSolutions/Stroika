/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtrBase_inl_
#define	_Stroika_Foundation_Memory_SharedPtrBase_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Execution/AtomicOperations.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {


			namespace	SharedPtrNS {
				namespace	Private {
					struct	SimpleSharedPtrBase : SharedPtrBase {
						public:
							virtual	void	DO_DELETE_REF_CNT ();

						public:
							DECLARE_USE_BLOCK_ALLOCATION(SimpleSharedPtrBase);
					};
				}
			}






		//	class	SharedPtrBase
			inline	SharedPtrBase::SharedPtrBase ():
				fCount_DONT_ACCESS (0)
				{
				}
			inline	SharedPtrBase::~SharedPtrBase ()
				{
				}


		}



	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtrBase_inl_*/
