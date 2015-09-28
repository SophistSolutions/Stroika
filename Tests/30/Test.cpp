/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/DataExchange/Atom.h"
#include    "Stroika/Foundation/DataExchange/OptionsFile.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include    "Stroika/Foundation/Streams/TextReader.h"


#if     qHasFeature_LZMA
#include    "Stroika/Foundation/DataExchange/7z/ArchiveReader.h"
#endif

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;






namespace   {
    void    Test1_Atom_()
    {
        {
            Atom<> a = L"d";
            Atom<> b = L"d";
            VerifyTestResult (a == b);
            VerifyTestResult (a.GetPrintName () == L"d");
            VerifyTestResult (a.As<String> () == L"d");
            VerifyTestResult (a.As<wstring> () == L"d");
            VerifyTestResult (not a.empty ());
        }
        {
            VerifyTestResult (Atom<> ().empty ());
        }
        {
            Atom<> a = L"d";
            Atom<> b = L"e";
            VerifyTestResult (a != b);
            VerifyTestResult (not a.empty ());
            Atom<>  c   =   a;
            VerifyTestResult (c == a);
        }
    }
}


namespace {
    void    Test2_OptionsFile_ ()
    {
        struct  MyData_ {
            bool                fEnabled = false;
            DateTime            fLastSynchronizedAt;
        };
        OptionsFile of {
            L"MyModule",
            [] () -> ObjectVariantMapper {
                ObjectVariantMapper mapper;
                mapper.AddClass<MyData_> ({
                    ObjectVariantMapper_StructureFieldInfo_Construction_Helper (MyData_, fEnabled, L"Enabled"),
                    ObjectVariantMapper_StructureFieldInfo_Construction_Helper (MyData_, fLastSynchronizedAt, L"Last-Synchronized-At"),
                });
                return mapper;
            } (),
            OptionsFile::kDefaultUpgrader,
            [] (const String & moduleName, const String & fileSuffix) -> String {
                return  IO::FileSystem::WellKnownLocations::GetTemporary () + moduleName + fileSuffix;
            }
        };
        MyData_ m = of.Read<MyData_> (MyData_ ());  // will return default values if file not present
        of.Write (m);                               // test writing
    }
}







