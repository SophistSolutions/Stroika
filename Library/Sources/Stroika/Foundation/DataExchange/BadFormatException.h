/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_BadFormatException_h_
#define _Stroika_Foundation_DataExchange_BadFormatException_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/Optional.h"
#include    "../Execution/StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /**
             * Use when reading from a structured stream the data is ill-formed
             */
            class   BadFormatException : public Execution::StringException {
            private:
                using   inherited   =   Execution::StringException;

            public:
                BadFormatException ();
                BadFormatException (const Characters::String& details);
                BadFormatException (const Characters::String& details, const Memory::Optional<unsigned int>& lineNumber, const Memory::Optional<unsigned int>& columnNumber, const Memory::Optional<uint64_t>& fileOffset);

            public:
                nonvirtual  Characters::String GetDetails () const;

            public:
                nonvirtual  void    GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const;

            private:
                Memory::Optional<unsigned int>  fLineNumber_;
                Memory::Optional<unsigned int>  fColumnNumber_;
                Memory::Optional<uint64_t>      fFileOffset_;

            private:
                Characters::String fDetails_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BadFormatException.inl"

#endif  /*_Stroika_Foundation_DataExchange_BadFormatException_h_*/
