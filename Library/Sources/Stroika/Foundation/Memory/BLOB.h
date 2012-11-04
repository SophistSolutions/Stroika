/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <map>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Containers/Common.h"
#include    "../Memory/Common.h"



/*
 * TODO:
 *
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            using   namespace   std;

            using   namespace   Stroika::Foundation;
            using   namespace   Stroika::Foundation::Configuration;
            using   namespace   Stroika::Foundation::Memory;

            
			/**
			 */
			class   BLOB {
            public:
                BLOB ();
                BLOB (const vector<Byte>& data);
                BLOB (const Byte* start, const Byte* end);

            public:
                nonvirtual  bool    empty () const;
            public:
                nonvirtual  void    clear ();

            public:
                /**
                 * Convert BLOB losslessly into a standard C++ type (right now just <vector<Byte>>
                 *  supported)
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                template    <typename   T>
                nonvirtual  void   As (T* into) const;
            public:
                //nonvirtual  operator const vector<Byte>& () const;
            public:
                nonvirtual  const Byte* begin () const;
                nonvirtual  const Byte* end () const;
            public:
                nonvirtual  size_t      GetSize () const;
                nonvirtual  size_t      size () const;
                nonvirtual  size_t      length () const;

            private:
                struct  Rep {
                    vector<Byte>    fData;
                };
                shared_ptr<Rep> fRep;
            };
            bool    operator< (const BLOB& lhs, const BLOB& rhs);
            bool    operator> (const BLOB& lhs, const BLOB& rhs);
            bool    operator== (const BLOB& lhs, const BLOB& rhs);

            template    <>
            void    BLOB::As (vector<Byte>* into) const;
            template    <>
            vector<Byte> BLOB::As () const;

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_BLOB_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BLOB.inl"