namespace {
    namespace Test3_7zArchive_ {
        namespace Private_ {
            void    ReadHardwired7zFile_ ()
            {
                // od sample_zip.7z -t x1 -w32 | sed 's/ /,0x/g'
                static
#if     qCompilerAndStdLib_largeconstexprarray_Buggy
                const
#else
                constexpr
#endif
                Byte    ksample_zip_7z_[] = {
                    0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c, 0x00, 0x03, 0xad, 0xd3, 0x3a, 0xb4, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xd9, 0x4d, 0x4c
                    , 0x00, 0x2d, 0x8c, 0x02, 0x26, 0x10, 0xd9, 0x9e, 0x69, 0xe9, 0xf2, 0xc8, 0xac, 0x29, 0x7c, 0xec, 0x89, 0x1d, 0x9e, 0x5d, 0xaf, 0x2f, 0x82, 0x5d, 0x47, 0x3b, 0x79, 0x78, 0x47, 0x44, 0xf8, 0x55
                    , 0xee, 0xb7, 0xc7, 0x3d, 0x8b, 0x6a, 0x2f, 0xc5, 0x53, 0x4d, 0x57, 0x4a, 0x1a, 0xb0, 0x16, 0x86, 0x3b, 0xb5, 0x33, 0xa9, 0x9c, 0xcc, 0x0a, 0xf0, 0xc6, 0xb8, 0xe7, 0x69, 0xf8, 0x6a, 0xc7, 0x90
                    , 0x20, 0xf1, 0xe0, 0x7c, 0x99, 0x2c, 0x89, 0xc6, 0x8c, 0x8b, 0x97, 0x02, 0x6e, 0x46, 0xc4, 0x4c, 0x53, 0x48, 0x09, 0x0e, 0x66, 0x88, 0x0e, 0xfc, 0x76, 0x15, 0xe6, 0xbe, 0x73, 0x62, 0x74, 0x8a
                    , 0x82, 0x4f, 0x0e, 0xdf, 0x7c, 0xde, 0x7c, 0x4d, 0xaf, 0x15, 0x71, 0xbc, 0x98, 0x74, 0x9c, 0x85, 0x38, 0xa6, 0x69, 0x0d, 0xe3, 0xa8, 0x98, 0xfb, 0x70, 0xf4, 0x57, 0x96, 0x6c, 0x99, 0x25, 0x44
                    , 0x2e, 0x58, 0x66, 0x89, 0x0c, 0x94, 0x66, 0x96, 0x74, 0x64, 0x12, 0x82, 0xc7, 0x32, 0xec, 0x30, 0xd2, 0xec, 0x4b, 0xfa, 0xc6, 0xbc, 0x86, 0x4e, 0xf3, 0x58, 0xa7, 0x62, 0xb6, 0xbe, 0xc6, 0x80
                    , 0x61, 0x1e, 0x38, 0xc5, 0xb2, 0xd0, 0xcb, 0x3f, 0x46, 0xff, 0xf2, 0x5a, 0xd8, 0xd2, 0xb2, 0xa8, 0xf5, 0xdc, 0xb6, 0xea, 0xfe, 0x89, 0xfc, 0xa8, 0x98, 0x14, 0x58, 0x72, 0xf1, 0x41, 0x77, 0x8b
                    , 0xec, 0xb8, 0x71, 0x4c, 0xf0, 0xd3, 0xed, 0x99, 0x70, 0x04, 0xbe, 0xcf, 0x3b, 0x46, 0x22, 0x60, 0x41, 0x65, 0xf9, 0x24, 0x60, 0x82, 0x6d, 0x2f, 0xc6, 0x6e, 0x18, 0xf1, 0x23, 0xd2, 0x3f, 0x02
                    , 0x89, 0x9e, 0x91, 0x98, 0x7a, 0x11, 0x5f, 0x9d, 0xf9, 0x08, 0xd6, 0xfd, 0xee, 0xaa, 0x57, 0xb9, 0x80, 0x7a, 0xde, 0x48, 0x56, 0xd8, 0x6a, 0x41, 0x04, 0x39, 0xc7, 0x27, 0xb0, 0xcc, 0xe4, 0xcc
                    , 0x0a, 0x75, 0x64, 0x09, 0xdf, 0x04, 0x86, 0x75, 0x36, 0x2e, 0xbe, 0x26, 0xac, 0x20, 0x20, 0x94, 0x37, 0xe1, 0xd8, 0x01, 0x7c, 0x70, 0x34, 0x6b, 0x6d, 0xdd, 0x3f, 0xeb, 0x29, 0x41, 0x85, 0xb3
                    , 0x5d, 0xfa, 0x17, 0xad, 0xba, 0xf1, 0x83, 0x8a, 0x59, 0x41, 0x94, 0xb4, 0xed, 0x45, 0xdd, 0x1d, 0xd8, 0x54, 0xc4, 0xd0, 0x2a, 0xf3, 0x06, 0x34, 0xec, 0xa8, 0x82, 0x00, 0xf4, 0x6b, 0xa0, 0x67
                    , 0xf6, 0x07, 0x39, 0xb7, 0x0a, 0xf6, 0x4a, 0xa6, 0x13, 0x71, 0x28, 0x5c, 0x50, 0x2a, 0xe9, 0x3f, 0xf3, 0x95, 0xa2, 0xdf, 0x86, 0x76, 0x5d, 0xf2, 0xdd, 0xea, 0x20, 0x5d, 0xed, 0x21, 0xcd, 0xb6
                    , 0xa5, 0x7d, 0x22, 0x92, 0x76, 0x5e, 0x06, 0x50, 0x94, 0x2c, 0xf5, 0xce, 0xff, 0x09, 0x68, 0xa0, 0xe5, 0xaf, 0x11, 0x19, 0x1b, 0x47, 0x6d, 0x35, 0x5a, 0xc6, 0x99, 0x5d, 0xbf, 0x98, 0xe9, 0x12
                    , 0xbe, 0x0a, 0xda, 0x6d, 0x5e, 0x32, 0x7d, 0xf2, 0x61, 0x83, 0x03, 0xaf, 0xa9, 0xe1, 0x71, 0x7b, 0x95, 0xa1, 0x34, 0xf8, 0xd6, 0xa7, 0x81, 0x5b, 0x7b, 0xe5, 0x9b, 0xda, 0x72, 0x01, 0xea, 0x93
                    , 0x47, 0x95, 0xe6, 0xf0, 0xd7, 0xf5, 0x03, 0xa5, 0x12, 0x81, 0xec, 0xc4, 0x73, 0x53, 0xdf, 0xaa, 0xc0, 0x44, 0x31, 0x83, 0x22, 0x91, 0x48, 0x23, 0x67, 0x74, 0x04, 0x93, 0xb3, 0x20, 0x6d, 0x66
                    , 0x7a, 0xd5, 0x33, 0xac, 0x99, 0x5e, 0xa4, 0x8c, 0xf4, 0x32, 0x38, 0x6f, 0xf2, 0x3a, 0x87, 0x4e, 0x93, 0x89, 0x31, 0xc0, 0xe0, 0xdd, 0xf4, 0x27, 0xec, 0x54, 0xf4, 0x4a, 0x91, 0x49, 0x28, 0x22
                    , 0x71, 0x7b, 0x99, 0xd7, 0xd8, 0x55, 0xcc, 0xd3, 0xbb, 0x1f, 0x03, 0x61, 0xcd, 0xf3, 0x25, 0xdf, 0xbe, 0x65, 0x1b, 0x01, 0xe4, 0x3d, 0x6d, 0xdf, 0x43, 0xac, 0xcf, 0xfc, 0x76, 0xb0, 0x73, 0x78
                    , 0x61, 0x7c, 0x4c, 0x3d, 0x91, 0xbb, 0x3e, 0x68, 0x93, 0xc5, 0x01, 0x61, 0x4b, 0xb8, 0x71, 0xd1, 0x74, 0x96, 0xf3, 0x86, 0x37, 0x5f, 0x82, 0x5d, 0x79, 0xd0, 0xb3, 0xd4, 0xf7, 0x12, 0x78, 0x6a
                    , 0x65, 0x7e, 0xe6, 0x2f, 0xd5, 0xb7, 0xa3, 0x2d, 0xd7, 0xb6, 0x81, 0xcc, 0xb5, 0xd4, 0x5a, 0xf2, 0x4e, 0x84, 0xcd, 0xd4, 0x78, 0x8e, 0x4e, 0xed, 0x0e, 0x99, 0x1b, 0x14, 0xd3, 0x03, 0x6c, 0x1c
                    , 0x88, 0xd0, 0x5c, 0xe8, 0xbc, 0x6c, 0x8c, 0x51, 0xfb, 0x5e, 0xcb, 0xa3, 0x87, 0xad, 0x30, 0x54, 0x3d, 0xed, 0x25, 0xa4, 0xa4, 0x9f, 0xfa, 0x4f, 0x4c, 0x83, 0x84, 0xb6, 0x73, 0xe6, 0x84, 0xa0
                    , 0x33, 0xd0, 0x72, 0x4b, 0xaf, 0xbb, 0x25, 0xed, 0x0d, 0xa3, 0xef, 0xdd, 0xc0, 0x0b, 0x2b, 0x64, 0x58, 0xb3, 0x80, 0x48, 0xaa, 0x95, 0x38, 0x3e, 0x25, 0x40, 0x8b, 0xd5, 0x61, 0xd0, 0xf0, 0x36
                    , 0xca, 0xde, 0x32, 0x55, 0xeb, 0x16, 0xc0, 0x3f, 0xb0, 0xb7, 0x9c, 0x34, 0x36, 0x75, 0x52, 0x47, 0x53, 0xb7, 0x63, 0x2d, 0x6b, 0x5b, 0x17, 0xd5, 0xe5, 0x3b, 0xa4, 0xd0, 0xf0, 0x92, 0x3c, 0xb6
                    , 0x48, 0x9a, 0xd1, 0x24, 0x0b, 0x5f, 0x96, 0x85, 0x9c, 0x60, 0x1b, 0x8f, 0x1e, 0x4d, 0xa0, 0x15, 0x8f, 0x81, 0x2d, 0xcc, 0x86, 0x5e, 0xba, 0xba, 0xba, 0x8f, 0xd7, 0xf7, 0x41, 0x38, 0x38, 0x20
                    , 0xe5, 0xa6, 0x4e, 0x73, 0xea, 0xc5, 0x72, 0xcf, 0x5e, 0x6d, 0xae, 0x5c, 0x29, 0x6e, 0x3a, 0x4c, 0xcb, 0x90, 0x2c, 0xc4, 0xa7, 0x93, 0xd4, 0x86, 0xe8, 0xc3, 0xb6, 0x0d, 0xc7, 0xaa, 0x40, 0x3f
                    , 0x00, 0xc3, 0xa5, 0x47, 0xd5, 0xc7, 0xb1, 0x87, 0xf4, 0x05, 0x3b, 0x6d, 0x89, 0x8f, 0x8b, 0x30, 0x05, 0x11, 0x56, 0x50, 0xab, 0x9a, 0x0e, 0x7e, 0xa8, 0xf9, 0x85, 0x2c, 0xb6, 0x27, 0x26, 0xe5
                    , 0xb4, 0xe9, 0xab, 0x96, 0x87, 0x72, 0xbd, 0x09, 0x93, 0x3d, 0x86, 0x65, 0x6c, 0x72, 0xc3, 0x87, 0xf2, 0x1b, 0xe8, 0x17, 0x59, 0x17, 0xe5, 0xad, 0x22, 0x9d, 0x6e, 0x02, 0x7c, 0x70, 0xe9, 0xf2
                    , 0x00, 0xc5, 0xac, 0xd7, 0x5d, 0x92, 0x8e, 0x15, 0x90, 0xe0, 0x29, 0xb5, 0xfd, 0x4a, 0x92, 0x2f, 0xfc, 0xa7, 0xdc, 0x55, 0xff, 0x60, 0xab, 0xf3, 0x8a, 0x61, 0x98, 0x80, 0x68, 0x7b, 0xfc, 0xb0
                    , 0x08, 0x07, 0x12, 0xbb, 0x32, 0x3f, 0xf7, 0x8d, 0x7a, 0xeb, 0x2b, 0x3b, 0x79, 0xc8, 0xe9, 0x14, 0xe8, 0xc5, 0x7e, 0x83, 0xf6, 0xb9, 0x95, 0xfa, 0xad, 0x3c, 0x9d, 0xf5, 0xec, 0xe8, 0xd9, 0x13
                    , 0x8f, 0x32, 0xc2, 0xbe, 0x4c, 0x2e, 0x28, 0x5d, 0x4f, 0x1d, 0xe2, 0xe7, 0xa0, 0xda, 0xf8, 0x96, 0xb0, 0x2f, 0x15, 0xc1, 0xc8, 0xc0, 0x51, 0xa1, 0xe1, 0xfb, 0x42, 0x05, 0xee, 0xa5, 0x66, 0x9d
                    , 0x38, 0x87, 0x61, 0x72, 0xbc, 0x69, 0x36, 0xa3, 0x89, 0xbc, 0x43, 0x1b, 0x14, 0x64, 0xe9, 0xad, 0x79, 0xc8, 0x62, 0xdf, 0x3f, 0xc4, 0xf8, 0x52, 0xfc, 0x5c, 0xae, 0xa1, 0x1a, 0x95, 0xd4, 0x9f
                    , 0xe8, 0x28, 0x6d, 0xc2, 0xbe, 0x9a, 0xc9, 0xa1, 0x20, 0x0e, 0x67, 0x6a, 0x90, 0x53, 0x49, 0x2f, 0xfd, 0x12, 0x69, 0xf8, 0xac, 0xf5, 0x40, 0x19, 0x53, 0x1f, 0x7f, 0x74, 0x89, 0xf5, 0x44, 0xa3
                    , 0x36, 0x4f, 0x19, 0x01, 0x9d, 0x10, 0xaa, 0x81, 0xc2, 0x67, 0x6e, 0x50, 0xab, 0xe2, 0xfb, 0x7a, 0x82, 0xe8, 0x0d, 0x4f, 0xd9, 0x65, 0x37, 0x88, 0x83, 0xa9, 0x83, 0xe3, 0x5c, 0x89, 0x38, 0xfe
                    , 0xf5, 0xac, 0x5a, 0x6f, 0x22, 0x3a, 0xcf, 0x6c, 0x6d, 0x48, 0xd2, 0x1b, 0xf8, 0x18, 0x2c, 0xf0, 0xbb, 0xc3, 0xcb, 0x8b, 0x71, 0xca, 0xbb, 0xa5, 0xdc, 0x6e, 0x14, 0x45, 0x75, 0x1b, 0x19, 0x60
                    , 0xdc, 0x5b, 0xc7, 0x1c, 0x96, 0xba, 0x80, 0x63, 0x6a, 0x55, 0x6d, 0x07, 0xe2, 0xf0, 0x7a, 0x2c, 0x6b, 0xa8, 0xc2, 0x7f, 0xcc, 0xdd, 0x26, 0xb4, 0x20, 0x79, 0x83, 0x74, 0x55, 0x90, 0xbe, 0x8f
                    , 0x9a, 0x4f, 0xd4, 0x9f, 0x65, 0xd4, 0x20, 0x34, 0x1f, 0x5d, 0x2b, 0xc5, 0x18, 0x69, 0xa8, 0xbc, 0x69, 0xbe, 0xa5, 0xb0, 0xde, 0x27, 0xcf, 0x1f, 0x99, 0x75, 0x6c, 0x66, 0xd3, 0x84, 0x89, 0x27
                    , 0x79, 0xe7, 0x22, 0x56, 0xe8, 0x4f, 0x04, 0x2f, 0x4c, 0xf7, 0xde, 0x43, 0x1b, 0x75, 0xe0, 0xbb, 0x86, 0x1f, 0xa0, 0x01, 0xc1, 0x2c, 0x9b, 0x4d, 0xf6, 0x92, 0xdb, 0x96, 0x1e, 0x9c, 0x04, 0x32
                    , 0x62, 0xd6, 0xb2, 0x47, 0xdd, 0xbb, 0x19, 0xde, 0x78, 0x72, 0x00, 0x62, 0x3f, 0xa4, 0x27, 0xf8, 0x78, 0x2b, 0x58, 0x63, 0xd2, 0x93, 0x1c, 0x1d, 0xd1, 0x98, 0x89, 0x4a, 0x69, 0x43, 0x5a, 0x5e
                    , 0x05, 0x93, 0x75, 0x6f, 0x2c, 0x70, 0xb7, 0x05, 0x04, 0xf5, 0xb9, 0x3b, 0xf9, 0xab, 0x8c, 0x3d, 0xc6, 0xbe, 0x06, 0x54, 0xe9, 0x06, 0xb1, 0x5f, 0xdd, 0x31, 0xec, 0xd8, 0x67, 0x6f, 0x38, 0xf5
                    , 0x84, 0xf5, 0x75, 0xf3, 0xa1, 0x58, 0xfa, 0x22, 0x6b, 0x4c, 0xf4, 0xc3, 0xe5, 0xea, 0x25, 0xf6, 0x9f, 0x4b, 0x82, 0x62, 0x50, 0x27, 0x02, 0x8a, 0xf4, 0x00, 0xc4, 0xbc, 0x23, 0x5c, 0x0f, 0x03
                    , 0xed, 0xa1, 0x37, 0x94, 0xcd, 0xcd, 0x33, 0x7a, 0xf8, 0x93, 0x81, 0x21, 0xac, 0x53, 0x35, 0xba, 0x27, 0x0e, 0x11, 0xb6, 0x9d, 0xba, 0x0e, 0x8b, 0x14, 0x6a, 0x71, 0xfc, 0x22, 0x20, 0x5a, 0xe4
                    , 0x3e, 0xbc, 0xf3, 0xd5, 0x60, 0xa2, 0x52, 0xbc, 0x4d, 0xab, 0x06, 0x32, 0x32, 0xb4, 0xd7, 0x41, 0xfa, 0x4f, 0x7d, 0x05, 0xb5, 0x78, 0xa8, 0x3d, 0xaf, 0x9b, 0xef, 0x5e, 0x02, 0xb5, 0x56, 0x20
                    , 0xe0, 0x08, 0xc0, 0x33, 0x49, 0xa8, 0x0d, 0xc0, 0x41, 0x2f, 0x1c, 0x7a, 0xa8, 0x21, 0xbb, 0x3b, 0x76, 0x8b, 0x85, 0x8f, 0x3d, 0x2c, 0x9c, 0x18, 0x84, 0xcd, 0x63, 0x4f, 0x7f, 0xde, 0x31, 0xb1
                    , 0xe6, 0x50, 0xe0, 0xdb, 0xa9, 0xed, 0x89, 0xd4, 0xee, 0xa6, 0x3f, 0xd6, 0x08, 0x29, 0x45, 0xf7, 0x10, 0xbd, 0x4c, 0x04, 0x22, 0xcc, 0x1d, 0x68, 0x54, 0x98, 0xf2, 0x9d, 0x7d, 0x03, 0xdc, 0xff
                    , 0x80, 0xd3, 0x93, 0x48, 0xbe, 0x72, 0x97, 0x4f, 0x60, 0xa8, 0xe4, 0x11, 0x33, 0xfb, 0xf8, 0x15, 0x21, 0x1e, 0xb0, 0x22, 0x7c, 0x20, 0xd2, 0x6e, 0x01, 0xaf, 0x90, 0xd9, 0x47, 0xa9, 0xb5, 0xce
                    , 0xb1, 0xdf, 0xd8, 0xd2, 0xaf, 0x62, 0x80, 0x22, 0x58, 0x9a, 0x39, 0x6c, 0xbb, 0x21, 0x56, 0xf1, 0x1b, 0x1c, 0xba, 0x13, 0x8e, 0x8d, 0xea, 0xc2, 0x5b, 0x69, 0x66, 0xe7, 0x95, 0x75, 0x57, 0x1e
                    , 0xf4, 0x31, 0x97, 0xd5, 0x29, 0x19, 0x5c, 0xf5, 0x98, 0x09, 0x86, 0x40, 0xbf, 0xdd, 0xb6, 0x4d, 0xde, 0x98, 0xc1, 0xa3, 0x92, 0x92, 0xc0, 0x5d, 0xd5, 0x30, 0xc9, 0xab, 0x80, 0x09, 0x7f, 0x14
                    , 0x05, 0x9d, 0x33, 0x11, 0x53, 0xfc, 0xba, 0x9d, 0xbe, 0x1d, 0x33, 0xda, 0xa3, 0x98, 0x6b, 0x74, 0x95, 0x88, 0xba, 0x80, 0x57, 0xc0, 0x07, 0xd9, 0x08, 0x3f, 0x3a, 0x39, 0x5f, 0xb8, 0xe9, 0x61
                    , 0x9a, 0xdd, 0x59, 0xb2, 0x8e, 0xa1, 0xeb, 0x4b, 0x7c, 0xd5, 0x0e, 0x07, 0xed, 0x24, 0x41, 0x34, 0x03, 0x23, 0x75, 0x8e, 0x7e, 0xb7, 0xdb, 0x95, 0xd2, 0xd7, 0x54, 0x56, 0x34, 0xc9, 0x5d, 0xf8
                    , 0xdf, 0xc1, 0x94, 0xaa, 0x59, 0xcc, 0x60, 0xfa, 0xe8, 0x08, 0x2a, 0x1a, 0xdc, 0x03, 0xab, 0xf7, 0xf3, 0x1b, 0xfa, 0x3a, 0xf8, 0x8e, 0x02, 0x8a, 0x2e, 0x2b, 0x50, 0x4b, 0x7d, 0x92, 0x98, 0x81
                    , 0x35, 0x75, 0xba, 0xa3, 0xd5, 0xe9, 0x70, 0x27, 0xce, 0xc2, 0xe9, 0xe8, 0x96, 0xff, 0x94, 0x7c, 0x53, 0xe0, 0x38, 0x0b, 0xcc, 0x8d, 0xfe, 0xce, 0x3a, 0x94, 0x82, 0xc2, 0xca, 0x78, 0xa7, 0x3e
                    , 0x62, 0x32, 0x9b, 0x61, 0xb0, 0x90, 0x1e, 0x09, 0xa0, 0x75, 0x04, 0x43, 0x3a, 0x70, 0x65, 0x04, 0x0c, 0x9f, 0x22, 0xac, 0x0a, 0x7c, 0xe1, 0x62, 0x39, 0x13, 0xab, 0xe4, 0xfc, 0x1d, 0x1b, 0xcf
                    , 0x8f, 0x10, 0x31, 0x00, 0x75, 0xfd, 0x6a, 0x60, 0x8d, 0x67, 0x9d, 0x58, 0x31, 0x1a, 0x74, 0x37, 0x98, 0x19, 0x46, 0x47, 0xb9, 0xa8, 0x8f, 0xbd, 0x39, 0x0e, 0xbd, 0xda, 0x86, 0xbc, 0x97, 0xc4
                    , 0x68, 0x9c, 0xca, 0x82, 0x2c, 0x74, 0xd7, 0x78, 0xa4, 0x69, 0xd1, 0x34, 0x8f, 0x57, 0x43, 0xb9, 0x47, 0x02, 0xe5, 0x29, 0x58, 0xda, 0x51, 0x0c, 0x13, 0x16, 0x24, 0x24, 0x79, 0xbe, 0xb4, 0x6e
                    , 0xf5, 0x5d, 0xc7, 0xd7, 0xe9, 0xab, 0x1a, 0x13, 0x19, 0x97, 0x06, 0x97, 0x8b, 0xf9, 0xd0, 0x37, 0xcb, 0x17, 0x44, 0x85, 0xa9, 0x18, 0x4d, 0x85, 0x76, 0xee, 0x9b, 0xd6, 0x5c, 0x8e, 0x24, 0x9f
                    , 0x5c, 0xf3, 0xc2, 0xed, 0x6e, 0xc5, 0x18, 0xfa, 0xc1, 0x14, 0x5a, 0xf0, 0xe1, 0xc0, 0xdb, 0x26, 0x1f, 0xe4, 0xd9, 0x7d, 0x79, 0xcc, 0xb3, 0x02, 0x72, 0x99, 0x60, 0xa4, 0xec, 0x2f, 0x78, 0xb8
                    , 0xa0, 0xd1, 0xf6, 0x90, 0x03, 0x08, 0x09, 0xde, 0xc4, 0xf9, 0xa6, 0x6f, 0x7f, 0x40, 0x62, 0xa7, 0x40, 0x10, 0xce, 0xd6, 0x90, 0x51, 0x2e, 0x8c, 0x81, 0x57, 0xc6, 0x62, 0x04, 0x87, 0x83, 0xb7
                    , 0x01, 0x11, 0xff, 0xa8, 0xc1, 0xc7, 0x8a, 0xcd, 0x2a, 0xed, 0x96, 0x57, 0xd5, 0x08, 0xd9, 0xbf, 0xd3, 0x66, 0x37, 0xb0, 0x3f, 0x2e, 0x66, 0x1f, 0x4d, 0xc9, 0xdd, 0x95, 0x2d, 0x37, 0x8e, 0xc0
                    , 0xf2, 0xdc, 0xba, 0x66, 0xea, 0x70, 0x3a, 0x71, 0x21, 0x49, 0x00, 0x9c, 0x0c, 0xcd, 0xb3, 0x34, 0x29, 0x11, 0xb6, 0x90, 0xd2, 0x37, 0x0b, 0xfe, 0x5f, 0x5d, 0x5e, 0x5f, 0x7a, 0x7b, 0x21, 0xb9
                    , 0x28, 0x87, 0x0b, 0xd8, 0xc2, 0x41, 0xed, 0x93, 0x64, 0x32, 0xde, 0xbd, 0x6a, 0xaa, 0x1e, 0x35, 0x6a, 0x95, 0xde, 0x53, 0x4d, 0xb9, 0x05, 0xec, 0xba, 0xeb, 0xb1, 0x41, 0x4c, 0xb4, 0xa5, 0xa5
                    , 0x87, 0x9b, 0x54, 0x53, 0xc1, 0x2c, 0x01, 0x86, 0x11, 0x94, 0x5f, 0xfe, 0xc1, 0x45, 0x8d, 0x9d, 0x3d, 0x9d, 0x13, 0x00, 0x00, 0x81, 0x33, 0x07, 0xae, 0x0f, 0xd5, 0x7b, 0xab, 0xa9, 0xd7, 0x24
                    , 0xd3, 0xfe, 0xb3, 0x7e, 0x2f, 0x89, 0x92, 0xbe, 0xbe, 0x3e, 0x27, 0x92, 0xcd, 0x04, 0x52, 0x61, 0x82, 0x2a, 0x95, 0x6e, 0xe4, 0x7b, 0xdc, 0x16, 0x19, 0x3a, 0x88, 0xd2, 0xe7, 0xdf, 0xf7, 0xae
                    , 0x3a, 0xd5, 0x66, 0x91, 0x94, 0x5b, 0x54, 0xa7, 0x9e, 0xf2, 0xd3, 0x51, 0x25, 0x12, 0xf1, 0xd2, 0x1d, 0x8b, 0xd9, 0xf3, 0xf3, 0x75, 0xc4, 0xce, 0xde, 0x09, 0xe4, 0xd5, 0x23, 0x4e, 0x1e, 0xf4
                    , 0x99, 0x60, 0x3d, 0x5d, 0x82, 0xb0, 0x31, 0x2a, 0xbf, 0xf6, 0x10, 0xea, 0xb6, 0xae, 0x18, 0xb8, 0xfb, 0x50, 0xc7, 0x96, 0x28, 0x7e, 0x5f, 0x6f, 0xd5, 0xf5, 0x6d, 0x6b, 0xfe, 0x24, 0x82, 0xe2
                    , 0xbd, 0xee, 0x55, 0xd0, 0x27, 0xc8, 0x28, 0x02, 0x98, 0x41, 0x8d, 0x04, 0x2b, 0x55, 0xc0, 0x57, 0x4c, 0x87, 0x6f, 0x36, 0x84, 0xfb, 0x71, 0x10, 0xae, 0xed, 0x71, 0x06, 0x81, 0x03, 0xcc, 0x90
                    , 0x26, 0xa4, 0xd3, 0x3a, 0x30, 0xe8, 0x60, 0xbf, 0x3c, 0xa7, 0xe1, 0x0d, 0x42, 0x0a, 0xf4, 0x21, 0xbb, 0xf4, 0x82, 0xe5, 0x83, 0x5d, 0x85, 0xa6, 0xe3, 0x2f, 0x9c, 0xfe, 0x36, 0xc6, 0xcb, 0x67
                    , 0x27, 0xcf, 0x86, 0x2d, 0x37, 0xe0, 0x27, 0x20, 0x16, 0x34, 0x6a, 0x5e, 0xd5, 0x56, 0x27, 0x4a, 0x18, 0x95, 0xb8, 0xd5, 0x0c, 0xdc, 0xf1, 0x2e, 0x53, 0xf1, 0x48, 0x6e, 0xdb, 0xe9, 0x62, 0x2e
                    , 0x67, 0x53, 0x1f, 0xca, 0x65, 0x93, 0xab, 0x99, 0xca, 0x00, 0x17, 0x06, 0x87, 0x53, 0x01, 0x09, 0x80, 0xd7, 0x00, 0x07, 0x0b, 0x01, 0x00, 0x01, 0x23, 0x03, 0x01, 0x01, 0x05, 0x5d, 0x00, 0x10
                    , 0x00, 0x00, 0x0c, 0x81, 0x9b, 0x0a, 0x01, 0xa0, 0xee, 0xa0, 0x06, 0x00, 0x00
                };
                Assert (sizeof (ksample_zip_7z_) == 2157);
#if     qHasFeature_LZMA
                _7z::ArchiveReader  reader (Streams::ExternallyOwnedMemoryInputStream<Byte> (begin (ksample_zip_7z_), end (ksample_zip_7z_)));
                VerifyTestResult ((reader.GetContainedFiles () == Set<String> {L"sample_zip/BlockAllocation-Valgrind.supp", L"sample_zip/Common-Valgrind.supp", L"sample_zip/TODO.txt", L"sample_zip/Tests-Description.txt"}));

                {
                    using   Streams::InputStream;
                    using   Streams::TextReader;
                    using   Memory::Byte;
                    VerifyTestResult (reader.GetData (L"sample_zip/TODO.txt").size () == 243);
                    VerifyTestResult (reader.GetData (L"sample_zip/BlockAllocation-Valgrind.supp").size () == 4296);
                    VerifyTestResult (reader.GetData (L"sample_zip/Common-Valgrind.supp").size () == 1661);
                    VerifyTestResult (reader.GetData (L"sample_zip/Tests-Description.txt").size () == 1934);
                    VerifyTestResult (TextReader (reader.GetData (L"sample_zip/TODO.txt").As<InputStream<Byte>> ()).ReadAll ().Contains (L"Once any of the ThreadSafetyBuiltinObject tests work - with the locking stuff - add more concrete tyeps"));
                    VerifyTestResult (TextReader (reader.GetData (L"sample_zip/Tests-Description.txt").As<InputStream<Byte>> ()).ReadAll ().Contains (L"[30]	Foundation::DataExchange::Other"));
                    try {
                        auto i = reader.GetData (L"file-not-found");
                        VerifyTestResult (false);
                    }
                    catch (...) {
                        // good
                    }
                }
#endif
            }
        }
        void    DoAll_ ()
        {
            Private_::ReadHardwired7zFile_ ();
        }
    }
}





namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Atom_ ();
        Test2_OptionsFile_ ();
        Test3_7zArchive_::DoAll_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



