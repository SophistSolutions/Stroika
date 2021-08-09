
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_Schema_inl_
#define _Stroika_Foundation_Database_SQL_ORM_Schema_inl_ 1

#include "../../../Memory/Optional.h"

namespace Stroika::Foundation::Database::SQL::ORM {

    /*
     ********************************************************************************
     ***************************** Schema::Field::Field *****************************
     ********************************************************************************
     */
    inline String Schema::Field::GetVariantValueFieldName () const
    {
        return Memory::NullCoalesce (fVariantValueFieldName, fName);
    }

}

#endif /*_Stroika_Foundation_Database_SQL_ORM_Schema_inl_*/
