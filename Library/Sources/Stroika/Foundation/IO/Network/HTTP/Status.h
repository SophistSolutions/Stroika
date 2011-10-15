/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_HTTP_Status_h_
#define	_Stroika_Foundation_IO_Network_HTTP_Status_h_	1

#include	"../../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../../Configuration/Common.h"


/*
 * TODO:
 *		(o)		
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	HTTP {



					typedef	unsigned int	Status;
					namespace	StatusCodes {
						const	Status	kOK					=	200;	
						const	Status	kMovedPermanently	=	301;	
						const	Status	kBadRequest			=	400;	
						const	Status	kUnauthorized		=	401;	
						const	Status	kNotFound			=	404;
						const	Status	kInternalError		=	500;
						const	Status	kServiceUnavailable	=	503;	//	indicates that the server is temporarily unavailable, usually due to high load or maintenance.
					}



				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_HTTP_Status_h_*/
