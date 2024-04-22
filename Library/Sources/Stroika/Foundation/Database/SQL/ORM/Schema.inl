/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::Database::SQL::ORM {

    /*
     ********************************************************************************
     ***************************** Schema::Field::Field *****************************
     ********************************************************************************
     */
    inline String Schema::Field::GetVariantValueFieldName () const
    {
        return Memory::NullCoalesce (fVariantValueName, fName);
    }

}
