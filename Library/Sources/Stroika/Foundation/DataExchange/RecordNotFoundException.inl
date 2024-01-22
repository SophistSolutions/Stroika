/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_RecordNotFoundException_inl_
#define _Stroika_Foundation_DataExchange_RecordNotFoundException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ******************** DataExchange::RecordNotFoundException *********************
     ********************************************************************************
     */
    inline RecordNotFoundException::RecordNotFoundException (const optional<Characters::String>& whatRecord = {})
        : inherited{whatRecord == nullopt ? "Record Not Found"sv : ("Record (" + *whatRecord + ") Not Found")}
    {
    }

}

#endif /*_Stroika_Foundation_DataExchange_RecordNotFoundException_inl_*/
