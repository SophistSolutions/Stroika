/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Locale_h_
#define _Stroika_Foundation_Configuration_Locale_h_  1

#include    "../StroikaPreComp.h"

#include    <locale>

#include    "../Configuration/Common.h"


/**
 *  \file
 *
 * TODO:
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


			/**
			 *	In C++, the default locale in "C", not the one inherited from the OS.
			 *	Its not hard to get/set the one from the OS, but I've found it not well documented,
			 *	so this is intended to make it a little easier/more readable.
			 */
			std::locale	GetPlatformDefaultLocale ();


			/**
			 *	\brief	Set the operating system locale into the current C++ locale used by locale functions (and most locale-dependent stroika funcitons).
			 *
			 *	In C++, the default locale in "C", not the one inherited from the OS.
			 *	Its not hard to get/set the one from the OS, but I've found it not well documented,
			 *	so this is intended to make it a little easier/more readable.
			 *
			 */
			void	UsePlatformDefaultLocaleAsDefaultLocale ();


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Locale_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Locale.inl"
