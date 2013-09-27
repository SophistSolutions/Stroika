/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_CheckedConverter_h_
#define _Stroika_Foundation_DataExchangeFormat_CheckedConverter_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"

#include	"BadFormatException.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - VERY PRELIMINARY DRAFT
 *
 *
 *  \em Design Note:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {



            /**
			 *	Throws BadFormatException if data bad. This is handy for 'file' reading, as most of the Stroika type constructors
			 *	Assert valid data (e.g. String::FromUTF8 ()).
             */
			template	<typename FROM, typename TO, typename EXTRA_DATA>
			TO	CheckedConverter (const FROM& from, const EXTRA_DATA& extraData = EXTRA_DATA ());


			struct UTF8 {};
			template	<>
			Characters::String	CheckedConverter<string,Characters::String,UTF8> (const string& from, const UTF8& extraData);

			struct ASCII {};
			template	<>
			string	CheckedConverter<Characters::String,string,ASCII> (const Characters::String& from, const ASCII& extraData);


			template	<typename	RANGE_TYPE>
			RANGE_TYPE	CheckedConverter_Range (typename const RANGE_TYPE::ElementType& s, typename const RANGE_TYPE::ElementType& e);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CheckedConverter.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_CheckedConverter_h_*/
