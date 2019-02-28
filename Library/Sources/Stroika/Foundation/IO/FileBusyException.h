/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileBusyException_h_
#define _Stroika_Foundation_IO_FileBusyException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::IO {

    using Characters::String;

    /**
     */
    class FileBusyException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        FileBusyException (const String& fileName = String ());

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
#include "FileBusyException.inl"

#endif /*_Stroika_Foundation_IO_FileBusyException_h_*/
