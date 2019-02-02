/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileFormatException_h_
#define _Stroika_Foundation_IO_FileFormatException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Execution/Exception.h"

namespace Stroika::Foundation::IO {

    using Characters::String;

    /**
     */
    class FileFormatException : public Execution::Exception<> {
    private:
        using inherited = Execution::Exception<>;

    public:
        FileFormatException (const String& fileName);

    public:
        nonvirtual String GetFileName () const;

    private:
        String fFileName_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileFormatException.inl"

#endif /*_Stroika_Foundation_IO_FileFormatException_h_*/
