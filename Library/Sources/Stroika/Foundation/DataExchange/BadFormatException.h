/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_BadFormatException_h_
#define _Stroika_Foundation_DataExchange_BadFormatException_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::DataExchange {

    /**
     * Use when reading from a structured stream the data is ill-formed
     */
    class BadFormatException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        /**
         */
        BadFormatException ();
        BadFormatException (const Characters::String& details);
        BadFormatException (const Characters::String& details, const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset);

    public:
        /**
         */
        nonvirtual Characters::String GetDetails () const;

    public:
        /**
         */
        nonvirtual void GetPositionInfo (optional<unsigned int>* lineNum, optional<unsigned int>* colNumber, optional<uint64_t>* fileOffset) const;

    public:
        /**
         */
        static const BadFormatException kThe;

    private:
        optional<unsigned int> fLineNumber_;
        optional<unsigned int> fColumnNumber_;
        optional<uint64_t>     fFileOffset_;

    private:
        Characters::String fDetails_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BadFormatException.inl"

#endif /*_Stroika_Foundation_DataExchange_BadFormatException_h_*/
