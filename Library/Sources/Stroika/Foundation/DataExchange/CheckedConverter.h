/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CheckedConverter_h_
#define _Stroika_Foundation_DataExchange_CheckedConverter_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"

#include    "BadFormatException.h"



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
        namespace   DataExchange {



            /**
             *  Throws BadFormatException if data bad. This is handy for 'file' reading, as most of the Stroika type constructors
             *  Assert valid data (e.g. String::FromUTF8 ()).
             */
            template    <typename TO, typename EXTRA_DATA, typename FROM>
            TO  CheckedConverter (const FROM& from, const EXTRA_DATA& extraData = EXTRA_DATA ());


            struct UTF8 {};
            struct ASCII {};

			template    <>
            Characters::String  CheckedConverter<Characters::String, UTF8, string> (const string& from, const UTF8& extraData);
            template    <>
            Characters::String  CheckedConverter<Characters::String, ASCII, string> (const string& from, const ASCII& extraData);

            template    <>
            string  CheckedConverter<string, ASCII, Characters::String> (const Characters::String& from, const ASCII& extraData);


            /**
             *  Note - for floating point ElementType - if the values are 'really close' - they are pinned to the min/max
             *  and not treated as out of range.
             */
            template    <typename   RANGE_TYPE>
            RANGE_TYPE  CheckedConverter_Range (const typename RANGE_TYPE::ElementType& s, const typename RANGE_TYPE::ElementType& e);


            /**
             *  Note - for floating point ElementType - if the values are 'really close' - they are pinned to the min/max
             *  and not treated as out of range.
             */
            template    <typename   RANGE_TYPE>
            typename RANGE_TYPE::ElementType  CheckedConverter_ValueInRange (typename RANGE_TYPE::ElementType val, const RANGE_TYPE& r);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CheckedConverter.inl"

#endif  /*_Stroika_Foundation_DataExchange_CheckedConverter_h_*/
