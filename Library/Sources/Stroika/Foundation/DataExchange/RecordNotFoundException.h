/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_RecordNotFoundException_h_
#define _Stroika_Foundation_DataExchange_RecordNotFoundException_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/String.h"
#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::DataExchange {

    /**
     */
    class RecordNotFoundException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        /**
         */
        RecordNotFoundException (const optional<Characters::String>& whatRecord = {});

        static const RecordNotFoundException kThe;
    };
    inline const RecordNotFoundException RecordNotFoundException::kThe;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RecordNotFoundException.inl"

#endif /*_Stroika_Foundation_DataExchange_RecordNotFoundException_h_*/
