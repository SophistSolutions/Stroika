/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_BadFormatException_h_
#define _Stroika_Foundation_DataExchangeFormat_BadFormatException_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/Optional.h"
#include    "../Execution/StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /**
             * Use when reading from a structured stream the data is ill-formed
             */
            class   BadFormatException : public Execution::StringException {
            public:
                BadFormatException ();
                BadFormatException (const wstring& details);
                BadFormatException (const wstring& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset);

            public:
                nonvirtual  wstring GetDetails () const;

            public:
                nonvirtual  void    GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const;
            private:
                Memory::Optional<unsigned int>  fLineNumber;
                Memory::Optional<unsigned int>  fColumnNumber;
                Memory::Optional<uint64_t>      fFileOffset;

            private:
                wstring fDetails_;
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

#endif  /*_Stroika_Foundation_DataExchangeFormat_BadFormatException_h_*/
