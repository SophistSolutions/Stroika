/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ******************** DataExchange::RecordNotFoundException *********************
     ********************************************************************************
     */
    inline RecordNotFoundException::RecordNotFoundException (const optional<Characters::String>& whatRecord)
        : inherited{whatRecord == nullopt ? "Record Not Found"sv : ("Record (" + *whatRecord + ") Not Found")}
    {
    }

}
